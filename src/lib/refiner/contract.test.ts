import { describe, it } from "node:test";
import assert from "node:assert/strict";
import {
  publicRefinerPolicy,
  refinerRequestSchema,
  refinerResponseSchema,
  validateRefinerResponse,
  type RefinerRequest,
  type RefinerResponse,
} from "./contract.ts";

const REQUEST_ID = "11111111-1111-4111-8111-111111111111";

const request: RefinerRequest = refinerRequestSchema.parse({
  schemaVersion: 1,
  mode: "pro",
  productType: "effect-pedal",
  originalPrompt: "Build a tight modern distortion with 4x oversampling and a 90 Hz low cut.",
  workingPrompt: "Build a tight modern distortion with 4x oversampling and a 90 Hz low cut, optimized for a dense mix.",
  protectedFacts: [
    {
      id: "oversampling",
      label: "Oversampling",
      value: "4x",
      origin: "user",
      mutable: false,
    },
    {
      id: "low-cut",
      label: "Low cut",
      value: "90 Hz",
      origin: "user",
      mutable: false,
    },
  ],
  allowProtectedChangeApplication: false,
});

function validResponse(): RefinerResponse {
  return refinerResponseSchema.parse({
    schemaVersion: 1,
    requestId: REQUEST_ID,
    mode: "pro",
    productType: "effect-pedal",
    originalPrompt: request.originalPrompt,
    inputWorkingPrompt: request.workingPrompt,
    refinedPrompt: "Create a tight modern distortion designed for dense mixes while preserving the requested 4x oversampling and 90 Hz low cut.",
    suggestions: [
      {
        id: "transient-control",
        title: "Clarify transient behavior",
        rationale: "Specify whether the attack should stay hard or become smoother under high input levels.",
      },
    ],
    preservedFacts: request.protectedFacts,
    protectedChangeProposals: [],
    service: {
      name: "FARNAZ",
      contractVersion: "1",
      serviceBuild: "test-build",
      executionBoundary: "server-side",
      browserProviderCall: false,
    },
  });
}

describe("WARLOCK FARNAZ Prompt Refiner contract", () => {
  it("accepts a response bound to the exact original, working prompt and protected facts", () => {
    const response = validateRefinerResponse(request, validResponse());
    assert.equal(response.requestId, REQUEST_ID);
    assert.equal(response.preservedFacts.length, 2);
  });

  it("rejects a response that rewrites the captured original prompt", () => {
    const response = { ...validResponse(), originalPrompt: `${request.originalPrompt} changed` };
    assert.throws(
      () => validateRefinerResponse(request, response),
      /changed the captured original prompt/,
    );
  });

  it("rejects a response that is not bound to the exact working prompt", () => {
    const response = { ...validResponse(), inputWorkingPrompt: request.originalPrompt };
    assert.throws(
      () => validateRefinerResponse(request, response),
      /exact working prompt/,
    );
  });

  it("rejects silent mutation of a protected technical fact", () => {
    const response = validResponse();
    response.preservedFacts = response.preservedFacts.map((fact) =>
      fact.id === "oversampling" ? { ...fact, value: "8x" } : fact,
    );
    assert.throws(
      () => validateRefinerResponse(request, response),
      /mutated protected fact: oversampling/,
    );
  });

  it("allows protected-value changes only as unapplied proposals bound to the current value", () => {
    const response = validResponse();
    response.protectedChangeProposals = [
      {
        factId: "low-cut",
        currentValue: "90 Hz",
        proposedValue: "100 Hz",
        reason: "Could create more kick/bass clearance; user approval required.",
        applied: false,
      },
    ];
    const checked = validateRefinerResponse(request, response);
    assert.equal(checked.protectedChangeProposals[0]?.applied, false);
  });

  it("rejects an applied protected-value change at schema level", () => {
    const response = validResponse();
    assert.throws(() =>
      refinerResponseSchema.parse({
        ...response,
        protectedChangeProposals: [
          {
            factId: "low-cut",
            currentValue: "90 Hz",
            proposedValue: "100 Hz",
            reason: "Not allowed to auto-apply.",
            applied: true,
          },
        ],
      }),
    );
  });

  it("keeps the public website disconnected and browser-provider calls forbidden until service proof exists", () => {
    assert.equal(publicRefinerPolicy.connected, false);
    assert.equal(publicRefinerPolicy.browserProviderCallsAllowed, false);
    assert.equal(publicRefinerPolicy.autoApplyProtectedChanges, false);
    assert.equal(publicRefinerPolicy.stagedFactoryRequestFromUnacceptedRewriteAllowed, false);
  });
});

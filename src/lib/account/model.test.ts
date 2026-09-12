import { describe, it } from "node:test";
import assert from "node:assert/strict";
import {
  accountProductSchema,
  billingSummarySchema,
  buildSummarySchema,
  licenseSummarySchema,
} from "./model.ts";

const HASH = "b".repeat(64);
const ID = "11111111-1111-4111-8111-111111111111";

describe("WARLOCK account contracts", () => {
  it("requires exact artifact identity for ready account products", () => {
    const product = accountProductSchema.parse({
      id: ID,
      productSlug: "void",
      productName: "VOID",
      version: "1.0.0",
      status: "ready",
      latestArtifactSha256: HASH,
    });
    assert.equal(product.latestArtifactSha256, HASH);
  });

  it("keeps build status explicit and artifact identity nullable before ready", () => {
    const build = buildSummarySchema.parse({
      requestId: ID,
      productSlug: null,
      status: "staged",
      createdAt: "2026-09-12T08:00:00.000Z",
      artifactSha256: null,
    });
    assert.equal(build.status, "staged");
  });

  it("enforces the standing two-device perpetual license target", () => {
    const license = licenseSummarySchema.parse({
      id: ID,
      productSlug: "void",
      state: "active",
      maxDevices: 2,
      activeDevices: 1,
      perpetual: true,
    });
    assert.equal(license.maxDevices, 2);
    assert.equal(license.perpetual, true);
  });

  it("represents disconnected billing without inventing a plan", () => {
    const billing = billingSummarySchema.parse({
      liveBillingConnected: false,
      planName: null,
      buildsRemaining: null,
      renewalAt: null,
    });
    assert.equal(billing.liveBillingConnected, false);
  });
});

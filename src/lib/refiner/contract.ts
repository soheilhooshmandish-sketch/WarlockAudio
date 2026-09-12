import { z } from "zod";

const factId = /^[a-z0-9]+(?:-[a-z0-9]+)*$/;

export const refinerModeSchema = z.enum(["refine", "enhance", "pro"]);

export const protectedFactSchema = z.object({
  id: z.string().regex(factId),
  label: z.string().min(1).max(80),
  value: z.string().min(1).max(240),
  origin: z.enum(["user", "productspec"]),
  mutable: z.literal(false),
});

export const refinerRequestSchema = z.object({
  schemaVersion: z.literal(1),
  mode: refinerModeSchema,
  productType: z.literal("effect-pedal"),
  originalPrompt: z.string().min(12).max(2000),
  workingPrompt: z.string().min(12).max(2000),
  protectedFacts: z.array(protectedFactSchema).max(64),
  allowProtectedChangeApplication: z.literal(false),
});

const suggestionSchema = z.object({
  id: z.string().regex(factId),
  title: z.string().min(1).max(100),
  rationale: z.string().min(1).max(600),
});

const protectedChangeProposalSchema = z.object({
  factId: z.string().regex(factId),
  currentValue: z.string().min(1).max(240),
  proposedValue: z.string().min(1).max(240),
  reason: z.string().min(1).max(600),
  applied: z.literal(false),
});

export const refinerResponseSchema = z.object({
  schemaVersion: z.literal(1),
  requestId: z.string().uuid(),
  mode: refinerModeSchema,
  productType: z.literal("effect-pedal"),
  originalPrompt: z.string().min(12).max(2000),
  inputWorkingPrompt: z.string().min(12).max(2000),
  refinedPrompt: z.string().min(12).max(4000),
  suggestions: z.array(suggestionSchema).max(24),
  preservedFacts: z.array(protectedFactSchema).max(64),
  protectedChangeProposals: z.array(protectedChangeProposalSchema).max(24),
  service: z.object({
    name: z.literal("FARNAZ"),
    contractVersion: z.literal("1"),
    serviceBuild: z.string().min(1).max(120),
    executionBoundary: z.literal("server-side"),
    browserProviderCall: z.literal(false),
  }),
});

export type RefinerRequest = z.infer<typeof refinerRequestSchema>;
export type RefinerResponse = z.infer<typeof refinerResponseSchema>;
export type ProtectedFact = z.infer<typeof protectedFactSchema>;
export type RefinerMode = z.infer<typeof refinerModeSchema>;

/**
 * Public website policy while the real FARNAZ service is not connected.
 * A UI may expose the contract/state, but it must not fabricate a rewrite.
 */
export const publicRefinerPolicy = {
  connected: false,
  browserProviderCallsAllowed: false,
  autoApplyProtectedChanges: false,
  stagedFactoryRequestFromUnacceptedRewriteAllowed: false,
} as const;

/**
 * Fail-closed semantic validation beyond structural Zod parsing.
 * Protected facts are immutable input facts. FARNAZ may suggest changing one,
 * but a suggestion must stay a proposal and cannot be silently applied.
 */
export function validateRefinerResponse(
  rawRequest: RefinerRequest,
  rawResponse: RefinerResponse,
): RefinerResponse {
  const request = refinerRequestSchema.parse(rawRequest);
  const response = refinerResponseSchema.parse(rawResponse);

  if (response.mode !== request.mode) {
    throw new Error("Refiner response mode does not match the request");
  }
  if (response.productType !== request.productType) {
    throw new Error("Refiner response product type does not match the request");
  }
  if (response.originalPrompt !== request.originalPrompt) {
    throw new Error("Refiner response changed the captured original prompt");
  }
  if (response.inputWorkingPrompt !== request.workingPrompt) {
    throw new Error("Refiner response does not bind to the exact working prompt");
  }

  const expectedFacts = new Map(request.protectedFacts.map((fact) => [fact.id, fact]));
  const preservedFacts = new Map(response.preservedFacts.map((fact) => [fact.id, fact]));

  if (preservedFacts.size !== expectedFacts.size) {
    throw new Error("Refiner response did not preserve the exact protected-fact set");
  }

  for (const [id, expected] of expectedFacts) {
    const actual = preservedFacts.get(id);
    if (!actual) {
      throw new Error(`Refiner response dropped protected fact: ${id}`);
    }
    if (
      actual.value !== expected.value ||
      actual.label !== expected.label ||
      actual.origin !== expected.origin ||
      actual.mutable !== false
    ) {
      throw new Error(`Refiner response mutated protected fact: ${id}`);
    }
  }

  for (const proposal of response.protectedChangeProposals) {
    const fact = expectedFacts.get(proposal.factId);
    if (!fact) {
      throw new Error(`Refiner proposed a change for unknown protected fact: ${proposal.factId}`);
    }
    if (proposal.currentValue !== fact.value) {
      throw new Error(`Refiner proposal is not bound to the current protected value: ${proposal.factId}`);
    }
    if (proposal.proposedValue === fact.value) {
      throw new Error(`Refiner protected-change proposal does not actually change anything: ${proposal.factId}`);
    }
  }

  return response;
}

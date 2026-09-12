import { z } from "zod";

export const publicHealthStateSchema = z.enum([
  "healthy",
  "degraded",
  "blocked",
  "unknown",
]);

export const publicHealthComponentSchema = z.object({
  id: z.enum(["website", "factory", "release", "account", "commerce"]),
  label: z.string().min(1).max(80),
  state: publicHealthStateSchema,
  reason: z.string().min(1).max(240),
  evidence: z.string().max(240).nullable(),
  nextAction: z.string().max(240).nullable(),
  observedAt: z.string().datetime().nullable(),
  source: z.enum(["live-telemetry", "verified-build-contract", "not-connected"]),
});

export const publicHealthSnapshotSchema = z
  .object({
    schemaVersion: z.literal(1),
    overall: publicHealthStateSchema,
    liveTelemetryConnected: z.boolean(),
    components: z.array(publicHealthComponentSchema).min(1),
  })
  .superRefine((value, ctx) => {
    if (!value.liveTelemetryConnected) {
      for (const [index, component] of value.components.entries()) {
        if (component.source === "live-telemetry") {
          ctx.addIssue({
            code: "custom",
            path: ["components", index, "source"],
            message: "Live telemetry source is impossible while telemetry is disconnected",
          });
        }
      }
      if (value.overall === "healthy") {
        ctx.addIssue({
          code: "custom",
          path: ["overall"],
          message: "Disconnected telemetry cannot claim overall healthy",
        });
      }
    }
  });

export type PublicHealthSnapshot = z.infer<typeof publicHealthSnapshotSchema>;

/**
 * Public V1 snapshot is deliberately non-live. It reports only the contracts
 * that are safe to state from the website build itself and refuses to infer
 * operational health for Factory, releases, billing, or account backends.
 */
export const publicHealthSnapshot: PublicHealthSnapshot = publicHealthSnapshotSchema.parse({
  schemaVersion: 1,
  overall: "unknown",
  liveTelemetryConnected: false,
  components: [
    {
      id: "website",
      label: "Website quality contract",
      state: "degraded",
      reason: "This site was built through strict dependency, type, contract-test and production-build gates, but live uptime telemetry is not connected.",
      evidence: "Verified build contract; not a live uptime probe.",
      nextAction: "Connect an authenticated runtime health source before claiming live availability.",
      observedAt: null,
      source: "verified-build-contract",
    },
    {
      id: "factory",
      label: "WARLOCK Factory",
      state: "unknown",
      reason: "Public website telemetry is not connected to the private Factory control plane.",
      evidence: null,
      nextAction: "Expose only an authenticated, redacted health summary from the Factory gateway.",
      observedAt: null,
      source: "not-connected",
    },
    {
      id: "release",
      label: "Commercial release pipeline",
      state: "unknown",
      reason: "Product release readiness is evaluated inside Factory evidence and is not mirrored live to the public site.",
      evidence: null,
      nextAction: "Publish product-specific readiness only from verified release evidence.",
      observedAt: null,
      source: "not-connected",
    },
    {
      id: "account",
      label: "Account data services",
      state: "unknown",
      reason: "Authentication UI exists, but customer product/build/license records are not represented as live health telemetry.",
      evidence: null,
      nextAction: "Bind account services to a health endpoint before exposing availability claims.",
      observedAt: null,
      source: "not-connected",
    },
    {
      id: "commerce",
      label: "Billing and commerce",
      state: "blocked",
      reason: "Live billing and checkout are intentionally not connected.",
      evidence: "Account billing model is fail-closed with liveBillingConnected=false.",
      nextAction: "Complete sandbox commerce before any production payment path is enabled.",
      observedAt: null,
      source: "verified-build-contract",
    },
  ],
});

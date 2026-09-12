import { z } from "zod";

const semver = /^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?(?:\+[0-9A-Za-z.-]+)?$/;
const sha256 = /^[0-9a-f]{64}$/;
const slug = /^[a-z0-9]+(?:-[a-z0-9]+)*$/;

export const accountProductSchema = z
  .object({
    id: z.string().uuid(),
    productSlug: z.string().regex(slug),
    productName: z.string().min(1).max(80),
    version: z.string().regex(semver),
    status: z.enum(["candidate", "blocked", "ready"]),
    latestArtifactSha256: z.string().regex(sha256).nullable(),
  })
  .superRefine((value, ctx) => {
    if (value.status === "ready" && !value.latestArtifactSha256) {
      ctx.addIssue({
        code: "custom",
        path: ["latestArtifactSha256"],
        message: "Ready products require exact artifact identity",
      });
    }
  });

export const buildSummarySchema = z
  .object({
    requestId: z.string().uuid(),
    productSlug: z.string().regex(slug).nullable(),
    status: z.enum([
      "staged",
      "queued",
      "building",
      "validating",
      "packaging",
      "ready",
      "failed",
      "blocked",
      "cancelled",
    ]),
    createdAt: z.string().datetime(),
    artifactSha256: z.string().regex(sha256).nullable(),
  })
  .superRefine((value, ctx) => {
    if (value.status === "ready" && !value.artifactSha256) {
      ctx.addIssue({
        code: "custom",
        path: ["artifactSha256"],
        message: "Ready builds require exact artifact identity",
      });
    }
  });

export const licenseSummarySchema = z.object({
  id: z.string().uuid(),
  productSlug: z.string().regex(slug),
  state: z.enum(["evaluation", "active", "revoked", "expired"]),
  maxDevices: z.literal(2),
  activeDevices: z.number().int().min(0).max(2),
  perpetual: z.literal(true),
});

export const billingSummarySchema = z
  .object({
    liveBillingConnected: z.boolean(),
    planName: z.string().nullable(),
    buildsRemaining: z.number().int().nonnegative().nullable(),
    renewalAt: z.string().datetime().nullable(),
  })
  .superRefine((value, ctx) => {
    if (
      !value.liveBillingConnected &&
      (value.planName !== null || value.buildsRemaining !== null || value.renewalAt !== null)
    ) {
      ctx.addIssue({
        code: "custom",
        message: "Disconnected billing cannot expose live-plan state",
      });
    }
  });

export type AccountProduct = z.infer<typeof accountProductSchema>;
export type BuildSummary = z.infer<typeof buildSummarySchema>;
export type LicenseSummary = z.infer<typeof licenseSummarySchema>;
export type BillingSummary = z.infer<typeof billingSummarySchema>;

/**
 * V1 account UI is deliberately honest: no customer records are fabricated
 * until a real account-bound backend provides them.
 */
export const accountProducts: readonly AccountProduct[] = [];
export const accountBuilds: readonly BuildSummary[] = [];
export const accountLicenses: readonly LicenseSummary[] = [];
export const billingSummary: BillingSummary = {
  liveBillingConnected: false,
  planName: null,
  buildsRemaining: null,
  renewalAt: null,
};

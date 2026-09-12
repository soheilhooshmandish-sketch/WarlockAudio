import { z } from "zod";

const semver = /^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?(?:\+[0-9A-Za-z.-]+)?$/;
const sha256 = /^[0-9a-f]{64}$/;
const assetId = /^[a-z0-9]+(?:-[a-z0-9]+)*$/;

export const assetStatusSchema = z.enum([
  "planned",
  "beta",
  "available",
  "retired",
]);

export const presetPackSchema = z.object({
  schemaVersion: z.literal(1),
  id: z.string().regex(assetId),
  name: z.string().min(1).max(80),
  vendor: z.string().min(1).max(80),
  version: z.string().regex(semver),
  status: assetStatusSchema,
  productCompatibility: z.array(z.string().regex(assetId)).min(1),
  minimumProductVersion: z.string().regex(semver),
  parameterSchemaVersion: z.number().int().positive(),
  presetCount: z.number().int().positive().max(512),
  author: z.string().min(1).max(80),
  contentSha256: z.string().regex(sha256),
});

export const skinSchema = z.object({
  schemaVersion: z.literal(1),
  id: z.string().regex(assetId),
  name: z.string().min(1).max(80),
  vendor: z.string().min(1).max(80),
  version: z.string().regex(semver),
  status: assetStatusSchema,
  productCompatibility: z.array(z.string().regex(assetId)).min(1),
  minimumProductVersion: z.string().regex(semver),
  brandGeometryVersion: z.string().min(1).max(40),
  materialFamily: z.enum([
    "metal",
    "frost",
    "glass",
    "biomechanical",
    "cosmic",
    "industrial",
    "organic",
    "other",
  ]),
  colorTokens: z.array(z.string().regex(/^#[0-9A-Fa-f]{6}$/)).max(12),
  contentSha256: z.string().regex(sha256),
});

export const skinUploadPolicy = {
  maxBytes: 12 * 1024 * 1024,
  allowedRasterMimeTypes: ["image/png", "image/jpeg", "image/webp"] as const,
  svg: {
    allowed: false,
    reason: "Scriptable or externally-referencing SVG is rejected until a dedicated sanitizer exists.",
  },
  executableContentAllowed: false,
  externalReferencesAllowed: false,
} as const;

export type PresetPack = z.infer<typeof presetPackSchema>;
export type Skin = z.infer<typeof skinSchema>;

/**
 * Public commercial catalogs intentionally start empty. A pack or skin may be
 * added only when its compatibility, version and content hash are real.
 */
export const presetPacks: readonly PresetPack[] = [];
export const skins: readonly Skin[] = [];

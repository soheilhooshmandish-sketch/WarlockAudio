import { describe, it } from "node:test";
import assert from "node:assert/strict";
import {
  presetPackSchema,
  skinSchema,
  skinUploadPolicy,
} from "./catalog.ts";

const HASH = "a".repeat(64);

describe("WARLOCK distributable asset contracts", () => {
  it("accepts a version-bound preset pack with compatibility and content hash", () => {
    const result = presetPackSchema.parse({
      schemaVersion: 1,
      id: "void-black-rituals",
      name: "VOID Black Rituals",
      vendor: "WARLOCK PLUGINS",
      version: "1.0.0",
      status: "beta",
      productCompatibility: ["void"],
      minimumProductVersion: "0.1.0",
      parameterSchemaVersion: 1,
      presetCount: 13,
      author: "WARLOCK PLUGINS",
      contentSha256: HASH,
    });
    assert.equal(result.presetCount, 13);
  });

  it("rejects a preset pack without exact content identity", () => {
    assert.throws(() =>
      presetPackSchema.parse({
        schemaVersion: 1,
        id: "bad-pack",
        name: "Bad Pack",
        vendor: "WARLOCK PLUGINS",
        version: "1.0.0",
        status: "available",
        productCompatibility: ["void"],
        minimumProductVersion: "0.1.0",
        parameterSchemaVersion: 1,
        presetCount: 1,
        author: "WARLOCK PLUGINS",
        contentSha256: "not-a-hash",
      }),
    );
  });

  it("requires skins to bind to canonical brand geometry metadata", () => {
    const result = skinSchema.parse({
      schemaVersion: 1,
      id: "void-frost",
      name: "VOID Frost",
      vendor: "WARLOCK PLUGINS",
      version: "1.0.0",
      status: "beta",
      productCompatibility: ["void"],
      minimumProductVersion: "0.1.0",
      brandGeometryVersion: "warlock-master-v1",
      materialFamily: "frost",
      colorTokens: ["#07090C", "#8EC8E8"],
      contentSha256: HASH,
    });
    assert.equal(result.brandGeometryVersion, "warlock-master-v1");
  });

  it("keeps public skin uploads fail-closed for executable vector content", () => {
    assert.equal(skinUploadPolicy.svg.allowed, false);
    assert.equal(skinUploadPolicy.executableContentAllowed, false);
    assert.equal(skinUploadPolicy.externalReferencesAllowed, false);
    assert.ok(skinUploadPolicy.maxBytes > 0);
  });
});

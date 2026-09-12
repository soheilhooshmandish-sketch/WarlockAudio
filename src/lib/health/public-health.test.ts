import { describe, it } from "node:test";
import assert from "node:assert/strict";
import {
  publicHealthSnapshot,
  publicHealthSnapshotSchema,
} from "./public-health.ts";

describe("WARLOCK public health model", () => {
  it("does not claim overall healthy while live telemetry is disconnected", () => {
    assert.equal(publicHealthSnapshot.liveTelemetryConnected, false);
    assert.notEqual(publicHealthSnapshot.overall, "healthy");
  });

  it("rejects live telemetry component claims when telemetry is disconnected", () => {
    assert.throws(() =>
      publicHealthSnapshotSchema.parse({
        schemaVersion: 1,
        overall: "unknown",
        liveTelemetryConnected: false,
        components: [
          {
            id: "factory",
            label: "Factory",
            state: "healthy",
            reason: "Claimed live",
            evidence: "probe",
            nextAction: null,
            observedAt: "2026-09-12T08:00:00.000Z",
            source: "live-telemetry",
          },
        ],
      }),
    );
  });

  it("rejects overall healthy without live telemetry", () => {
    assert.throws(() =>
      publicHealthSnapshotSchema.parse({
        schemaVersion: 1,
        overall: "healthy",
        liveTelemetryConnected: false,
        components: [
          {
            id: "website",
            label: "Website",
            state: "degraded",
            reason: "Build contract only",
            evidence: null,
            nextAction: null,
            observedAt: null,
            source: "verified-build-contract",
          },
        ],
      }),
    );
  });
});

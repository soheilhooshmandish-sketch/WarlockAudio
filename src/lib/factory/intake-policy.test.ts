import assert from "node:assert/strict";
import { describe, it } from "node:test";
import { resolveFactoryIntakePolicy } from "./intake-policy.ts";

const remoteBase = {
  WARLOCK_FACTORY_PUBLIC_INTAKE_ENABLED: "true",
  WARLOCK_FACTORY_GATEWAY_URL: "https://agent.warlock-plugins.com/",
  WARLOCK_FACTORY_INTAKE_TOKEN: "intake-secret",
  WARLOCK_CF_ACCESS_CLIENT_ID: "service-id",
  WARLOCK_CF_ACCESS_CLIENT_SECRET: "service-secret",
};

describe("resolveFactoryIntakePolicy", () => {
  it("fails closed unless public intake is explicitly armed", () => {
    const result = resolveFactoryIntakePolicy({
      ...remoteBase,
      WARLOCK_FACTORY_PUBLIC_INTAKE_ENABLED: "false",
    });
    assert.deepEqual(result, { ok: false, reason: "disabled" });
  });

  it("rejects insecure non-local HTTP gateways", () => {
    const result = resolveFactoryIntakePolicy({
      ...remoteBase,
      WARLOCK_FACTORY_GATEWAY_URL: "http://agent.warlock-plugins.com",
    });
    assert.deepEqual(result, { ok: false, reason: "invalid_gateway" });
  });

  it("allows local HTTP development without Cloudflare service credentials", () => {
    const result = resolveFactoryIntakePolicy({
      WARLOCK_FACTORY_PUBLIC_INTAKE_ENABLED: "true",
      WARLOCK_FACTORY_GATEWAY_URL: "http://127.0.0.1:8780/",
      WARLOCK_FACTORY_INTAKE_TOKEN: "local-secret",
    });
    assert.equal(result.ok, true);
    if (!result.ok) return;
    assert.equal(result.config.baseUrl, "http://127.0.0.1:8780");
    assert.equal(result.config.local, true);
    assert.equal(result.config.cfClientId, "");
    assert.equal(result.config.cfClientSecret, "");
  });

  it("requires both Cloudflare Access service credentials for remote gateways", () => {
    const result = resolveFactoryIntakePolicy({
      ...remoteBase,
      WARLOCK_CF_ACCESS_CLIENT_SECRET: "",
    });
    assert.deepEqual(result, {
      ok: false,
      reason: "missing_cloudflare_access",
    });
  });

  it("accepts the armed HTTPS production shape and normalizes the base URL", () => {
    const result = resolveFactoryIntakePolicy(remoteBase);
    assert.equal(result.ok, true);
    if (!result.ok) return;
    assert.equal(result.config.baseUrl, "https://agent.warlock-plugins.com");
    assert.equal(result.config.local, false);
    assert.equal(result.config.intakeToken, "intake-secret");
    assert.equal(result.config.cfClientId, "service-id");
    assert.equal(result.config.cfClientSecret, "service-secret");
  });

  it("rejects gateway URLs containing embedded credentials, query strings, or fragments", () => {
    for (const gateway of [
      "https://user:pass@agent.warlock-plugins.com",
      "https://agent.warlock-plugins.com?token=bad",
      "https://agent.warlock-plugins.com#fragment",
    ]) {
      const result = resolveFactoryIntakePolicy({
        ...remoteBase,
        WARLOCK_FACTORY_GATEWAY_URL: gateway,
      });
      assert.deepEqual(result, { ok: false, reason: "invalid_gateway" });
    }
  });
});

import { describe, it } from "node:test";
import assert from "node:assert/strict";
import {
  applySandboxCommerceEvent,
  newSandboxCommerceSession,
  sandboxCommerceTruth,
} from "./sandbox.ts";

const SESSION = "11111111-1111-4111-8111-111111111111";
const ACCOUNT = "22222222-2222-4222-8222-222222222222";
const ENTITLEMENT = "33333333-3333-4333-8333-333333333333";
const LICENSE = "44444444-4444-4444-8444-444444444444";
const HASH = "c".repeat(64);

function base() {
  return newSandboxCommerceSession({
    sessionId: SESSION,
    accountId: ACCOUNT,
    productSlug: "void",
    productVersion: "1.0.0",
  });
}

describe("WARLOCK sandbox commerce", () => {
  it("runs the entire fake-sale lifecycle without moving real money", () => {
    let session = base();
    session = applySandboxCommerceEvent(session, { type: "CREATE_CHECKOUT" });
    session = applySandboxCommerceEvent(session, { type: "SIMULATE_PAYMENT_SUCCESS" });
    session = applySandboxCommerceEvent(session, { type: "RECORD_ORDER" });
    session = applySandboxCommerceEvent(session, {
      type: "GRANT_ENTITLEMENT",
      entitlementId: ENTITLEMENT,
    });
    session = applySandboxCommerceEvent(session, {
      type: "ISSUE_TEST_LICENSE",
      licenseId: LICENSE,
    });
    session = applySandboxCommerceEvent(session, {
      type: "BIND_TEST_ARTIFACT",
      artifactSha256: HASH,
    });
    session = applySandboxCommerceEvent(session, { type: "MARK_INSTALLED" });
    session = applySandboxCommerceEvent(session, { type: "ACTIVATE_DEVICE" });
    session = applySandboxCommerceEvent(session, { type: "ACTIVATE_DEVICE" });

    assert.equal(session.state, "device_2_active");
    assert.equal(session.activeDevices, 2);
    assert.equal(session.artifactSha256, HASH);
    assert.equal(session.entitlementId, ENTITLEMENT);
    assert.equal(session.licenseId, LICENSE);
    assert.equal(session.truth.sandboxOnly, true);
    assert.equal(session.truth.realMoneyMoved, false);
    assert.equal(session.truth.livePaymentProviderCalled, false);
    assert.equal(session.truth.customerContacted, false);
    assert.equal(session.truth.publicReleasePublished, false);

    session = applySandboxCommerceEvent(session, { type: "REVOKE_TEST_LICENSE" });
    assert.equal(session.state, "revoked");
    assert.equal(session.activeDevices, 0);

    session = applySandboxCommerceEvent(session, { type: "SIMULATE_REFUND" });
    assert.equal(session.state, "refunded");
    assert.equal(session.truth.realMoneyMoved, false);
  });

  it("rejects impossible or out-of-order commerce events", () => {
    const session = base();
    assert.throws(() =>
      applySandboxCommerceEvent(session, {
        type: "ISSUE_TEST_LICENSE",
        licenseId: LICENSE,
      }),
    );
  });

  it("requires exact artifact identity before download/install", () => {
    let session = base();
    session = applySandboxCommerceEvent(session, { type: "CREATE_CHECKOUT" });
    session = applySandboxCommerceEvent(session, { type: "SIMULATE_PAYMENT_SUCCESS" });
    session = applySandboxCommerceEvent(session, { type: "RECORD_ORDER" });
    session = applySandboxCommerceEvent(session, {
      type: "GRANT_ENTITLEMENT",
      entitlementId: ENTITLEMENT,
    });
    session = applySandboxCommerceEvent(session, {
      type: "ISSUE_TEST_LICENSE",
      licenseId: LICENSE,
    });

    assert.throws(() =>
      applySandboxCommerceEvent(session, {
        type: "BIND_TEST_ARTIFACT",
        artifactSha256: "not-a-hash",
      }),
    );
  });

  it("never allows a third active device", () => {
    let session = base();
    for (const event of [
      { type: "CREATE_CHECKOUT" } as const,
      { type: "SIMULATE_PAYMENT_SUCCESS" } as const,
      { type: "RECORD_ORDER" } as const,
      { type: "GRANT_ENTITLEMENT", entitlementId: ENTITLEMENT } as const,
      { type: "ISSUE_TEST_LICENSE", licenseId: LICENSE } as const,
      { type: "BIND_TEST_ARTIFACT", artifactSha256: HASH } as const,
      { type: "MARK_INSTALLED" } as const,
      { type: "ACTIVATE_DEVICE" } as const,
      { type: "ACTIVATE_DEVICE" } as const,
    ]) {
      session = applySandboxCommerceEvent(session, event);
    }

    assert.equal(session.activeDevices, 2);
    assert.throws(() => applySandboxCommerceEvent(session, { type: "ACTIVATE_DEVICE" }));
  });

  it("documents that the sandbox engine has no production side effects", () => {
    assert.equal(sandboxCommerceTruth.executesNetworkCalls, false);
    assert.equal(sandboxCommerceTruth.movesRealMoney, false);
    assert.equal(sandboxCommerceTruth.issuesProductionLicenses, false);
    assert.equal(sandboxCommerceTruth.grantsProductionDownloads, false);
    assert.equal(sandboxCommerceTruth.contactsCustomers, false);
  });
});

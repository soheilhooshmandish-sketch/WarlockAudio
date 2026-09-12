import { z } from "zod";

const id = z.string().uuid();
const sha256 = z.string().regex(/^[0-9a-f]{64}$/);

export const sandboxCommerceStateSchema = z.enum([
  "cart",
  "checkout",
  "payment_succeeded",
  "order_recorded",
  "entitled",
  "license_issued",
  "download_ready",
  "installed",
  "device_1_active",
  "device_2_active",
  "revoked",
  "refunded",
]);

export const sandboxCommerceSessionSchema = z.object({
  schemaVersion: z.literal(1),
  mode: z.literal("sandbox"),
  sessionId: id,
  accountId: id,
  productSlug: z.string().regex(/^[a-z0-9]+(?:-[a-z0-9]+)*$/),
  productVersion: z.string().regex(/^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?(?:\+[0-9A-Za-z.-]+)?$/),
  state: sandboxCommerceStateSchema,
  artifactSha256: sha256.nullable(),
  entitlementId: id.nullable(),
  licenseId: id.nullable(),
  activeDevices: z.number().int().min(0).max(2),
  truth: z.object({
    sandboxOnly: z.literal(true),
    realMoneyMoved: z.literal(false),
    livePaymentProviderCalled: z.literal(false),
    customerContacted: z.literal(false),
    publicReleasePublished: z.literal(false),
  }),
});

export type SandboxCommerceSession = z.infer<typeof sandboxCommerceSessionSchema>;
export type SandboxCommerceState = z.infer<typeof sandboxCommerceStateSchema>;

export type SandboxCommerceEvent =
  | { type: "CREATE_CHECKOUT" }
  | { type: "SIMULATE_PAYMENT_SUCCESS" }
  | { type: "RECORD_ORDER" }
  | { type: "GRANT_ENTITLEMENT"; entitlementId: string }
  | { type: "ISSUE_TEST_LICENSE"; licenseId: string }
  | { type: "BIND_TEST_ARTIFACT"; artifactSha256: string }
  | { type: "MARK_INSTALLED" }
  | { type: "ACTIVATE_DEVICE" }
  | { type: "REVOKE_TEST_LICENSE" }
  | { type: "SIMULATE_REFUND" };

const allowed: Record<SandboxCommerceState, readonly SandboxCommerceEvent["type"][]> = {
  cart: ["CREATE_CHECKOUT"],
  checkout: ["SIMULATE_PAYMENT_SUCCESS"],
  payment_succeeded: ["RECORD_ORDER"],
  order_recorded: ["GRANT_ENTITLEMENT"],
  entitled: ["ISSUE_TEST_LICENSE"],
  license_issued: ["BIND_TEST_ARTIFACT"],
  download_ready: ["MARK_INSTALLED"],
  installed: ["ACTIVATE_DEVICE", "SIMULATE_REFUND"],
  device_1_active: ["ACTIVATE_DEVICE", "REVOKE_TEST_LICENSE", "SIMULATE_REFUND"],
  device_2_active: ["REVOKE_TEST_LICENSE", "SIMULATE_REFUND"],
  revoked: ["SIMULATE_REFUND"],
  refunded: [],
};

function assertUuid(value: string, label: string): string {
  const result = id.safeParse(value);
  if (!result.success) throw new Error(`${label} must be a UUID`);
  return result.data;
}

function assertHash(value: string): string {
  const result = sha256.safeParse(value);
  if (!result.success) throw new Error("artifactSha256 must be a SHA256 hex digest");
  return result.data;
}

export function newSandboxCommerceSession(input: {
  sessionId: string;
  accountId: string;
  productSlug: string;
  productVersion: string;
}): SandboxCommerceSession {
  return sandboxCommerceSessionSchema.parse({
    schemaVersion: 1,
    mode: "sandbox",
    sessionId: input.sessionId,
    accountId: input.accountId,
    productSlug: input.productSlug,
    productVersion: input.productVersion,
    state: "cart",
    artifactSha256: null,
    entitlementId: null,
    licenseId: null,
    activeDevices: 0,
    truth: {
      sandboxOnly: true,
      realMoneyMoved: false,
      livePaymentProviderCalled: false,
      customerContacted: false,
      publicReleasePublished: false,
    },
  });
}

export function applySandboxCommerceEvent(
  current: SandboxCommerceSession,
  event: SandboxCommerceEvent,
): SandboxCommerceSession {
  const session = sandboxCommerceSessionSchema.parse(current);
  if (!allowed[session.state].includes(event.type)) {
    throw new Error(`Event ${event.type} is not allowed from ${session.state}`);
  }

  let next: SandboxCommerceSession = session;

  switch (event.type) {
    case "CREATE_CHECKOUT":
      next = { ...session, state: "checkout" };
      break;
    case "SIMULATE_PAYMENT_SUCCESS":
      next = { ...session, state: "payment_succeeded" };
      break;
    case "RECORD_ORDER":
      next = { ...session, state: "order_recorded" };
      break;
    case "GRANT_ENTITLEMENT":
      next = {
        ...session,
        state: "entitled",
        entitlementId: assertUuid(event.entitlementId, "entitlementId"),
      };
      break;
    case "ISSUE_TEST_LICENSE":
      if (!session.entitlementId) throw new Error("Entitlement is required before test license issuance");
      next = {
        ...session,
        state: "license_issued",
        licenseId: assertUuid(event.licenseId, "licenseId"),
      };
      break;
    case "BIND_TEST_ARTIFACT":
      if (!session.licenseId) throw new Error("Test license is required before artifact delivery");
      next = {
        ...session,
        state: "download_ready",
        artifactSha256: assertHash(event.artifactSha256),
      };
      break;
    case "MARK_INSTALLED":
      if (!session.artifactSha256) throw new Error("Exact artifact identity is required before install proof");
      next = { ...session, state: "installed" };
      break;
    case "ACTIVATE_DEVICE": {
      if (!session.licenseId) throw new Error("Test license is required before activation");
      const activeDevices = session.activeDevices + 1;
      if (activeDevices > 2) throw new Error("WARLOCK test license cannot exceed two active devices");
      next = {
        ...session,
        activeDevices,
        state: activeDevices === 1 ? "device_1_active" : "device_2_active",
      };
      break;
    }
    case "REVOKE_TEST_LICENSE":
      if (!session.licenseId) throw new Error("No test license exists to revoke");
      next = { ...session, state: "revoked", activeDevices: 0 };
      break;
    case "SIMULATE_REFUND":
      next = { ...session, state: "refunded", activeDevices: 0 };
      break;
  }

  return sandboxCommerceSessionSchema.parse(next);
}

export const sandboxCommerceTruth = {
  executesNetworkCalls: false,
  movesRealMoney: false,
  issuesProductionLicenses: false,
  grantsProductionDownloads: false,
  contactsCustomers: false,
} as const;

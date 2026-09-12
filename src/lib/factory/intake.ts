import { createServerFn } from "@tanstack/react-start";
import { z } from "zod";

const requestSchema = z.object({
  prompt: z.string().trim().min(12).max(2000),
  clientRequestId: z.string().uuid(),
});

const stagedSchema = z.object({
  schema_version: z.literal(1),
  request_id: z.string().uuid(),
  state: z.literal("staged"),
  source: z.literal("website"),
  received_at: z.string(),
  prompt_sha256: z.string().regex(/^[0-9a-f]{64}$/),
  prompt_length: z.number().int().nonnegative(),
  idempotent: z.boolean(),
  truth: z.object({
    request_validated: z.literal(true),
    build_started: z.literal(false),
    paid_api_called: z.literal(false),
    money_moved: z.literal(false),
    license_issued: z.literal(false),
    customer_contacted: z.literal(false),
    public_release_published: z.literal(false),
    commercial_distribution_allowed: z.literal(false),
  }),
});

type FailureCode =
  | "not_configured"
  | "gateway_auth_failed"
  | "request_conflict"
  | "factory_unavailable"
  | "invalid_factory_response";

type GatewayConfig = {
  baseUrl: string;
  local: boolean;
};

function failure(code: FailureCode, message: string) {
  return { ok: false as const, code, message };
}

function publicIntakeEnabled(): boolean {
  return (
    (process.env.WARLOCK_FACTORY_PUBLIC_INTAKE_ENABLED ?? "")
      .trim()
      .toLowerCase() === "true"
  );
}

function gatewayConfig(): GatewayConfig | null {
  const raw = (process.env.WARLOCK_FACTORY_GATEWAY_URL ?? "").trim();
  if (!raw) return null;
  try {
    const parsed = new URL(raw);
    const local = ["127.0.0.1", "localhost"].includes(parsed.hostname);
    const localHttp = parsed.protocol === "http:" && local;
    if (parsed.protocol !== "https:" && !localHttp) return null;
    return {
      baseUrl: parsed.toString().replace(/\/$/, ""),
      local,
    };
  } catch {
    return null;
  }
}

export const submitFactoryIntake = createServerFn({ method: "POST" })
  .validator((input: unknown) => requestSchema.parse(input))
  .handler(async ({ data }) => {
    if (!publicIntakeEnabled()) {
      return failure(
        "not_configured",
        "Factory intake is not enabled for public use.",
      );
    }

    const gateway = gatewayConfig();
    const intakeToken = (process.env.WARLOCK_FACTORY_INTAKE_TOKEN ?? "").trim();
    if (!gateway || !intakeToken) {
      return failure(
        "not_configured",
        "Factory intake is not connected in this environment.",
      );
    }

    const cfClientId = (process.env.WARLOCK_CF_ACCESS_CLIENT_ID ?? "").trim();
    const cfClientSecret = (
      process.env.WARLOCK_CF_ACCESS_CLIENT_SECRET ?? ""
    ).trim();
    if (!gateway.local && (!cfClientId || !cfClientSecret)) {
      return failure(
        "not_configured",
        "Factory gateway service authentication is not configured.",
      );
    }

    const headers: Record<string, string> = {
      "Content-Type": "application/json",
      "X-Warlock-Intake-Token": intakeToken,
    };
    if (cfClientId && cfClientSecret) {
      headers["CF-Access-Client-Id"] = cfClientId;
      headers["CF-Access-Client-Secret"] = cfClientSecret;
    }

    let response: Response;
    try {
      response = await fetch(`${gateway.baseUrl}/factory/intake`, {
        method: "POST",
        headers,
        body: JSON.stringify({
          prompt: data.prompt,
          client_request_id: data.clientRequestId,
          source: "website",
        }),
        signal: AbortSignal.timeout(8000),
      });
    } catch {
      return failure(
        "factory_unavailable",
        "Factory gateway is currently unavailable.",
      );
    }

    if (response.status === 401 || response.status === 403) {
      return failure(
        "gateway_auth_failed",
        "Factory gateway rejected the website connection.",
      );
    }
    if (response.status === 409) {
      return failure(
        "request_conflict",
        "This build request ID was already used for different content.",
      );
    }
    if (!response.ok) {
      return failure(
        "factory_unavailable",
        "Factory did not accept the build request.",
      );
    }

    try {
      const parsed = stagedSchema.parse(await response.json());
      return {
        ok: true as const,
        requestId: parsed.request_id,
        state: parsed.state,
        promptSha256: parsed.prompt_sha256,
        promptLength: parsed.prompt_length,
        idempotent: parsed.idempotent,
        truth: parsed.truth,
      };
    } catch {
      return failure(
        "invalid_factory_response",
        "Factory returned an unverified intake response.",
      );
    }
  });

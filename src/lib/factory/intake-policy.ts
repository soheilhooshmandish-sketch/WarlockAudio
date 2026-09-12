export type FactoryIntakeEnv = Record<string, string | undefined>;

export type FactoryIntakeConnection = {
  baseUrl: string;
  local: boolean;
  intakeToken: string;
  cfClientId: string;
  cfClientSecret: string;
};

export type FactoryIntakePolicyResult =
  | { ok: true; config: FactoryIntakeConnection }
  | {
      ok: false;
      reason:
        | "disabled"
        | "invalid_gateway"
        | "missing_intake_token"
        | "missing_cloudflare_access";
    };

const LOCAL_HOSTS = new Set(["127.0.0.1", "localhost", "::1"]);

function value(env: FactoryIntakeEnv, name: string): string {
  return (env[name] ?? "").trim();
}

export function resolveFactoryIntakePolicy(
  env: FactoryIntakeEnv,
): FactoryIntakePolicyResult {
  if (value(env, "WARLOCK_FACTORY_PUBLIC_INTAKE_ENABLED").toLowerCase() !== "true") {
    return { ok: false, reason: "disabled" };
  }

  const rawGateway = value(env, "WARLOCK_FACTORY_GATEWAY_URL");
  let parsed: URL;
  try {
    parsed = new URL(rawGateway);
  } catch {
    return { ok: false, reason: "invalid_gateway" };
  }

  if (parsed.username || parsed.password || parsed.search || parsed.hash) {
    return { ok: false, reason: "invalid_gateway" };
  }

  const local = LOCAL_HOSTS.has(parsed.hostname);
  const localHttp = local && parsed.protocol === "http:";
  if (parsed.protocol !== "https:" && !localHttp) {
    return { ok: false, reason: "invalid_gateway" };
  }

  const intakeToken = value(env, "WARLOCK_FACTORY_INTAKE_TOKEN");
  if (!intakeToken) {
    return { ok: false, reason: "missing_intake_token" };
  }

  const cfClientId = value(env, "WARLOCK_CF_ACCESS_CLIENT_ID");
  const cfClientSecret = value(env, "WARLOCK_CF_ACCESS_CLIENT_SECRET");
  if (!local && (!cfClientId || !cfClientSecret)) {
    return { ok: false, reason: "missing_cloudflare_access" };
  }

  return {
    ok: true,
    config: {
      baseUrl: parsed.toString().replace(/\/$/, ""),
      local,
      intakeToken,
      cfClientId,
      cfClientSecret,
    },
  };
}

import { useState } from "react";
import { createFileRoute, Link, Navigate } from "@tanstack/react-router";
import { ArrowLeft, LockKeyhole, ShieldCheck } from "lucide-react";
import { authEnabled, GROK_PROVIDERS, signIn } from "@/lib/auth/client";
import { useCurrentUserState } from "@/lib/auth/use-current-user";
import "../warlock-account.css";

export const Route = createFileRoute("/login")({ component: LoginPage });

function LoginPage() {
  const { user, isPending } = useCurrentUserState();
  const [workingProvider, setWorkingProvider] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);

  if (isPending) {
    return <main className="wa-login-page"><div className="wa-account-loading">CHECKING SESSION…</div></main>;
  }

  if (user) return <Navigate to="/account" />;

  if (!authEnabled) return <Navigate to="/account" />;

  async function start(providerId: string) {
    if (workingProvider) return;
    setError(null);
    setWorkingProvider(providerId);
    try {
      await signIn(providerId, { callbackURL: "/account", errorCallbackURL: "/login" });
    } catch {
      setError("Sign-in could not be started. No account change was made.");
      setWorkingProvider(null);
    }
  }

  return (
    <main className="wa-login-page">
      <section className="wa-login-card">
        <Link to="/" className="wa-login-back"><ArrowLeft size={15} /> WARLOCK HOME</Link>
        <div className="wa-login-mark" aria-hidden="true">W</div>
        <p>WARLOCK ACCOUNT</p>
        <h1>Sign in to your products, builds and licenses.</h1>
        <span>
          Authentication uses the existing WARLOCK app auth boundary. Factory secrets, payment keys and license signing material are never exposed to this page.
        </span>

        <div className="wa-login-providers">
          {GROK_PROVIDERS.map((provider) => (
            <button
              key={provider.providerId}
              type="button"
              disabled={Boolean(workingProvider)}
              onClick={() => void start(provider.providerId)}
            >
              {workingProvider === provider.providerId ? `CONNECTING ${provider.label.toUpperCase()}…` : `CONTINUE WITH ${provider.label.toUpperCase()}`}
            </button>
          ))}
        </div>

        {error ? <div className="wa-login-error" role="alert">{error}</div> : null}

        <div className="wa-login-proof">
          <ShieldCheck size={17} aria-hidden="true" />
          <span>Account session only. No purchase, build, activation or release is triggered by signing in.</span>
        </div>
        <div className="wa-login-proof">
          <LockKeyhole size={17} aria-hidden="true" />
          <span>Customer operations remain separately authorized and fail closed.</span>
        </div>
      </section>
    </main>
  );
}

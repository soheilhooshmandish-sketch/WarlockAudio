import { createFileRoute, Link } from "@tanstack/react-router";
import {
  Activity,
  ArrowLeft,
  CircleAlert,
  CircleHelp,
  LockKeyhole,
  ShieldCheck,
} from "lucide-react";
import { publicHealthSnapshot } from "@/lib/health/public-health";
import "../warlock-catalog.css";
import "../warlock-health.css";

export const Route = createFileRoute("/status")({ component: StatusPage });

function StateIcon({ state }: { state: "healthy" | "degraded" | "blocked" | "unknown" }) {
  if (state === "healthy") return <ShieldCheck size={20} aria-hidden="true" />;
  if (state === "degraded") return <CircleAlert size={20} aria-hidden="true" />;
  if (state === "blocked") return <LockKeyhole size={20} aria-hidden="true" />;
  return <CircleHelp size={20} aria-hidden="true" />;
}

function StatusPage() {
  return (
    <main className="wc-page wh-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand"><span className="wc-mark" aria-hidden="true">W</span><span>WARLOCK</span></Link>
        <nav aria-label="Status navigation">
          <Link to="/technology">TECHNOLOGY</Link>
          <Link to="/support">SUPPORT</Link>
          <Link to="/account">ACCOUNT</Link>
        </nav>
      </header>

      <section className="wc-hero wh-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> HOME</Link>
        <p>WARLOCK SYSTEM STATUS</p>
        <h1>No fake green lights.</h1>
        <span>
          This public V1 does not have live Factory telemetry. Unknown stays unknown. A build contract is not presented as uptime, and a private release gate is not presented as a public availability claim.
        </span>
        <div className="wh-overall" data-state={publicHealthSnapshot.overall}>
          <Activity size={18} aria-hidden="true" />
          <span>OVERALL: {publicHealthSnapshot.overall.toUpperCase()}</span>
          <em>{publicHealthSnapshot.liveTelemetryConnected ? "LIVE TELEMETRY" : "LIVE TELEMETRY NOT CONNECTED"}</em>
        </div>
      </section>

      <section className="wh-grid" aria-label="WARLOCK system components">
        {publicHealthSnapshot.components.map((component) => (
          <article key={component.id} data-state={component.state}>
            <div className="wh-card-head">
              <StateIcon state={component.state} />
              <span>{component.state.toUpperCase()}</span>
            </div>
            <p>{component.label.toUpperCase()}</p>
            <h2>{component.reason}</h2>
            <dl>
              <div><dt>SOURCE</dt><dd>{component.source.replaceAll("-", " ").toUpperCase()}</dd></div>
              <div><dt>EVIDENCE</dt><dd>{component.evidence ?? "NOT PUBLISHED"}</dd></div>
              <div><dt>NEXT</dt><dd>{component.nextAction ?? "NO ACTION PUBLISHED"}</dd></div>
              <div><dt>OBSERVED</dt><dd>{component.observedAt ?? "NO LIVE OBSERVATION"}</dd></div>
            </dl>
          </article>
        ))}
      </section>

      <section className="wh-policy">
        <div>
          <p>STATUS POLICY</p>
          <h2>Healthy requires evidence.</h2>
        </div>
        <div>
          <span>
            When live telemetry is connected later, it must be authenticated, redacted and bounded. The public website will never receive Factory secrets, raw private CI logs, customer data or license-signing material merely to draw a green status light.
          </span>
          <Link to="/technology">READ THE PROOF SYSTEM</Link>
        </div>
      </section>
    </main>
  );
}

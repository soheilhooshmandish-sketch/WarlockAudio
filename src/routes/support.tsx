import { createFileRoute, Link } from "@tanstack/react-router";
import {
  ArrowLeft,
  Download,
  KeyRound,
  LockKeyhole,
  PackageCheck,
  ShieldCheck,
  Wrench,
} from "lucide-react";
import "../warlock-catalog.css";

export const Route = createFileRoute("/support")({ component: SupportPage });

const supportAreas = [
  {
    title: "BUILD PROBLEM",
    copy: "For Generator or Factory jobs that stop at a validation gate. The public flow will show the failed layer instead of pretending success.",
    icon: Wrench,
    state: "DIAGNOSTICS PATH DESIGNED",
  },
  {
    title: "INSTALL / DOWNLOAD",
    copy: "Installer and download support will use exact product/version identity so the wrong binary is never treated as the right release.",
    icon: Download,
    state: "COMMERCIAL DELIVERY NOT LIVE",
  },
  {
    title: "ACTIVATION / LICENSE",
    copy: "The target policy is perpetual licensing on up to two machines. Public activation support stays locked until enforcement is proven end to end.",
    icon: KeyRound,
    state: "ACTIVATION SUPPORT NOT LIVE",
  },
  {
    title: "PRODUCT STATUS",
    copy: "WARLOCK product pages expose development or concept state honestly. A product is never marked available from artwork or a build alone.",
    icon: PackageCheck,
    state: "CATALOG STATUS PUBLISHED",
  },
];

function SupportPage() {
  return (
    <main className="wc-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand">
          <span className="wc-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <nav aria-label="Support navigation">
          <Link to="/products">PRODUCTS</Link>
          <Link to="/generate" search={{ prompt: "" }}>GENERATE</Link>
          <Link to="/status">STATUS</Link>
          <Link to="/pricing">PRICING</Link>
        </nav>
      </header>

      <section className="wc-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> COMPANY SITE</Link>
        <p>WARLOCK SUPPORT</p>
        <h1>Evidence before guesswork.</h1>
        <span>
          Support is being built around exact product, build and release identity. Customer support requests are not connected yet, so this page does not fake ticket creation or response times.
        </span>
      </section>

      <section className="wc-detail-grid" aria-label="Support areas">
        {supportAreas.map(({ title, copy, icon: Icon, state }) => (
          <article key={title}>
            <span className="wc-detail-icon"><Icon size={18} /></span>
            <p>{state}</p>
            <h2>{title}</h2>
            <span>{copy}</span>
          </article>
        ))}
      </section>

      <section className="wc-detail-policy">
        <div>
          <p>CUSTOMER CONTACT</p>
          <h2>Support request backend is not connected.</h2>
        </div>
        <div>
          <span>
            WARLOCK will not show a working-looking form that drops messages. Ticket creation, email delivery and human response commitments remain disabled until those services are connected and monitored.
          </span>
          <Link to="/status">OPEN SYSTEM STATUS</Link>
        </div>
      </section>

      <section className="wc-truth-strip">
        <LockKeyhole size={20} />
        <div>
          <strong>NO FAKE TICKETS</strong>
          <span>
            This V1 support surface provides navigation and status truth only. It does not send email, create a case, change a license, issue a refund or contact a customer.
          </span>
        </div>
      </section>

      <section className="wc-truth-strip">
        <ShieldCheck size={20} />
        <div>
          <strong>SUPPORT MODEL</strong>
          <span>
            Future diagnostics should attach the exact product ID, version, artifact identity and failed gate automatically, while sensitive values stay redacted.
          </span>
        </div>
      </section>
    </main>
  );
}

import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, BadgeCheck, LockKeyhole, ShieldCheck } from "lucide-react";
import "../warlock-catalog.css";

export const Route = createFileRoute("/pricing")({ component: PricingPage });

const pluginPolicy = [
  "Perpetual product license target",
  "Up to two activated machines",
  "Silent activation-compatible design target",
  "14-day evaluation target",
  "No plugin subscription requirement planned",
];

const factoryPolicy = [
  "Prompt review and preview do not claim a completed build",
  "Usage cost will be tied to real build work",
  "Failed release gates never become sellable releases",
  "Marketplace listing remains optional",
  "Final public pricing waits for compute and support cost proof",
];

function PricingPage() {
  return (
    <main className="wc-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand">
          <span className="wc-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <nav aria-label="Pricing navigation">
          <Link to="/products">PRODUCTS</Link>
          <Link to="/generate" search={{ prompt: "" }}>GENERATE</Link>
        </nav>
      </header>

      <section className="wc-hero wc-pricing-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> COMPANY SITE</Link>
        <p>PRICING POLICY</p>
        <h1>Price only what we can prove.</h1>
        <span>
          WARLOCK will not publish invented launch prices while build cost, support load,
          licensing operations and tax handling are still being validated.
        </span>
      </section>

      <section className="wc-pricing-grid">
        <article className="wc-price-card wc-price-primary">
          <div className="wc-price-card-head">
            <ShieldCheck size={22} />
            <div>
              <p>PLUGIN LICENSE</p>
              <h2>PERPETUAL</h2>
            </div>
          </div>
          <ul>
            {pluginPolicy.map((item) => (
              <li key={item}><BadgeCheck size={16} /> {item}</li>
            ))}
          </ul>
          <button type="button" disabled>LAUNCH PRICE NOT YET PUBLISHED</button>
        </article>

        <article className="wc-price-card">
          <div className="wc-price-card-head">
            <LockKeyhole size={22} />
            <div>
              <p>FACTORY BUILDS</p>
              <h2>USAGE BASED</h2>
            </div>
          </div>
          <ul>
            {factoryPolicy.map((item) => (
              <li key={item}><BadgeCheck size={16} /> {item}</li>
            ))}
          </ul>
          <button type="button" disabled>COST MODEL UNDER VALIDATION</button>
        </article>
      </section>

      <section className="wc-pricing-rule">
        <div>
          <p>SEPARATE THE TWO THINGS</p>
          <h2>Factory access is not plugin ownership.</h2>
        </div>
        <span>
          A future Generator plan may control how many products a customer can create. A legitimate perpetual plugin license must remain governed by its own license policy rather than disappearing because Generator access changes.
        </span>
      </section>

      <section className="wc-truth-strip">
        <LockKeyhole size={20} />
        <div>
          <strong>LIVE PAYMENT IS NOT ENABLED</strong>
          <span>Checkout, VAT handling, billing, refunds and live license issuance remain outside the public flow until sandbox evidence is complete.</span>
        </div>
      </section>
    </main>
  );
}

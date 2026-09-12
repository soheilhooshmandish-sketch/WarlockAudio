import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, Check, Crown, Gauge, Layers3, ShieldCheck, Sparkles, Zap } from "lucide-react";
import { SiteHeader } from "@/components/warlock/site-header";

export const Route = createFileRoute("/pricing")({ component: PricingPage });

const plans = [
  { name: "GENERATE", price: "€9.90", cadence: "per completed build", note: "For one-off creation without a subscription.", featured: false, features: ["Prompt Refiner", "AUTO DNA selection", "Standard skin", "Factory presets", "Production Health"] },
  { name: "GENERATE PRO", price: "€19.90", cadence: "per completed build", note: "For creators who want deeper sound and visual control.", featured: true, features: ["Full PRO menu", "Advanced Prompt Refiner", "Advanced DSP intent", "Skin variations", "Custom image/vector/logo slots"] },
  { name: "CREATOR", price: "€19.90", cadence: "per month", note: "PRO access with 3 completed builds each month.", featured: false, features: ["3 PRO builds / month", "PRO menu", "Preset tools", "Skin tools", "Priority queue"] },
  { name: "STUDIO", price: "€39.90", cadence: "per month", note: "For regular creation with 8 completed builds each month.", featured: false, features: ["8 PRO builds / month", "PRO menu", "Advanced skin workflow", "Preset pack tools", "Commercial workflow"] },
  { name: "WARLOCK PRO", price: "€69.90", cadence: "per month", note: "For high-volume professional creation with 16 completed builds.", featured: false, features: ["16 PRO builds / month", "Advanced audio controls", "Custom branding slots", "Priority builds", "Professional support"] },
];

function PricingPage() {
  return (
    <main className="warlock-app-page warlock-page-enter">
      <SiteHeader section="PRICING" active="/pricing" />
      <section className="warlock-simple-hero">
        <Link to="/" className="warlock-backlink"><ArrowLeft size={15} /> BACK TO WARLOCK</Link>
        <p className="warlock-app-kicker">SIMPLE. FAIR. POWERFUL.</p>
        <h1>BUILD ONCE.<br /><span>OR CREATE EVERY MONTH.</span></h1>
        <p>Pay only when Factory produces a completed build, or subscribe for a monthly build allowance and permanent access to the PRO creation menu.</p>
      </section>
      <section className="warlock-plan-grid">
        {plans.map((plan) => (
          <article key={plan.name} className={`warlock-plan-card ${plan.featured ? "is-featured" : ""}`}>
            {plan.featured ? <div className="warlock-plan-ribbon"><Crown size={14} /> BEST FOR CUSTOM WORK</div> : null}
            <p>{plan.name}</p><h2>{plan.price}</h2><small>{plan.cadence}</small><span>{plan.note}</span>
            <ul>{plan.features.map((feature) => <li key={feature}><Check size={15} />{feature}</li>)}</ul>
            <button className={`warlock-button ${plan.featured ? "warlock-button-primary" : "warlock-button-secondary"}`}>SELECT PLAN</button>
          </article>
        ))}
      </section>
      <section className="warlock-pricing-rules">
        <article><Sparkles size={20} /><div><strong>PREVIEW IS FREE</strong><span>Prompt refinement and non-build preview should not consume a paid build.</span></div></article>
        <article><Gauge size={20} /><div><strong>NO UNLIMITED PLAN</strong><span>Customer-facing plans use clear build allowances so compute remains controlled.</span></div></article>
        <article><Layers3 size={20} /><div><strong>PLUGIN OWNERSHIP IS SEPARATE</strong><span>Generator subscription does not become a subscription on the finished plugin.</span></div></article>
        <article><ShieldCheck size={20} /><div><strong>FACTORY HEALTH GATED</strong><span>A completed build must pass required Production Health gates before delivery.</span></div></article>
      </section>
      <section className="warlock-pricing-cta"><div><Zap size={24} /><h2>START WITH THE SOUND.</h2><p>You can choose one-off or monthly access after the product preview is ready.</p></div><Link to="/generate" className="warlock-button warlock-button-primary">OPEN GENERATOR</Link></section>
    </main>
  );
}

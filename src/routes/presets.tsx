import { createFileRoute, Link } from "@tanstack/react-router";
import {
  ArrowLeft,
  BadgeCheck,
  Boxes,
  FileKey,
  Gauge,
  LockKeyhole,
  PackageSearch,
} from "lucide-react";
import { presetPacks } from "@/lib/assets/catalog";
import "../warlock-catalog.css";
import "../warlock-assets.css";

export const Route = createFileRoute("/presets")({ component: PresetsPage });

const contract = [
  {
    title: "PRODUCT COMPATIBILITY",
    copy: "Every pack declares the compatible product IDs and the minimum product version it can safely load into.",
    icon: Boxes,
  },
  {
    title: "PARAMETER SCHEMA",
    copy: "Preset data is tied to a versioned parameter schema so product updates cannot silently reinterpret controls.",
    icon: Gauge,
  },
  {
    title: "CONTENT IDENTITY",
    copy: "A distributable pack carries its own version and SHA256 content identity before it can be treated as a real asset.",
    icon: FileKey,
  },
];

function PresetsPage() {
  return (
    <main className="wc-page wa-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand">
          <span className="wc-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <nav aria-label="Preset navigation">
          <Link to="/products">PRODUCTS</Link>
          <Link to="/skins">SKINS</Link>
          <Link to="/generate" search={{ prompt: "" }}>GENERATE</Link>
        </nav>
      </header>

      <section className="wc-hero wa-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> HOME</Link>
        <p>WARLOCK PRESET SYSTEM</p>
        <h1>Presets are versioned assets, not loose knob snapshots.</h1>
        <span>
          The public preset marketplace is not live yet. This route defines the contract that future factory, genre and artist packs must satisfy before distribution.
        </span>
      </section>

      <section className="wa-contract-grid" aria-label="Preset pack contract">
        {contract.map(({ title, copy, icon: Icon }) => (
          <article key={title}>
            <Icon size={22} aria-hidden="true" />
            <p>{title}</p>
            <span>{copy}</span>
          </article>
        ))}
      </section>

      <section className="wa-catalog-state">
        <div>
          <p>PUBLIC CATALOG</p>
          <h2>{presetPacks.length === 0 ? "No commercial preset packs published yet." : `${presetPacks.length} packs available.`}</h2>
          <span>
            WARLOCK will not populate this list with placeholder products. A pack appears here only after compatibility, schema version and exact content identity are known.
          </span>
        </div>
        <div className="wa-state-card" data-state="locked">
          <LockKeyhole size={21} aria-hidden="true" />
          <strong>COMMERCE LOCKED</strong>
          <span>Checkout, download entitlement and preset-pack licensing are not connected.</span>
        </div>
      </section>

      <section className="wa-ready-strip">
        <div><BadgeCheck size={17} /><span>LOAD / SAVE CONTRACT</span></div>
        <div><PackageSearch size={17} /><span>PACK METADATA</span></div>
        <div><FileKey size={17} /><span>CONTENT HASH</span></div>
      </section>
    </main>
  );
}

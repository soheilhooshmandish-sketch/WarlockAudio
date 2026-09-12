import { createFileRoute, Link } from "@tanstack/react-router";
import {
  ArrowLeft,
  BadgeCheck,
  ImageOff,
  Layers3,
  LockKeyhole,
  ShieldCheck,
  SwatchBook,
} from "lucide-react";
import { skinUploadPolicy, skins } from "@/lib/assets/catalog";
import { SiteHeader } from "@/components/warlock/site-header";
import "../warlock-catalog.css";
import "../warlock-assets.css";

export const Route = createFileRoute("/skins")({ component: SkinsPage });

const rules = [
  {
    title: "MASTER GEOMETRY LOCKED",
    copy: "A product skin may change material, color and surface treatment. It may not redraw, stretch or replace the canonical WARLOCK mark.",
    icon: ShieldCheck,
  },
  {
    title: "DSP STAYS SEPARATE",
    copy: "Skin identity is independent from DSP/state so visual packs cannot silently alter parameters, automation or saved projects.",
    icon: Layers3,
  },
  {
    title: "BOUNDED PERSONALIZATION",
    copy: "Future custom image, text and band-branding slots must stay inside approved regions and preserve control readability.",
    icon: SwatchBook,
  },
];

function SkinsPage() {
  return (
    <main className="wc-page wa-page">
      <SiteHeader active="/skins" />

      <section className="wc-hero wa-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> HOME</Link>
        <p>WARLOCK SKIN SYSTEM</p>
        <h1>One symbol. Many realms. No broken controls.</h1>
        <span>
          Skins are a separate visual asset layer. The commercial skin catalog and custom-upload pipeline remain locked until sanitization, compatibility and delivery are proven end to end.
        </span>
      </section>

      <section className="wa-contract-grid" aria-label="Skin system contract">
        {rules.map(({ title, copy, icon: Icon }) => (
          <article key={title}>
            <Icon size={22} aria-hidden="true" />
            <p>{title}</p>
            <span>{copy}</span>
          </article>
        ))}
      </section>

      <section className="wa-policy-grid">
        <article>
          <ImageOff size={22} aria-hidden="true" />
          <p>UPLOAD SAFETY</p>
          <h2>Fail closed until sanitization is explicit.</h2>
          <span>
            Raster uploads are intended to be bounded by type and size. SVG is currently rejected because scriptable/external-reference content is not accepted without a dedicated sanitizer.
          </span>
          <dl>
            <div><dt>MAX FILE</dt><dd>{Math.round(skinUploadPolicy.maxBytes / 1024 / 1024)} MB</dd></div>
            <div><dt>SVG</dt><dd>BLOCKED</dd></div>
            <div><dt>EXECUTABLE CONTENT</dt><dd>BLOCKED</dd></div>
          </dl>
        </article>
        <article>
          <LockKeyhole size={22} aria-hidden="true" />
          <p>PUBLIC CATALOG</p>
          <h2>{skins.length === 0 ? "No commercial skins published yet." : `${skins.length} skins available.`}</h2>
          <span>
            A skin appears here only after product compatibility, minimum product version, brand geometry version and exact content identity are known.
          </span>
        </article>
      </section>

      <section className="wa-ready-strip">
        <div><BadgeCheck size={17} /><span>BRAND GEOMETRY</span></div>
        <div><Layers3 size={17} /><span>DSP / SKIN SEPARATION</span></div>
        <div><ShieldCheck size={17} /><span>UPLOAD POLICY</span></div>
      </section>
    </main>
  );
}

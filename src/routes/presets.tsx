import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, ArrowRight, Check, Headphones, Layers3, Package, Sparkles } from "lucide-react";
import { SiteHeader } from "@/components/warlock/site-header";

export const Route = createFileRoute("/presets")({ component: PresetsPage });

const packs = [
  { name: "FUNERAL DOOM", count: 13, character: "Massive low-mid weight, slow decay and ritual space.", realm: "doom" },
  { name: "BLACK FROST", count: 13, character: "Cold attack, raw edge and frozen ambience.", realm: "frost" },
  { name: "THALL / MODERN", count: 13, character: "Tight transients, controlled low-end and aggressive articulation.", realm: "thall" },
  { name: "ABYSSAL SPACE", count: 13, character: "Long-form reverb, reverse textures and deep stereo movement.", realm: "abyss" },
  { name: "ALIEN SIGNAL", count: 13, character: "Biomechanical distortion, modulation and hostile texture layers.", realm: "alien" },
  { name: "FRACTURED RHYTHM", count: 13, character: "Glitch timing, repeat structures and broken-signal movement.", realm: "fracture" },
];

function PresetsPage() {
  return (
    <main className="warlock-app-page warlock-page-enter">
      <SiteHeader section="PRESETS" active="/presets" />

      <section className="warlock-simple-hero">
        <Link to="/" className="warlock-backlink"><ArrowLeft size={15} /> BACK TO WARLOCK</Link>
        <p className="warlock-app-kicker">PRESET ECOSYSTEM</p>
        <h1>SOUNDS WITH REAL CHARACTER.<br /><span>NOT JUST DIFFERENT KNOB POSITIONS.</span></h1>
        <p>WARLOCK preset packs are designed as distinct sound identities. Each preset must produce a meaningful musical change, not a cosmetic parameter shift.</p>
      </section>

      <section className="warlock-preset-grid">
        {packs.map((pack, index) => (
          <article className="warlock-preset-card" data-realm={pack.realm} key={pack.name}>
            <div className="warlock-preset-art">
              <img src="/brand/warlock-core.svg" alt="" />
              <span>{String(index + 1).padStart(2, "0")}</span>
            </div>
            <div className="warlock-preset-copy">
              <p>WARLOCK PRESET PACK</p>
              <h2>{pack.name}</h2>
              <small>{pack.count} DISTINCT SOUNDS</small>
              <span>{pack.character}</span>
              <ul>
                <li><Check size={14} /> Loadable preset format</li>
                <li><Check size={14} /> Version compatibility metadata</li>
                <li><Check size={14} /> Designed for real tonal variation</li>
              </ul>
              <div className="warlock-preset-actions">
                <button className="warlock-button warlock-button-secondary" disabled><Headphones size={15} /> AUDIO PREVIEW NEXT</button>
                <button className="warlock-button warlock-button-quiet" disabled>STORE NEXT</button>
              </div>
            </div>
          </article>
        ))}
      </section>

      <section className="warlock-preset-system">
        <article><Sparkles size={20} /><div><strong>13 AS A FACTORY STANDARD</strong><span>Effect-pedal signature products may use the established 13-preset factory-bank rule where it fits the product.</span></div></article>
        <article><Layers3 size={20} /><div><strong>PACKS ARE VERSIONED</strong><span>Preset compatibility, product version and required features stay explicit.</span></div></article>
        <article><Package size={20} /><div><strong>BUNDLES COME LATER</strong><span>Preset packs can eventually bundle with compatible skins without coupling the DSP to the visual layer.</span></div></article>
        <Link to="/generate" className="warlock-button warlock-button-primary">CREATE A SOUND <ArrowRight size={16} /></Link>
      </section>
    </main>
  );
}

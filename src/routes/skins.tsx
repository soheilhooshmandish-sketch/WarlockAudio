import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, ArrowRight, Check, Image as ImageIcon, LockKeyhole, Palette, ShieldCheck, Sparkles, Type } from "lucide-react";

export const Route = createFileRoute("/skins")({ component: SkinsPage });

const skins = [
  { name: "BLACK FROST", realm: "frost", material: "FROZEN STEEL / BLACK GLASS", note: "Ice material, restrained white-blue emission and cold reflective edges." },
  { name: "ALIEN", realm: "alien", material: "BIOMECHANICAL METAL", note: "Organic machine structure, dark metal cavities and hostile internal light." },
  { name: "ABYSS", realm: "abyss", material: "DEEP GLASS / OCEANIC STEEL", note: "Smoked glass, submerged depth and controlled cold-blue atmosphere." },
  { name: "THALL", realm: "thall", material: "FRACTURED INDUSTRIAL STEEL", note: "Heavy machined surfaces, impact geometry and minimal aggressive light." },
  { name: "VOID", realm: "void", material: "COSMIC BLACK / OPTICAL GLASS", note: "Black spatial surfaces, restrained edge light and dimensional depth." },
  { name: "CUSTOM PRO", realm: "custom", material: "SAFE USER BRANDING SLOTS", note: "Approved image, vector, logo, text and compatible material customization." },
];

function SkinsPage() {
  return (
    <main className="warlock-app-page warlock-page-enter">
      <header className="warlock-app-header">
        <Link to="/" className="warlock-app-brand"><img src="/brand/warlock-core.svg" alt="" /><span><strong>WARLOCK</strong><small>SKINS</small></span></Link>
        <nav><Link to="/">Home</Link><Link to="/products">Products</Link><Link to="/generate">Generate</Link><Link to="/presets">Presets</Link><Link to="/skins" className="is-active">Skins</Link><Link to="/pricing">Pricing</Link><Link to="/support">Support</Link></nav>
        <Link to="/generate" className="warlock-button warlock-button-primary">CREATE</Link>
      </header>

      <section className="warlock-simple-hero">
        <Link to="/" className="warlock-backlink"><ArrowLeft size={15} /> BACK TO WARLOCK</Link>
        <p className="warlock-app-kicker">VISUAL DNA LIBRARY</p>
        <h1>ONE MASTER IDENTITY.<br /><span>MANY PRODUCT REALMS.</span></h1>
        <p>The WARLOCK emblem never changes geometry. Skins change approved material, environment, surface treatment and bounded user-branding slots without damaging usability.</p>
      </section>

      <section className="warlock-skin-library">
        {skins.map((skin, index) => (
          <article className="warlock-skin-card" data-realm={skin.realm} key={skin.name}>
            <div className="warlock-skin-visual">
              <span>{String(index + 1).padStart(2, "0")}</span>
              <img src="/brand/warlock-emblem.svg" alt="" />
              <div className="warlock-skin-material" />
            </div>
            <div className="warlock-skin-copy">
              <p>WARLOCK SKIN DNA</p>
              <h2>{skin.name}</h2>
              <small>{skin.material}</small>
              <span>{skin.note}</span>
              <div className="warlock-skin-rules">
                <div><Check size={14} /> Fixed master logo geometry</div>
                <div><Check size={14} /> Protected control zones</div>
                <div><Check size={14} /> Responsive-safe layout</div>
              </div>
              <button className="warlock-button warlock-button-secondary" disabled>APPLY FLOW NEXT</button>
            </div>
          </article>
        ))}
      </section>

      <section className="warlock-custom-skin">
        <div className="warlock-custom-skin-copy">
          <p className="warlock-app-kicker">CUSTOM — PRO</p>
          <h2>Personalize without breaking the instrument.</h2>
          <span>Customer media is only placed into bounded safe slots. Uploaded artwork never gets direct executable access to Factory builds.</span>
        </div>
        <div className="warlock-custom-tools">
          <article><ImageIcon size={20} /><div><strong>IMAGE / VECTOR</strong><span>Upload artwork into approved display regions.</span></div></article>
          <article><Type size={20} /><div><strong>TEXT</strong><span>Add band/product text inside protected typography zones.</span></div></article>
          <article><Palette size={20} /><div><strong>COLOR / MATERIAL</strong><span>Choose compatible material and emission treatments.</span></div></article>
          <article><LockKeyhole size={20} /><div><strong>SAFE SLOTS</strong><span>Identity, interaction and accessibility remain protected.</span></div></article>
        </div>
      </section>

      <section className="warlock-skin-proof">
        <article><ShieldCheck size={20} /><div><strong>NO GENERATED SUBSTITUTE LOGOS</strong><span>If the canonical asset fails, WARLOCK falls back to text-only identity rather than inventing a new mark.</span></div></article>
        <article><Sparkles size={20} /><div><strong>2–3 STRONG VARIANTS</strong><span>Factory should present a small curated set, not dozens of random low-quality outputs.</span></div></article>
        <Link to="/generate" className="warlock-button warlock-button-primary">OPEN GENERATOR <ArrowRight size={16} /></Link>
      </section>
    </main>
  );
}

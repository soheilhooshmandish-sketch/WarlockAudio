import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, ArrowRight, CircleDot, Headphones, ShieldCheck } from "lucide-react";

export const Route = createFileRoute("/products")({ component: ProductsPage });

const products = [
  { name: "VOID", type: "SPATIAL TEXTURE", status: "ACTIVE DEVELOPMENT", realm: "void", note: "Pitch, grain, reverse and space in one evolving system." },
  { name: "THALL", type: "MODERN METAL", status: "PROTOTYPE", realm: "thall", note: "Tight transient control for modern heavy rhythm." },
  { name: "ABYSS", type: "AMBIENT / REVERB / DELAY", status: "PROTOTYPE", realm: "abyss", note: "Long-form space, depth and frozen atmosphere." },
  { name: "CHIMERA", type: "OCTAVE / HARMONY", status: "PROTOTYPE", realm: "chimera", note: "Hybrid voices, interval movement and layered width." },
  { name: "ALIEN", type: "DISTORTION / MULTI-FX", status: "PROTOTYPE", realm: "alien", note: "Hostile textures built around a biomechanical core." },
  { name: "BLACK FROST", type: "BLACK METAL DISTORTION", status: "IN DEVELOPMENT", realm: "frost", note: "Cold attack. Raw edge. Controlled low end." },
  { name: "FRACTURE", type: "GLITCH / RHYTHM", status: "CONCEPT / PROTOTYPE", realm: "fracture", note: "Broken rhythm, repeat structures and controlled instability." },
  { name: "DISTO / SYNTH", type: "DISTORTION / SYNTH", status: "CONCEPT / PROTOTYPE", realm: "synth", note: "Distortion and synthesis inside a shared performance system." },
];

function ProductsPage() {
  return (
    <main className="warlock-app-page warlock-page-enter">
      <header className="warlock-app-header">
        <Link to="/" className="warlock-app-brand"><img src="/brand/warlock-core.svg" alt="" /><span><strong>WARLOCK</strong><small>PRODUCTS</small></span></Link>
        <nav><Link to="/">Home</Link><Link to="/products" className="is-active">Products</Link><Link to="/generate">Generate</Link><Link to="/pricing">Pricing</Link><Link to="/support">Support</Link></nav>
        <Link to="/generate" className="warlock-button warlock-button-primary">CREATE</Link>
      </header>

      <section className="warlock-simple-hero">
        <Link to="/" className="warlock-backlink"><ArrowLeft size={15} /> BACK TO WARLOCK</Link>
        <p className="warlock-app-kicker">SIGNATURE COLLECTION</p>
        <h1>BUILT BY WARLOCK.<br /><span>EACH PRODUCT IS ITS OWN REALM.</span></h1>
        <p>Signature products use WARLOCK’s fixed master identity, while sound, material, movement and atmosphere are specific to each product world.</p>
      </section>

      <section className="warlock-catalog-grid">
        {products.map((product, index) => (
          <article key={product.name} className="warlock-catalog-card" data-realm={product.realm}>
            <div className="warlock-catalog-art">
              <span className="warlock-catalog-index">{String(index + 1).padStart(2, "0")}</span>
              <img src="/brand/warlock-emblem.svg" alt="" />
              <div className="warlock-catalog-energy" />
            </div>
            <div className="warlock-catalog-copy">
              <p><CircleDot size={11} /> {product.status}</p>
              <h2>{product.name}</h2>
              <small>{product.type}</small>
              <span>{product.note}</span>
              <div className="warlock-catalog-actions">
                <button className="warlock-button warlock-button-secondary" disabled>DETAIL PAGE NEXT</button>
                <button className="warlock-catalog-listen" disabled><Headphones size={16} /> AUDIO SOON</button>
              </div>
            </div>
          </article>
        ))}
      </section>

      <section className="warlock-catalog-proof">
        <article><ShieldCheck size={20} /><div><strong>STATUS IS EVIDENCE-BASED</strong><span>Artwork or HTML alone never marks a product as commercially available.</span></div></article>
        <article><img src="/brand/warlock-core.svg" alt="" /><div><strong>MASTER GEOMETRY IS FIXED</strong><span>Product identity changes through approved realm material, never a redrawn WARLOCK emblem.</span></div></article>
        <Link to="/generate" className="warlock-button warlock-button-primary">CREATE YOUR OWN <ArrowRight size={16} /></Link>
      </section>
    </main>
  );
}

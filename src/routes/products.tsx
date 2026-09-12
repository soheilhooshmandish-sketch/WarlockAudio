import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, ArrowRight, CircleDot, Headphones, ShieldCheck } from "lucide-react";
import { SiteHeader } from "@/components/warlock/site-header";
import { products } from "@/lib/catalog/products";

export const Route = createFileRoute("/products")({ component: ProductsPage });

function ProductsPage() {
  return (
    <main className="warlock-app-page warlock-page-enter">
      <SiteHeader section="PRODUCTS" active="/products" />

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
              <p><CircleDot size={11} /> {product.statusLabel}</p>
              <h2>{product.name}</h2>
              <small>{product.category}</small>
              <span>{product.character}</span>
              <div className="warlock-catalog-actions">
                <Link to="/products/$slug" params={{ slug: product.slug }} className="warlock-button warlock-button-secondary">VIEW PRODUCT</Link>
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

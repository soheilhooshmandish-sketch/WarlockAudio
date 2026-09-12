import { createFileRoute, Link } from "@tanstack/react-router";
import {
  ArrowLeft,
  BadgeCheck,
  CircleAlert,
  LockKeyhole,
  ShieldCheck,
} from "lucide-react";
import { productBySlug } from "@/lib/catalog/products";
import "../warlock-catalog.css";

export const Route = createFileRoute("/products/$slug")({ component: ProductPage });

function ProductPage() {
  const { slug } = Route.useParams();
  const product = productBySlug(slug);

  if (!product) {
    return (
      <main className="wc-page wc-detail-page">
        <header className="wc-topbar">
          <Link to="/" className="wc-brand"><span className="wc-mark">W</span><span>WARLOCK</span></Link>
        </header>
        <section className="wc-empty-state">
          <LockKeyhole size={26} />
          <h1>Product not found.</h1>
          <Link to="/products">RETURN TO PRODUCTS</Link>
        </section>
      </main>
    );
  }

  return (
    <main className="wc-page wc-detail-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand">
          <span className="wc-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <nav aria-label="Product navigation">
          <Link to="/products">PRODUCTS</Link>
          <Link to="/generate" search={{ prompt: `Create a product inspired by ${product.name}, but with my own sound.` }}>GENERATE</Link>
        </nav>
      </header>

      <section className="wc-detail-hero">
        <div>
          <Link to="/products" className="wc-back"><ArrowLeft size={15} /> ALL PRODUCTS</Link>
          <p>{product.category}</p>
          <h1>{product.name}</h1>
          <span>{product.summary}</span>
          <div className="wc-status wc-status-large" data-status={product.status}>
            {product.status === "in-development" ? <ShieldCheck size={16} /> : <LockKeyhole size={16} />}
            {product.statusLabel}
          </div>
        </div>
        <div className={`wc-detail-art wc-art-${product.slug}`} aria-hidden="true">
          <span>{product.name}</span>
        </div>
      </section>

      <section className="wc-detail-grid">
        <article>
          <span className="wc-detail-icon"><BadgeCheck size={18} /></span>
          <p>WHAT IS REAL</p>
          <h2>Evidence before marketing.</h2>
          <span>{product.evidenceNote}</span>
        </article>
        <article>
          <span className="wc-detail-icon"><CircleAlert size={18} /></span>
          <p>RELEASE BLOCKER</p>
          <h2>Why it is not marked available.</h2>
          <span>{product.releaseBlocker}</span>
        </article>
      </section>

      <section className="wc-detail-policy">
        <div>
          <p>COMMERCIAL STATUS</p>
          <h2>{product.status === "in-development" ? "In development — not for sale." : "Concept DNA — not a release candidate."}</h2>
        </div>
        <div>
          <span>
            Product pages will gain audio demos, controls, system requirements, license and Buy/Try actions only when those surfaces are backed by real product evidence.
          </span>
          <Link to="/generate" search={{ prompt: "" }}>OPEN GENERATOR</Link>
        </div>
      </section>
    </main>
  );
}

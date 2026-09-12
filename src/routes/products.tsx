import { createFileRoute, Link } from "@tanstack/react-router";
import { ArrowLeft, ArrowRight, LockKeyhole, ShieldCheck } from "lucide-react";
import { products } from "@/lib/catalog/products";
import "../warlock-catalog.css";

export const Route = createFileRoute("/products")({ component: ProductsPage });

function ProductsPage() {
  return (
    <main className="wc-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand">
          <span className="wc-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <nav aria-label="Products navigation">
          <Link to="/generate" search={{ prompt: "" }}>GENERATE</Link>
          <Link to="/pricing">PRICING</Link>
        </nav>
      </header>

      <section className="wc-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> COMPANY SITE</Link>
        <p>WARLOCK SIGNATURE</p>
        <h1>Products with a real status.</h1>
        <span>
          A concept is not a release. A working build is not automatically commercial.
          WARLOCK only advances product status when the required evidence exists.
        </span>
      </section>

      <section className="wc-product-grid" aria-label="WARLOCK product catalog">
        {products.map((product) => (
          <article className="wc-product-card" key={product.slug}>
            <div className={`wc-product-art wc-art-${product.slug}`} aria-hidden="true">
              <span>{product.name}</span>
            </div>
            <div className="wc-product-meta">
              <div className="wc-status" data-status={product.status}>
                {product.status === "in-development" ? <ShieldCheck size={14} /> : <LockKeyhole size={14} />}
                {product.statusLabel}
              </div>
              <p>{product.category}</p>
              <h2>{product.name}</h2>
              <span>{product.character}</span>
              <Link to="/products/$slug" params={{ slug: product.slug }}>
                EXPLORE <ArrowRight size={15} />
              </Link>
            </div>
          </article>
        ))}
      </section>

      <section className="wc-truth-strip">
        <ShieldCheck size={20} />
        <div>
          <strong>STATUS POLICY</strong>
          <span>
            AVAILABLE will remain unused until the exact product artifact clears its required build,
            host, human review, installer, license and release evidence.
          </span>
        </div>
      </section>
    </main>
  );
}

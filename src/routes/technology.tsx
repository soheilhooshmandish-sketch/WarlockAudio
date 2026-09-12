import { createFileRoute, Link } from "@tanstack/react-router";
import {
  ArrowLeft,
  BadgeCheck,
  Binary,
  Boxes,
  Fingerprint,
  HardDriveDownload,
  LockKeyhole,
  ShieldCheck,
  UserCheck,
} from "lucide-react";
import "../warlock-catalog.css";

export const Route = createFileRoute("/technology")({ component: TechnologyPage });

const proofLayers = [
  {
    title: "EXACT BINARY IDENTITY",
    copy: "The production chain binds build evidence to a deterministic VST3 bundle-tree SHA256. Plugin validation must point to that same binary identity rather than only matching a version number.",
    icon: Fingerprint,
    label: "BUILD → HASH → PLUGINVAL",
  },
  {
    title: "REAL HOST + HUMAN GATES",
    copy: "Host/DAW evidence, Founder Guitar Test and Engineering QC remain explicit gates. CI contract tests cannot manufacture those manual approvals for a product.",
    icon: UserCheck,
    label: "HOST / FOUNDER / ENGINEERING",
  },
  {
    title: "INSTALLER ROUNDTRIP",
    copy: "Installer preparation is tied to the exact validated VST3. The release path verifies installation, installed-file integrity and clean uninstall before treating installer evidence as valid.",
    icon: HardDriveDownload,
    label: "INSTALL → VERIFY → UNINSTALL",
  },
  {
    title: "FULFILLMENT REHASH",
    copy: "The buyer-fulfillment boundary distrusts high-level release flags alone. It recomputes the VST3 tree hash and installer hash from the release archive before a buyer package can be prepared.",
    icon: Boxes,
    label: "RELEASE ZIP → REHASH → BUYER PACK",
  },
];

function TechnologyPage() {
  return (
    <main className="wc-page">
      <header className="wc-topbar">
        <Link to="/" className="wc-brand">
          <span className="wc-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <nav aria-label="Technology navigation">
          <Link to="/products">PRODUCTS</Link>
          <Link to="/generate" search={{ prompt: "" }}>GENERATE</Link>
          <Link to="/pricing">PRICING</Link>
          <Link to="/support">SUPPORT</Link>
        </nav>
      </header>

      <section className="wc-hero">
        <Link to="/" className="wc-back"><ArrowLeft size={15} /> COMPANY SITE</Link>
        <p>WARLOCK TECHNOLOGY</p>
        <h1>Proof is part of the product.</h1>
        <span>
          WARLOCK is being built so a green label is not enough. The release path carries evidence about the exact binary from build through validation, installer and fulfillment.
        </span>
      </section>

      <section className="wc-detail-grid" aria-label="WARLOCK production proof layers">
        {proofLayers.map(({ title, copy, icon: Icon, label }) => (
          <article key={title}>
            <span className="wc-detail-icon"><Icon size={18} /></span>
            <p>{label}</p>
            <h2>{title}</h2>
            <span>{copy}</span>
          </article>
        ))}
      </section>

      <section className="wc-detail-policy">
        <div>
          <p>RELEASE GUARDIAN</p>
          <h2>Missing or stale evidence keeps release locked.</h2>
        </div>
        <div>
          <span>
            FARNAZ Release Guardian classifies release blockers and keeps commercial distribution fail-closed. Safe machine work may be diagnosed automatically, while real host tests, Founder review, Engineering QC and repository administration remain human/admin boundaries.
          </span>
          <Link to="/products">SEE PRODUCT STATUS</Link>
        </div>
      </section>

      <section className="wc-truth-strip">
        <Binary size={20} />
        <div>
          <strong>ONE BINARY, ONE EVIDENCE CHAIN</strong>
          <span>
            Changing VST3 bytes without changing the semantic version invalidates binary-bound evidence instead of silently inheriting approval from an older artifact.
          </span>
        </div>
      </section>

      <section className="wc-truth-strip">
        <LockKeyhole size={20} />
        <div>
          <strong>THIS DOES NOT MEAN EVERY PRODUCT IS READY</strong>
          <span>
            The production system can enforce gates while an individual product is still concept, development or blocked. Commercial status still depends on that product producing its own required evidence.
          </span>
        </div>
      </section>
    </main>
  );
}

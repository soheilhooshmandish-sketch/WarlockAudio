import { FormEvent, useState } from "react";
import { createFileRoute } from "@tanstack/react-router";
import {
  ArrowDown,
  ArrowRight,
  Box,
  Cpu,
  Package,
  ShieldCheck,
  Sparkles,
} from "lucide-react";
import { PedalEnclosure } from "@/components/pedal/enclosure";
import { StudioDock } from "@/components/studio/studio";
import "../warlock-home.css";

export const Route = createFileRoute("/")({ component: Home });

const promptExamples = [
  "Modern high-gain distortion",
  "Lo-fi tape saturation",
  "Ambient black-metal space",
];

const products = [
  {
    eyebrow: "DISTORTION SYSTEM",
    name: "VOID",
    copy: "A focused heavy-guitar platform built around exact control, mix-ready output and a release chain that can prove what shipped.",
    icon: Cpu,
  },
  {
    eyebrow: "AMP + CAB SYSTEM",
    name: "MONOLITH",
    copy: "Amp, cabinet and IR architecture designed as one coherent rig instead of a loose pile of processors.",
    icon: Box,
  },
  {
    eyebrow: "FULL RIG",
    name: "PORTAL",
    copy: "A single product surface for pedal, amp, cab and routing — built to become a complete playable signal path.",
    icon: Package,
  },
];

function Home() {
  const [prompt, setPrompt] = useState("");
  const [stagedPrompt, setStagedPrompt] = useState("");

  function submitPrompt(event: FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const value = prompt.trim();
    if (!value) return;
    setStagedPrompt(value);
  }

  return (
    <main className="warlock-site">
      <header className="warlock-nav-wrap">
        <div className="warlock-nav">
          <a className="warlock-brand" href="#top" aria-label="Warlock Plugins home">
            <span className="warlock-brand-mark" aria-hidden="true">W</span>
            <span className="warlock-brand-name">WARLOCK</span>
          </a>

          <nav className="warlock-nav-links" aria-label="Primary navigation">
            <a href="#products">PRODUCTS</a>
            <a href="#marketplace">MARKETPLACE</a>
            <a href="#company">COMPANY</a>
          </nav>

          <button
            className="warlock-sign-in"
            type="button"
            title="Accounts are not enabled on this build yet"
            aria-label="Sign in — account system coming soon"
          >
            SIGN IN
          </button>
        </div>
      </header>

      <section className="warlock-hero" id="top">
        <div className="warlock-hero-grid" aria-hidden="true" />
        <div className="warlock-hero-content">
          <p className="warlock-kicker">WARLOCK PLUGINS · PROFESSIONAL AUDIO SOFTWARE</p>
          <h1>
            Describe your sound.
            <br />
            <span>Warlock builds it.</span>
          </h1>
          <p className="warlock-hero-copy">
            Turn an idea into a production-ready audio tool through a proof-driven
            factory built for VST3 workflows.
          </p>

          <form className="warlock-prompt" onSubmit={submitPrompt}>
            <label htmlFor="warlock-prompt" className="warlock-sr-only">
              Describe the audio plugin you want to build
            </label>
            <textarea
              id="warlock-prompt"
              value={prompt}
              onChange={(event) => setPrompt(event.target.value)}
              placeholder="e.g. A brutal high-gain distortion for modern metal with a tight low end, controllable saturation and mix-ready output."
              rows={3}
            />
            <button type="submit" className="warlock-generate" disabled={!prompt.trim()}>
              <Sparkles size={18} strokeWidth={1.8} aria-hidden="true" />
              GENERATE
            </button>
          </form>

          <div className="warlock-examples" aria-label="Prompt examples">
            {promptExamples.map((example) => (
              <button key={example} type="button" onClick={() => setPrompt(example)}>
                {example}
              </button>
            ))}
          </div>

          {stagedPrompt ? (
            <div className="warlock-stage-note" role="status">
              <ShieldCheck size={17} strokeWidth={1.8} aria-hidden="true" />
              <span>
                Prompt staged locally. No paid build or public release has been started.
              </span>
            </div>
          ) : null}
        </div>

        <a className="warlock-scroll" href="#products">
          <span>SCROLL TO DISCOVER</span>
          <ArrowDown size={17} strokeWidth={1.6} aria-hidden="true" />
        </a>
      </section>

      <section className="warlock-section" id="products">
        <div className="warlock-section-heading">
          <p>WARLOCK SYSTEMS</p>
          <h2>Built as instruments. Proven as software.</h2>
          <span>
            Each product is designed around a real signal path, deterministic build
            evidence and a release chain that fails closed instead of guessing.
          </span>
        </div>

        <div className="warlock-product-grid">
          {products.map(({ eyebrow, name, copy, icon: Icon }) => (
            <article className="warlock-product-card" key={name}>
              <div className="warlock-product-icon">
                <Icon size={24} strokeWidth={1.45} aria-hidden="true" />
              </div>
              <p>{eyebrow}</p>
              <h3>{name}</h3>
              <span>{copy}</span>
              <button type="button" aria-label={`${name} details coming soon`}>
                EXPLORE
                <ArrowRight size={16} strokeWidth={1.6} aria-hidden="true" />
              </button>
            </article>
          ))}
        </div>
      </section>

      <section className="warlock-lab-section" aria-labelledby="lab-title">
        <div className="warlock-section-heading warlock-section-heading-left">
          <p>LIVE LAB PROTOTYPE</p>
          <h2 id="lab-title">The old demo stays available while the company layer evolves.</h2>
          <span>
            This interactive pedal surface remains a lab prototype. It is not a claim
            that a commercial release is ready.
          </span>
        </div>
        <div className="warlock-lab-frame">
          <PedalEnclosure />
          <StudioDock />
        </div>
      </section>

      <section className="warlock-section warlock-marketplace" id="marketplace">
        <div className="warlock-section-heading">
          <p>MARKETPLACE</p>
          <h2>A future home for plugins, presets and IRs.</h2>
          <span>
            Marketplace listings will only unlock after creator ownership, package
            integrity, licensing and buyer-delivery rules are proven end to end.
          </span>
        </div>
        <div className="warlock-market-strip" aria-label="Marketplace roadmap">
          <span>PLUGINS</span>
          <span>PRESET BANKS</span>
          <span>IR LIBRARIES</span>
          <span>CREATOR RELEASES</span>
        </div>
      </section>

      <section className="warlock-company" id="company">
        <div>
          <p>THE WARLOCK STANDARD</p>
          <h2>No fake green lights.</h2>
        </div>
        <div className="warlock-company-copy">
          <p>
            WARLOCK is being built around one rule: the exact binary delivered to a
            customer must be the binary that passed its evidence chain.
          </p>
          <p>
            Build, plugin validation, host testing, founder review, engineering QC,
            installer integrity and fulfillment are treated as one connected system.
          </p>
        </div>
      </section>

      <footer className="warlock-footer">
        <div className="warlock-brand warlock-brand-footer">
          <span className="warlock-brand-mark" aria-hidden="true">W</span>
          <span className="warlock-brand-name">WARLOCK PLUGINS</span>
        </div>
        <p>Professional audio software · VST3 production pipeline · 2026</p>
        <p>Commercial release remains gated until every required proof is present.</p>
      </footer>
    </main>
  );
}

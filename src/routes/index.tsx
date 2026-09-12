import { FormEvent, useState } from "react";
import { createFileRoute } from "@tanstack/react-router";
import {
  Activity,
  ArrowDown,
  ArrowRight,
  BadgeCheck,
  Box,
  Cpu,
  Download,
  Headphones,
  LifeBuoy,
  LockKeyhole,
  Package,
  ShieldCheck,
  Sparkles,
  Terminal,
  Wrench,
} from "lucide-react";
import { PedalEnclosure } from "@/components/pedal/enclosure";
import { StudioDock } from "@/components/studio/studio";
import "../warlock-home.css";
import "../warlock-sections.css";

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

const factorySteps = [
  {
    number: "01",
    title: "Describe",
    copy: "Define the sound, use case and controls in plain language.",
    icon: Terminal,
  },
  {
    number: "02",
    title: "Build",
    copy: "WARLOCK turns the ProductSpec into a deterministic VST3 candidate.",
    icon: Wrench,
  },
  {
    number: "03",
    title: "Prove",
    copy: "Validation, exact-binary evidence and installer checks must agree.",
    icon: BadgeCheck,
  },
  {
    number: "04",
    title: "Deliver",
    copy: "Only a release that clears the required gates can reach fulfillment.",
    icon: Download,
  },
];

const waveform = [
  18, 31, 22, 47, 69, 39, 78, 54, 34, 61, 86, 72, 42, 58, 92, 63, 38, 74,
  51, 83, 65, 33, 57, 76, 44, 68, 88, 55, 35, 71, 49, 79, 60, 27, 48, 66,
  37, 59, 73, 46,
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
            <a href="#factory">HOW IT WORKS</a>
            <a href="#pricing">PRICING</a>
            <a href="#support">SUPPORT</a>
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

      <section className="warlock-factory-section" id="factory">
        <div className="warlock-section-heading">
          <p>THE FACTORY</p>
          <h2>Build smarter. Ship only what is proven.</h2>
          <span>
            Automation handles repeatable engineering work. Human gates stay human.
            Release remains locked whenever evidence is missing or stale.
          </span>
        </div>

        <div className="warlock-factory-flow">
          {factorySteps.map(({ number, title, copy, icon: Icon }) => (
            <article className="warlock-factory-step" key={number}>
              <div className="warlock-factory-step-top">
                <span>{number}</span>
                <Icon size={22} strokeWidth={1.45} aria-hidden="true" />
              </div>
              <h3>{title}</h3>
              <p>{copy}</p>
            </article>
          ))}
        </div>

        <div className="warlock-proof-strip">
          <div><Activity size={18} aria-hidden="true" /><span>BUILD</span></div>
          <div><BadgeCheck size={18} aria-hidden="true" /><span>PLUGINVAL</span></div>
          <div><ShieldCheck size={18} aria-hidden="true" /><span>HOST / QC</span></div>
          <div><Package size={18} aria-hidden="true" /><span>INSTALLER</span></div>
          <div><LockKeyhole size={18} aria-hidden="true" /><span>RELEASE GUARD</span></div>
        </div>
      </section>

      <section className="warlock-preview-section" aria-labelledby="preview-title">
        <div className="warlock-preview-copy">
          <p>TEST BEFORE DELIVERY</p>
          <h2 id="preview-title">Hear it before you download.</h2>
          <span>
            The final Factory will render a real test riff through the exact candidate
            binary. Until that render exists, WARLOCK does not fabricate an audio preview.
          </span>
          <div className="warlock-preview-state">
            <Headphones size={18} aria-hidden="true" />
            <span>{stagedPrompt ? "PROMPT READY · WAITING FOR FACTORY RENDER" : "NO VERIFIED RENDER YET"}</span>
          </div>
        </div>

        <div className="warlock-wave-panel" aria-label="Audio preview placeholder">
          <div className="warlock-wave-header">
            <span>VERIFIED AUDIO PREVIEW</span>
            <span>LOCKED</span>
          </div>
          <div className="warlock-waveform" aria-hidden="true">
            {waveform.map((height, index) => (
              <i key={`${height}-${index}`} style={{ height: `${height}%` }} />
            ))}
          </div>
          <div className="warlock-wave-footer">
            <span>DRY / WET comparison becomes available after a real render passes.</span>
            <LockKeyhole size={17} aria-hidden="true" />
          </div>
        </div>
      </section>

      <section className="warlock-pricing-section" id="pricing">
        <div className="warlock-section-heading">
          <p>PRICING POLICY</p>
          <h2>Pay for products, not recurring access.</h2>
          <span>
            WARLOCK is being designed around perpetual product licenses. Final build
            pricing and credit economics will be published only after compute and support
            costs are proven.
          </span>
        </div>

        <div className="warlock-pricing-grid">
          <article className="warlock-pricing-card warlock-pricing-card-primary">
            <p>PLUGIN LICENSE</p>
            <h3>Perpetual</h3>
            <ul>
              <li><BadgeCheck size={16} aria-hidden="true" /> One purchase, up to two machines</li>
              <li><BadgeCheck size={16} aria-hidden="true" /> Silent activation target</li>
              <li><BadgeCheck size={16} aria-hidden="true" /> 14-day evaluation target</li>
              <li><BadgeCheck size={16} aria-hidden="true" /> No subscription planned</li>
            </ul>
            <button type="button" disabled>LAUNCH PRICING NOT YET PUBLISHED</button>
          </article>

          <article className="warlock-pricing-card">
            <p>FACTORY BUILDS</p>
            <h3>Usage based</h3>
            <ul>
              <li><BadgeCheck size={16} aria-hidden="true" /> Credits tied to actual build work</li>
              <li><BadgeCheck size={16} aria-hidden="true" /> Failed gates do not become sellable releases</li>
              <li><BadgeCheck size={16} aria-hidden="true" /> Marketplace listing remains optional</li>
              <li><BadgeCheck size={16} aria-hidden="true" /> Temporary build files can be discarded</li>
            </ul>
            <button type="button" disabled>COST MODEL UNDER VALIDATION</button>
          </article>
        </div>
      </section>

      <section className="warlock-support-section" id="support">
        <div className="warlock-support-icon">
          <LifeBuoy size={30} strokeWidth={1.35} aria-hidden="true" />
        </div>
        <div>
          <p>WARLOCK SUPPORT</p>
          <h2>Diagnostics before guesswork.</h2>
          <span>
            Build failures, activation problems and release blockers are intended to carry
            machine-readable evidence so FARNAZ can diagnose the exact failed layer first.
          </span>
        </div>
        <button type="button" disabled>SUPPORT CENTER COMING WITH LAUNCH</button>
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

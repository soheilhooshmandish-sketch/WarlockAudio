import { createFileRoute } from "@tanstack/react-router";
import {
  Activity,
  ArrowRight,
  BadgeEuro,
  Check,
  ChevronRight,
  CircleHelp,
  Headphones,
  Layers3,
  LifeBuoy,
  Menu,
  MessageCircle,
  PackageCheck,
  Search,
  ShieldCheck,
  Sparkles,
  SunMoon,
  User,
  WandSparkles,
  X,
  Zap,
} from "lucide-react";
import { useEffect, useMemo, useState } from "react";

export const Route = createFileRoute("/")({ component: Home });

type Atmosphere = "morning" | "day" | "dusk" | "night";
type GeneratorMode = "auto" | "pro";

type Product = {
  name: string;
  type: string;
  note: string;
  status: string;
  realm: string;
};

const products: Product[] = [
  {
    name: "BLACK FROST",
    type: "BLACK METAL DISTORTION",
    note: "Cold attack. Raw edge. Controlled low end.",
    status: "IN DEVELOPMENT",
    realm: "frost",
  },
  {
    name: "ALIEN",
    type: "DISTORTION / MULTI-FX",
    note: "Hostile textures built around a biomechanical core.",
    status: "PROTOTYPE",
    realm: "alien",
  },
  {
    name: "ABYSS",
    type: "AMBIENT / REVERB / DELAY",
    note: "Long-form space, depth and frozen atmosphere.",
    status: "PROTOTYPE",
    realm: "abyss",
  },
  {
    name: "THALL",
    type: "MODERN METAL",
    note: "Tight transient control for modern heavy rhythm.",
    status: "PROTOTYPE",
    realm: "thall",
  },
  {
    name: "CHIMERA",
    type: "OCTAVE / HARMONY",
    note: "Hybrid voices, interval movement and layered width.",
    status: "PROTOTYPE",
    realm: "chimera",
  },
  {
    name: "VOID",
    type: "SPATIAL TEXTURE",
    note: "Pitch, grain, reverse and space in one evolving system.",
    status: "ACTIVE DEVELOPMENT",
    realm: "void",
  },
];

const steps = ["IDEA", "SOUND", "SKIN", "PREVIEW", "BUILD"];

function getAtmosphere(): Atmosphere {
  const hour = new Date().getHours();
  if (hour >= 6 && hour < 10) return "morning";
  if (hour >= 10 && hour < 17) return "day";
  if (hour >= 17 && hour < 21) return "dusk";
  return "night";
}

function Home() {
  const [atmosphere, setAtmosphere] = useState<Atmosphere>("night");
  const [mode, setMode] = useState<GeneratorMode>("auto");
  const [prompt, setPrompt] = useState("");
  const [refined, setRefined] = useState("");
  const [activeStep, setActiveStep] = useState(0);
  const [guideOpen, setGuideOpen] = useState(true);
  const [menuOpen, setMenuOpen] = useState(false);
  const [tourStep, setTourStep] = useState(0);

  useEffect(() => {
    setAtmosphere(getAtmosphere());
  }, []);

  const guideCopy = useMemo(() => {
    if (tourStep === 0) {
      return {
        eyebrow: "WELCOME TO WARLOCK",
        title: "Start with the sound, not the settings.",
        body: "Describe the result you want in your own words. I’ll guide the next step.",
        yes: "START TOUR",
        no: "NOT NOW",
      };
    }
    if (tourStep === 1) {
      return {
        eyebrow: "STEP 1 — IDEA",
        title: "Write the sound you hear in your head.",
        body: "Genre, feel, texture and effects are enough. Technical values are optional.",
        yes: "GOT IT",
        no: "SHOW EXAMPLE",
      };
    }
    if (tourStep === 2) {
      return {
        eyebrow: "STEP 2 — REFINE",
        title: "Refine before you build.",
        body: "WARLOCK can structure the idea and suggest useful audio choices without changing your intent.",
        yes: "CONTINUE",
        no: "SKIP",
      };
    }
    return {
      eyebrow: "READY",
      title: "You stay in control.",
      body: "No build or purchase starts without your explicit confirmation.",
      yes: "CREATE",
      no: "CLOSE",
    };
  }, [tourStep]);

  function handleRefine() {
    const source = prompt.trim();
    if (!source) {
      setPrompt(
        "Cold black-metal distortion with metallic attack, tight low-end, atmospheric reverb and a wide mix-ready character.",
      );
      setRefined("");
      return;
    }

    setRefined(
      `${source} — structured for ${mode === "pro" ? "PRO" : "AUTO"} mode with focus on character, signal flow, dynamics, space and mix context.`,
    );
    setActiveStep(1);
  }

  function handleGeneratePreview() {
    if (!prompt.trim()) {
      handleRefine();
      return;
    }
    setActiveStep((current) => Math.min(current + 1, 3));
    setGuideOpen(true);
    setTourStep(3);
  }

  return (
    <main className="warlock-site" data-atmosphere={atmosphere}>
      <div className="warlock-aurora" aria-hidden="true" />
      <div className="warlock-grid" aria-hidden="true" />

      <header className="warlock-header">
        <a className="warlock-brand" href="#top" aria-label="WARLOCK home">
          <span className="warlock-brand-mark" aria-hidden="true">
            <img src="/brand/warlock-core.svg" alt="" />
          </span>
          <span>
            <strong>WARLOCK</strong>
            <small>PLUGINS</small>
          </span>
        </a>

        <nav className="warlock-nav" aria-label="Primary navigation">
          <a href="#products">Products</a>
          <a href="#generator">Generate</a>
          <a href="#ecosystem">Presets</a>
          <a href="#ecosystem">Skins</a>
          <a href="#pricing">Pricing</a>
          <a href="#technology">Technology</a>
          <a href="#support">Support</a>
        </nav>

        <div className="warlock-header-actions">
          <button className="warlock-icon-button" aria-label="Search">
            <Search size={18} />
          </button>
          <button className="warlock-icon-button" aria-label="Account">
            <User size={18} />
          </button>
          <a className="warlock-button warlock-button-primary warlock-create" href="#generator">
            CREATE
          </a>
          <button
            className="warlock-icon-button warlock-mobile-menu"
            aria-label="Open menu"
            aria-expanded={menuOpen}
            onClick={() => setMenuOpen((open) => !open)}
          >
            {menuOpen ? <X size={20} /> : <Menu size={20} />}
          </button>
        </div>
      </header>

      {menuOpen ? (
        <div className="warlock-mobile-panel">
          {["Products", "Generate", "Presets", "Skins", "Pricing", "Technology", "Support"].map(
            (item) => (
              <a key={item} href={`#${item.toLowerCase()}`} onClick={() => setMenuOpen(false)}>
                {item}
                <ChevronRight size={18} />
              </a>
            ),
          )}
        </div>
      ) : null}

      <section className="warlock-hero" id="top">
        <div className="warlock-hero-copy">
          <p className="warlock-kicker">AI-ASSISTED AUDIO CREATION</p>
          <h1>
            DESCRIBE YOUR SOUND.
            <span>WARLOCK BUILDS IT.</span>
          </h1>
          <p className="warlock-lead">From sound idea to production-ready audio software.</p>

          <div className="warlock-prompt-shell" id="generator">
            <label htmlFor="sound-prompt">Write your sound idea</label>
            <textarea
              id="sound-prompt"
              value={prompt}
              onChange={(event) => {
                setPrompt(event.target.value);
                setRefined("");
                setActiveStep(0);
              }}
              placeholder="e.g. Cold black-metal distortion with metallic attack, tight low-end, atmospheric reverb and a wide, mix-ready character..."
            />

            <div className="warlock-prompt-bottom">
              <div className="warlock-segment" aria-label="Generator mode">
                <button
                  className={mode === "auto" ? "is-active" : ""}
                  onClick={() => setMode("auto")}
                >
                  AUTO
                </button>
                <button
                  className={mode === "pro" ? "is-active" : ""}
                  onClick={() => setMode("pro")}
                >
                  PRO
                </button>
              </div>
              <span>Preview UI · Factory connection follows validated API integration</span>
            </div>
          </div>

          {refined ? (
            <div className="warlock-refined">
              <div>
                <Sparkles size={17} />
                <strong>WARLOCK REFINE</strong>
              </div>
              <p>{refined}</p>
            </div>
          ) : null}

          <div className="warlock-hero-actions">
            <button className="warlock-button warlock-button-secondary" onClick={handleRefine}>
              <WandSparkles size={17} />
              REFINE
            </button>
            <button className="warlock-button warlock-button-primary" onClick={handleGeneratePreview}>
              GENERATE PREVIEW
              <ArrowRight size={17} />
            </button>
          </div>

          <div className="warlock-stepper" aria-label="Generation progress">
            {steps.map((step, index) => (
              <button
                key={step}
                className={index <= activeStep ? "is-complete" : ""}
                onClick={() => setActiveStep(index)}
              >
                <span>{index + 1}</span>
                <small>{step}</small>
              </button>
            ))}
          </div>

          <button className="warlock-powered" onClick={() => setGuideOpen(true)}>
            <MessageCircle size={15} />
            GUIDED BY FARNAZ
            <span>INTELLIGENCE · ENGINEERING · SOUND</span>
          </button>
        </div>

        <div className="warlock-hero-visual" aria-label="WARLOCK living system preview">
          <div className="warlock-orbit" aria-hidden="true">
            <div className="warlock-orbit-ring ring-one" />
            <div className="warlock-orbit-ring ring-two" />
          </div>
          <div className="warlock-emblem-stage">
            <img src="/brand/warlock-emblem.svg" alt="WARLOCK emblem" />
            <div className="warlock-emblem-glow" aria-hidden="true" />
          </div>
          <div className="warlock-soundline" aria-hidden="true">
            {Array.from({ length: 33 }, (_, index) => (
              <span key={index} style={{ "--wave": `${20 + ((index * 17) % 64)}%` } as React.CSSProperties} />
            ))}
          </div>
          <div className="warlock-visual-copy">
            <p>IDEAS BECOME INSTRUMENTS</p>
            <span>{atmosphere.toUpperCase()} MODE · LOCAL TIME</span>
          </div>
        </div>
      </section>

      <section className="warlock-how" id="technology">
        <div className="warlock-section-heading">
          <p>HOW IT WORKS</p>
          <h2>FROM IDEA TO INSTRUMENT</h2>
        </div>
        <div className="warlock-how-grid">
          {[
            ["01", "DESCRIBE", "Tell us what you hear.", MessageCircle],
            ["02", "REFINE", "FARNAZ structures the idea.", Sparkles],
            ["03", "DESIGN", "Validated DNA shapes the product.", Layers3],
            ["04", "VERIFY", "Production Health checks the result.", ShieldCheck],
            ["05", "DELIVER", "Package, license and download.", PackageCheck],
          ].map(([number, title, body, Icon]) => {
            const IconComponent = Icon as typeof MessageCircle;
            return (
              <article className="warlock-process-card" key={String(title)}>
                <div>
                  <span>{String(number)}</span>
                  <IconComponent size={20} />
                </div>
                <h3>{String(title)}</h3>
                <p>{String(body)}</p>
              </article>
            );
          })}
        </div>
      </section>

      <section className="warlock-products" id="products">
        <div className="warlock-section-row">
          <div className="warlock-section-heading">
            <p>SIGNATURE PRODUCTS</p>
            <h2>INSTRUMENTS FOR A WIDER REALITY</h2>
          </div>
          <button className="warlock-text-button">
            View all products <ArrowRight size={15} />
          </button>
        </div>

        <div className="warlock-product-grid">
          {products.map((product) => (
            <article className="warlock-product-card" data-realm={product.realm} key={product.name}>
              <div className="warlock-product-art" aria-hidden="true">
                <img src="/brand/warlock-core.svg" alt="" />
                <span />
              </div>
              <div className="warlock-product-copy">
                <p>{product.status}</p>
                <h3>{product.name}</h3>
                <small>{product.type}</small>
                <span>{product.note}</span>
              </div>
              <button aria-label={`Explore ${product.name}`}>
                <ChevronRight size={18} />
              </button>
            </article>
          ))}
        </div>
      </section>

      <section className="warlock-factory" id="ecosystem">
        <div className="warlock-factory-copy">
          <p className="warlock-kicker">WARLOCK FACTORY</p>
          <h2>YOUR IDEAS. OUR DNA. REAL PRODUCTS.</h2>
          <p>
            A guided creation layer built around validated audio modules, controlled visual DNA,
            Production Health and explicit build approval.
          </p>
          <a className="warlock-button warlock-button-primary" href="#generator">
            START CREATING
            <ArrowRight size={17} />
          </a>
        </div>
        <div className="warlock-factory-system">
          <div className="warlock-factory-core">
            <img src="/brand/warlock-emblem.svg" alt="" />
            <span>FACTORY</span>
          </div>
          <div className="warlock-factory-nodes">
            {[
              ["DNA SYSTEM", Layers3],
              ["FARNAZ ASSIST", Sparkles],
              ["PRODUCTION HEALTH", Activity],
              ["SKINS + PRESETS", Zap],
              ["LICENSE + DELIVERY", PackageCheck],
            ].map(([label, Icon]) => {
              const IconComponent = Icon as typeof Layers3;
              return (
                <div key={String(label)}>
                  <IconComponent size={18} />
                  <span>{String(label)}</span>
                </div>
              );
            })}
          </div>
        </div>
      </section>

      <section className="warlock-pricing" id="pricing">
        <div className="warlock-section-heading">
          <p>PRICING</p>
          <h2>BUILD ONCE. OR CREATE EVERY MONTH.</h2>
        </div>
        <div className="warlock-pricing-grid">
          {[
            ["GENERATE", "€9.90", "/ build", "One completed standard build", false],
            ["GENERATE PRO", "€19.90", "/ build", "Advanced sound + skin controls", true],
            ["CREATOR", "€19.90", "/ month", "Pro menu + monthly build allowance", false],
            ["STUDIO", "€39.90", "/ month", "Higher allowance + creator workflow", false],
          ].map(([name, price, cadence, note, featured]) => (
            <article className={`warlock-price-card ${featured ? "is-featured" : ""}`} key={String(name)}>
              {featured ? <span className="warlock-price-badge">POPULAR</span> : null}
              <p>{String(name)}</p>
              <h3>{String(price)}</h3>
              <small>{String(cadence)}</small>
              <span>{String(note)}</span>
              <ul>
                <li>
                  <Check size={14} /> Prompt Refiner
                </li>
                <li>
                  <Check size={14} /> Preview before build
                </li>
                <li>
                  <Check size={14} /> Production Health
                </li>
              </ul>
              <button className="warlock-button warlock-button-secondary">VIEW PLAN</button>
            </article>
          ))}
        </div>
      </section>

      <section className="warlock-support" id="support">
        <div className="warlock-section-heading">
          <p>SUPPORT & CONTACT</p>
          <h2>REAL HELP. CLEAR ROUTES.</h2>
        </div>
        <div className="warlock-support-grid">
          {[
            ["Technical Support", "Installation, activation and troubleshooting.", Headphones],
            ["Sales & Licensing", "Plans, commercial questions and partnerships.", BadgeEuro],
            ["Report a Build Problem", "Found an issue? Let us know what happened.", Activity],
            ["FAQ", "Quick answers to common questions.", CircleHelp],
            ["System Status", "Check current service availability.", LifeBuoy],
          ].map(([title, body, Icon]) => {
            const IconComponent = Icon as typeof Headphones;
            return (
              <button className="warlock-support-card" key={String(title)}>
                <span>
                  <IconComponent size={20} />
                </span>
                <div>
                  <strong>{String(title)}</strong>
                  <small>{String(body)}</small>
                </div>
                <ChevronRight size={18} />
              </button>
            );
          })}
        </div>
      </section>

      <footer className="warlock-footer">
        <div className="warlock-brand">
          <span className="warlock-brand-mark" aria-hidden="true">
            <img src="/brand/warlock-core.svg" alt="" />
          </span>
          <span>
            <strong>WARLOCK</strong>
            <small>SOUND BEYOND REALITY</small>
          </span>
        </div>
        <div>
          <a href="#products">Products</a>
          <a href="#generator">Generate</a>
          <a href="#pricing">Pricing</a>
          <a href="#support">Support</a>
        </div>
        <p>© 2026 WARLOCK PLUGINS. Preview redesign branch.</p>
      </footer>

      <button
        className="warlock-guide-orb"
        onClick={() => setGuideOpen((open) => !open)}
        aria-label="Toggle FARNAZ guide"
        aria-expanded={guideOpen}
      >
        <Sparkles size={19} />
        <span>FARNAZ</span>
      </button>

      {guideOpen ? (
        <aside className="warlock-guide" aria-label="FARNAZ guide">
          <div className="warlock-guide-head">
            <div>
              <span className="warlock-guide-avatar">F</span>
              <div>
                <strong>FARNAZ</strong>
                <small>
                  <i /> ONLINE · GUIDE
                </small>
              </div>
            </div>
            <button aria-label="Close FARNAZ guide" onClick={() => setGuideOpen(false)}>
              <X size={18} />
            </button>
          </div>

          <div className="warlock-guide-body">
            <p>{guideCopy.eyebrow}</p>
            <h3>{guideCopy.title}</h3>
            <span>{guideCopy.body}</span>
          </div>

          <div className="warlock-guide-actions">
            <button
              className="warlock-button warlock-button-primary"
              onClick={() => {
                if (tourStep >= 3) {
                  setGuideOpen(false);
                  document.getElementById("generator")?.scrollIntoView({ behavior: "smooth" });
                } else {
                  setTourStep((step) => step + 1);
                }
              }}
            >
              {guideCopy.yes}
            </button>
            <button
              className="warlock-button warlock-button-secondary"
              onClick={() => {
                if (tourStep === 1 && !prompt) {
                  setPrompt(
                    "Cold black-metal distortion with metallic attack, tight low-end, atmospheric reverb and a wide mix-ready character.",
                  );
                } else {
                  setGuideOpen(false);
                }
              }}
            >
              {guideCopy.no}
            </button>
          </div>

          <div className="warlock-guide-links">
            <button>
              <SunMoon size={16} /> Adaptive atmosphere: {atmosphere}
            </button>
            <button>
              <CircleHelp size={16} /> Help & support
            </button>
          </div>
        </aside>
      ) : null}
    </main>
  );
}

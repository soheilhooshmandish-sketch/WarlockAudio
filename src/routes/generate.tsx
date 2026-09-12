import { createFileRoute, Link } from "@tanstack/react-router";
import {
  ArrowLeft,
  ArrowRight,
  Check,
  ChevronRight,
  CircleHelp,
  Gauge,
  Layers3,
  LockKeyhole,
  MessageCircle,
  Paintbrush,
  RotateCcw,
  ShieldCheck,
  Sparkles,
  WandSparkles,
  X,
  Zap,
} from "lucide-react";
import { useMemo, useState } from "react";

export const Route = createFileRoute("/generate")({ component: GeneratePage });

type Mode = "auto" | "pro";
type Step = "idea" | "sound" | "skin" | "preview" | "build";

const steps: Array<{ id: Step; label: string; note: string }> = [
  { id: "idea", label: "IDEA", note: "Describe your sound" },
  { id: "sound", label: "SOUND", note: "Shape character" },
  { id: "skin", label: "SKIN", note: "Choose a realm" },
  { id: "preview", label: "PREVIEW", note: "Review the product" },
  { id: "build", label: "BUILD", note: "Factory handoff" },
];

const skinChoices = [
  { id: "auto", name: "AUTO DNA", note: "WARLOCK selects the best compatible visual family." },
  { id: "frost", name: "FROST", note: "Cold steel, black glass, restrained ice light." },
  { id: "alien", name: "ALIEN", note: "Biomechanical structure with hostile material language." },
  { id: "void", name: "VOID", note: "Cosmic depth, black glass and spatial geometry." },
];

function GeneratePage() {
  const [mode, setMode] = useState<Mode>("auto");
  const [step, setStep] = useState<Step>("idea");
  const [prompt, setPrompt] = useState("");
  const [refined, setRefined] = useState("");
  const [skin, setSkin] = useState("auto");
  const [guideOpen, setGuideOpen] = useState(true);
  const [character, setCharacter] = useState(68);
  const [tightness, setTightness] = useState(72);
  const [space, setSpace] = useState(38);
  const [width, setWidth] = useState(62);

  const currentIndex = steps.findIndex((item) => item.id === step);
  const selectedSkin = skinChoices.find((item) => item.id === skin) ?? skinChoices[0];

  const farnazCopy = useMemo(() => {
    if (step === "idea") {
      return {
        title: "Start with the result you want.",
        body: "Genre, texture, attack, space and mood are enough. Technical values are optional.",
      };
    }
    if (step === "sound") {
      return {
        title: "Shape the musical behavior.",
        body: "These controls describe intent. Factory DSP decisions stay behind the interface unless PRO is enabled.",
      };
    }
    if (step === "skin") {
      return {
        title: "Choose character, not clutter.",
        body: "Skins change the visual realm without breaking control layout, readability or WARLOCK identity.",
      };
    }
    if (step === "preview") {
      return {
        title: "Review before spending a build.",
        body: "Prompt refinement and preview should not consume a paid build. Confirm only when the result is right.",
      };
    }
    return {
      title: "Factory handoff is approval-gated.",
      body: "The live build button stays locked in this preview until the validated Factory API and payment entitlement are connected.",
    };
  }, [step]);

  function refinePrompt() {
    const source = prompt.trim();
    if (!source) {
      const example =
        "Cold black-metal distortion with metallic attack, tight low-end, atmospheric reverb and a wide mix-ready character.";
      setPrompt(example);
      setRefined("");
      return;
    }
    setRefined(
      `${source} — structured for ${mode.toUpperCase()} creation with explicit tone, transient, noise, spatial and mix-context intent.`,
    );
  }

  function next() {
    const index = Math.min(currentIndex + 1, steps.length - 1);
    setStep(steps[index].id);
  }

  function back() {
    const index = Math.max(currentIndex - 1, 0);
    setStep(steps[index].id);
  }

  return (
    <main className="warlock-app-page warlock-page-enter">
      <header className="warlock-app-header">
        <Link to="/" className="warlock-app-brand" aria-label="Back to WARLOCK home">
          <img src="/brand/warlock-core.svg" alt="" />
          <span>
            <strong>WARLOCK</strong>
            <small>GENERATE</small>
          </span>
        </Link>
        <nav>
          <Link to="/">Home</Link>
          <Link to="/generate" className="is-active">Generate</Link>
          <Link to="/pricing">Pricing</Link>
          <Link to="/support">Support</Link>
        </nav>
        <div className="warlock-app-mode">
          <button className={mode === "auto" ? "is-active" : ""} onClick={() => setMode("auto")}>AUTO</button>
          <button className={mode === "pro" ? "is-active" : ""} onClick={() => setMode("pro")}>PRO</button>
        </div>
      </header>

      <section className="warlock-generator-shell">
        <aside className="warlock-generator-steps" aria-label="Generator steps">
          <p className="warlock-app-kicker">CREATE PRODUCT</p>
          {steps.map((item, index) => (
            <button
              key={item.id}
              className={step === item.id ? "is-active" : index < currentIndex ? "is-complete" : ""}
              onClick={() => setStep(item.id)}
            >
              <span>{index < currentIndex ? <Check size={15} /> : index + 1}</span>
              <div>
                <strong>{item.label}</strong>
                <small>{item.note}</small>
              </div>
              <ChevronRight size={16} />
            </button>
          ))}
        </aside>

        <section className="warlock-generator-main">
          <div className="warlock-generator-titlebar">
            <div>
              <p>STEP {currentIndex + 1} / {steps.length}</p>
              <h1>{steps[currentIndex].label}</h1>
            </div>
            <span className="warlock-preview-pill">PREVIEW MODE</span>
          </div>

          {step === "idea" ? (
            <div className="warlock-step-panel is-visible">
              <div className="warlock-generator-copy">
                <h2>Describe the sound you want.</h2>
                <p>Write naturally. WARLOCK should translate the idea into a structured audio brief without forcing you to write a perfect prompt.</p>
              </div>
              <textarea
                className="warlock-large-prompt"
                value={prompt}
                onChange={(event) => {
                  setPrompt(event.target.value);
                  setRefined("");
                }}
                placeholder="e.g. Funeral black-metal distortion with cold metallic attack, controlled low-end, wide reverb and a slow phaser..."
              />
              <div className="warlock-generator-actions-row">
                <button className="warlock-button warlock-button-secondary" onClick={refinePrompt}>
                  <WandSparkles size={17} /> REFINE
                </button>
                <button className="warlock-button warlock-button-primary" onClick={next}>
                  CONTINUE <ArrowRight size={17} />
                </button>
              </div>
              {refined ? (
                <article className="warlock-refiner-result">
                  <div><Sparkles size={17} /><strong>WARLOCK REFINE</strong></div>
                  <p>{refined}</p>
                  <div className="warlock-tag-row">
                    <span>DARK</span><span>METALLIC</span><span>ATMOSPHERIC</span><span>MIX-READY</span>
                  </div>
                </article>
              ) : null}
            </div>
          ) : null}

          {step === "sound" ? (
            <div className="warlock-step-panel is-visible">
              <div className="warlock-generator-copy">
                <h2>Shape musical behavior.</h2>
                <p>Basic mode stays musical. PRO can later expose detailed DSP topology, routing, oversampling and advanced ranges.</p>
              </div>
              <div className="warlock-sound-controls">
                {[
                  ["CHARACTER", character, setCharacter],
                  ["TIGHTNESS", tightness, setTightness],
                  ["SPACE", space, setSpace],
                  ["WIDTH", width, setWidth],
                ].map(([label, value, setter]) => (
                  <label key={String(label)}>
                    <span><strong>{String(label)}</strong><small>{Number(value)}%</small></span>
                    <input
                      type="range"
                      min="0"
                      max="100"
                      value={Number(value)}
                      onChange={(event) => (setter as React.Dispatch<React.SetStateAction<number>>)(Number(event.target.value))}
                    />
                  </label>
                ))}
              </div>
              <div className="warlock-signal-summary">
                <Gauge size={18} />
                <div><strong>SOUND INTENT</strong><span>Cold attack · controlled low-end · moderate space · wide stereo image</span></div>
              </div>
            </div>
          ) : null}

          {step === "skin" ? (
            <div className="warlock-step-panel is-visible">
              <div className="warlock-generator-copy">
                <h2>Select a visual DNA.</h2>
                <p>The master WARLOCK geometry remains fixed. Product character changes through approved material, color and realm treatments.</p>
              </div>
              <div className="warlock-skin-grid">
                {skinChoices.map((choice) => (
                  <button key={choice.id} className={skin === choice.id ? "is-active" : ""} onClick={() => setSkin(choice.id)}>
                    <span className={`warlock-skin-orb realm-${choice.id}`}><Paintbrush size={20} /></span>
                    <strong>{choice.name}</strong>
                    <small>{choice.note}</small>
                    {skin === choice.id ? <Check size={17} /> : null}
                  </button>
                ))}
              </div>
              <div className="warlock-pro-note"><LockKeyhole size={17} /><span>Custom image, vector, logo and typography slots belong to PRO and remain bounded by layout safety rules.</span></div>
            </div>
          ) : null}

          {step === "preview" ? (
            <div className="warlock-step-panel is-visible">
              <div className="warlock-generator-copy">
                <h2>Review before build.</h2>
                <p>This summary is local preview state. It does not claim a real VST3 build has occurred.</p>
              </div>
              <div className="warlock-preview-card">
                <div className="warlock-preview-emblem"><img src="/brand/warlock-emblem.svg" alt="WARLOCK emblem" /></div>
                <div className="warlock-preview-summary">
                  <p>PRODUCT BRIEF</p>
                  <h3>{prompt || "Untitled WARLOCK product"}</h3>
                  <dl>
                    <div><dt>MODE</dt><dd>{mode.toUpperCase()}</dd></div>
                    <div><dt>SKIN</dt><dd>{selectedSkin.name}</dd></div>
                    <div><dt>CHARACTER</dt><dd>{character}%</dd></div>
                    <div><dt>TIGHTNESS</dt><dd>{tightness}%</dd></div>
                    <div><dt>SPACE</dt><dd>{space}%</dd></div>
                    <div><dt>WIDTH</dt><dd>{width}%</dd></div>
                  </dl>
                </div>
              </div>
            </div>
          ) : null}

          {step === "build" ? (
            <div className="warlock-step-panel is-visible">
              <div className="warlock-build-gate">
                <ShieldCheck size={30} />
                <p>FACTORY CONNECTION</p>
                <h2>Approval gate ready.</h2>
                <span>The visual flow is ready, but this preview does not fake payment, compilation, license creation or delivery. Connect the validated Factory API before enabling the paid build action.</span>
                <button className="warlock-button warlock-button-primary" disabled>
                  <LockKeyhole size={16} /> FACTORY API NOT CONNECTED
                </button>
              </div>
            </div>
          ) : null}

          <footer className="warlock-generator-footer">
            <button className="warlock-button warlock-button-quiet" onClick={back} disabled={currentIndex === 0}>
              <ArrowLeft size={16} /> BACK
            </button>
            <button className="warlock-button warlock-button-quiet" onClick={() => {
              setStep("idea"); setPrompt(""); setRefined(""); setSkin("auto"); setCharacter(68); setTightness(72); setSpace(38); setWidth(62);
            }}>
              <RotateCcw size={16} /> RESET
            </button>
            {currentIndex < steps.length - 1 ? (
              <button className="warlock-button warlock-button-primary" onClick={next}>NEXT <ArrowRight size={16} /></button>
            ) : null}
          </footer>
        </section>

        <aside className="warlock-farnaz-rail">
          <div className="warlock-farnaz-card">
            <div className="warlock-farnaz-head">
              <span>F</span>
              <div><strong>FARNAZ</strong><small><i /> ONLINE · GUIDE</small></div>
              <button onClick={() => setGuideOpen((value) => !value)} aria-label="Toggle guide">{guideOpen ? <X size={17} /> : <MessageCircle size={17} />}</button>
            </div>
            {guideOpen ? (
              <>
                <div className="warlock-farnaz-body"><Sparkles size={18} /><h3>{farnazCopy.title}</h3><p>{farnazCopy.body}</p></div>
                <div className="warlock-farnaz-yesno"><button className="is-yes" onClick={next}>YES</button><button onClick={() => setGuideOpen(false)}>NO</button></div>
                <div className="warlock-farnaz-links">
                  <button><CircleHelp size={15} /> Explain this step</button>
                  <button><Zap size={15} /> Show an example</button>
                </div>
              </>
            ) : null}
          </div>

          <article className="warlock-live-summary">
            <p>LIVE SUMMARY</p>
            <div><span>MODE</span><strong>{mode.toUpperCase()}</strong></div>
            <div><span>STEP</span><strong>{steps[currentIndex].label}</strong></div>
            <div><span>SKIN</span><strong>{selectedSkin.name}</strong></div>
            <div><span>BUILD</span><strong>NOT STARTED</strong></div>
          </article>
        </aside>
      </section>
    </main>
  );
}

import { useMemo, useState } from "react";
import { createFileRoute, Link } from "@tanstack/react-router";
import { z } from "zod";
import {
  ArrowLeft,
  ArrowRight,
  BadgeCheck,
  CircleDot,
  Cpu,
  LockKeyhole,
  ShieldCheck,
  Sparkles,
  WandSparkles,
} from "lucide-react";
import { submitFactoryIntake } from "@/lib/factory/intake.server";
import "../warlock-generate.css";

const searchSchema = z.object({
  prompt: z.string().max(2000).optional().default(""),
});

export const Route = createFileRoute("/generate")({
  validateSearch: (search) => searchSchema.parse(search),
  component: GeneratePage,
});

type Mode = "auto" | "pro";
type StepId = "idea" | "review" | "sound" | "skin" | "preview";
type IntakeState =
  | { status: "idle" }
  | { status: "pending" }
  | { status: "staged"; requestId: string }
  | { status: "error"; message: string };

type SoundProfile = {
  character: "tight" | "raw" | "cold" | "doom";
  lowEnd: "controlled" | "full" | "huge";
  width: "mono-compatible" | "wide";
  space: "dry" | "atmospheric";
};

type SkinProfile = "auto" | "dark-steel" | "black-frost" | "biomechanical";

const steps: Array<{ id: StepId; label: string; eyebrow: string }> = [
  { id: "idea", label: "IDEA", eyebrow: "01" },
  { id: "review", label: "REFINE", eyebrow: "02" },
  { id: "sound", label: "SOUND", eyebrow: "03" },
  { id: "skin", label: "SKIN", eyebrow: "04" },
  { id: "preview", label: "PREVIEW", eyebrow: "05" },
];

const promptExamples = [
  "Cold black-metal distortion with metallic attack, controlled low-end and atmospheric space.",
  "Modern thall distortion with hard transient definition, tight lows and mix-ready output.",
  "Raw doom fuzz with huge low-end, slow bloom and dark wide ambience.",
];

function OptionButton({
  active,
  label,
  detail,
  onClick,
}: {
  active: boolean;
  label: string;
  detail: string;
  onClick: () => void;
}) {
  return (
    <button
      type="button"
      className="wg-option"
      data-active={active ? "true" : "false"}
      onClick={onClick}
      aria-pressed={active}
    >
      <span className="wg-option-dot" aria-hidden="true">
        <CircleDot size={16} strokeWidth={1.6} />
      </span>
      <span>
        <strong>{label}</strong>
        <small>{detail}</small>
      </span>
    </button>
  );
}

function GeneratePage() {
  const search = Route.useSearch();
  const initialPrompt = search.prompt.trim();
  const [stepIndex, setStepIndex] = useState(0);
  const [mode, setMode] = useState<Mode>("auto");
  const [prompt, setPrompt] = useState(initialPrompt);
  const [originalPrompt, setOriginalPrompt] = useState(initialPrompt);
  const [sound, setSound] = useState<SoundProfile>({
    character: "tight",
    lowEnd: "controlled",
    width: "mono-compatible",
    space: "dry",
  });
  const [skin, setSkin] = useState<SkinProfile>("auto");
  const [intake, setIntake] = useState<IntakeState>({ status: "idle" });

  const step = steps[stepIndex];
  const canContinue = prompt.trim().length >= 12;

  const factoryPrompt = useMemo(() => {
    const selectedSkin = skin === "auto" ? "warlock-auto-skin" : skin;
    return [
      prompt.trim(),
      "",
      "WARLOCK GENERATOR WORKSPACE SELECTIONS",
      "product_type=effect-pedal",
      `mode=${mode}`,
      `character=${sound.character}`,
      `low_end=${sound.lowEnd}`,
      `width=${sound.width}`,
      `space=${sound.space}`,
      `skin=${selectedSkin}`,
      "commercial_release_allowed=false",
    ].join("\n");
  }, [mode, prompt, skin, sound]);

  function goNext() {
    if (!canContinue || stepIndex >= steps.length - 1) return;
    if (step.id === "idea") setOriginalPrompt(prompt.trim());
    setIntake({ status: "idle" });
    setStepIndex((current) => Math.min(current + 1, steps.length - 1));
  }

  function goBack() {
    setIntake({ status: "idle" });
    setStepIndex((current) => Math.max(current - 1, 0));
  }

  async function stageRequest() {
    if (!canContinue || intake.status === "pending") return;
    setIntake({ status: "pending" });
    try {
      const result = await submitFactoryIntake({
        data: {
          prompt: factoryPrompt,
          clientRequestId: crypto.randomUUID(),
        },
      });
      if (!result.ok) {
        setIntake({ status: "error", message: result.message });
        return;
      }
      setIntake({ status: "staged", requestId: result.requestId });
    } catch {
      setIntake({
        status: "error",
        message: "Factory intake could not be reached. No request was staged.",
      });
    }
  }

  return (
    <main className="wg-shell">
      <header className="wg-header">
        <Link to="/" className="wg-brand" aria-label="Back to WARLOCK home">
          <span className="wg-brand-mark" aria-hidden="true">W</span>
          <span>WARLOCK</span>
        </Link>
        <div className="wg-header-state">
          <span>GENERATOR V1</span>
          <i aria-hidden="true" />
          <span>FACTORY INTAKE FAIL-CLOSED</span>
        </div>
      </header>

      <section className="wg-layout">
        <aside className="wg-rail" aria-label="Generator progress">
          <p>CREATE PRODUCT</p>
          <nav>
            {steps.map((item, index) => (
              <button
                key={item.id}
                type="button"
                className="wg-step"
                data-state={index === stepIndex ? "active" : index < stepIndex ? "done" : "future"}
                onClick={() => {
                  if (index <= stepIndex || (canContinue && index === stepIndex + 1)) {
                    if (step.id === "idea" && index > stepIndex) setOriginalPrompt(prompt.trim());
                    setStepIndex(index);
                    setIntake({ status: "idle" });
                  }
                }}
              >
                <span>{item.eyebrow}</span>
                <strong>{item.label}</strong>
                {index < stepIndex ? <BadgeCheck size={16} aria-hidden="true" /> : null}
              </button>
            ))}
          </nav>
          <div className="wg-rail-note">
            <ShieldCheck size={18} strokeWidth={1.6} aria-hidden="true" />
            <span>
              Nothing here can publish, charge a customer, issue a license or clear a release gate.
            </span>
          </div>
        </aside>

        <section className="wg-workspace" aria-labelledby="wg-title">
          <div className="wg-workspace-head">
            <div>
              <p>STEP {step.eyebrow} / 05</p>
              <h1 id="wg-title">{step.label}</h1>
            </div>
            <div className="wg-mode" role="group" aria-label="Generator mode">
              <button type="button" data-active={mode === "auto"} onClick={() => setMode("auto")}>AUTO</button>
              <button type="button" data-active={mode === "pro"} onClick={() => setMode("pro")}>PRO</button>
            </div>
          </div>

          {step.id === "idea" ? (
            <div className="wg-panel">
              <div className="wg-panel-title">
                <Sparkles size={20} strokeWidth={1.7} aria-hidden="true" />
                <div>
                  <h2>Describe the product.</h2>
                  <p>Effect pedals are the validated V1 path. Other product classes stay locked until their DNA is proven.</p>
                </div>
              </div>

              <label className="wg-field-label" htmlFor="wg-prompt">SOUND / PRODUCT INTENT</label>
              <textarea
                id="wg-prompt"
                className="wg-prompt"
                value={prompt}
                onChange={(event) => setPrompt(event.target.value)}
                rows={7}
                maxLength={2000}
                placeholder="Describe the tone, genre, dynamics, controls and behavior you want..."
              />
              <div className="wg-input-meta">
                <span>{prompt.length} / 2000</span>
                <span>{canContinue ? "READY" : "MINIMUM 12 CHARACTERS"}</span>
              </div>

              <div className="wg-example-row">
                {promptExamples.map((example) => (
                  <button key={example} type="button" onClick={() => setPrompt(example)}>{example}</button>
                ))}
              </div>

              <div className="wg-product-types">
                <OptionButton active label="EFFECT PEDAL" detail="Validated V1 creation path" onClick={() => undefined} />
                <button type="button" className="wg-locked-card" disabled><LockKeyhole size={16} /> AMP / CAB · PRIVATE BETA</button>
                <button type="button" className="wg-locked-card" disabled><LockKeyhole size={16} /> SYNTH / DRUMS · FUTURE</button>
              </div>
            </div>
          ) : null}

          {step.id === "review" ? (
            <div className="wg-panel">
              <div className="wg-panel-title">
                <WandSparkles size={20} strokeWidth={1.7} aria-hidden="true" />
                <div>
                  <h2>Review before intelligence changes anything.</h2>
                  <p>FARNAZ Prompt Refiner is not connected to the public website yet. No AI rewrite is being pretended here.</p>
                </div>
              </div>

              <div className="wg-compare">
                <div>
                  <label className="wg-field-label">ORIGINAL</label>
                  <div className="wg-readonly">{originalPrompt || "No original prompt captured yet."}</div>
                </div>
                <div>
                  <label className="wg-field-label" htmlFor="wg-working-prompt">WORKING PROMPT · MANUAL EDIT ONLY</label>
                  <textarea
                    id="wg-working-prompt"
                    className="wg-prompt wg-prompt-compact"
                    value={prompt}
                    onChange={(event) => setPrompt(event.target.value)}
                    rows={6}
                    maxLength={2000}
                  />
                </div>
              </div>

              <div className="wg-honesty-note">
                <LockKeyhole size={17} aria-hidden="true" />
                <span>REFINE / ENHANCE / PRO intelligence remains locked until the real FARNAZ service contract is connected and tested.</span>
              </div>
            </div>
          ) : null}

          {step.id === "sound" ? (
            <div className="wg-panel">
              <div className="wg-panel-title">
                <Cpu size={20} strokeWidth={1.7} aria-hidden="true" />
                <div>
                  <h2>Choose musical intent, not fake DSP.</h2>
                  <p>These selections are build intent only. They do not claim a DSP engine already exists.</p>
                </div>
              </div>

              <div className="wg-control-group">
                <label className="wg-field-label">CHARACTER</label>
                <div className="wg-option-grid">
                  {(["tight", "raw", "cold", "doom"] as const).map((value) => (
                    <OptionButton
                      key={value}
                      active={sound.character === value}
                      label={value.toUpperCase()}
                      detail={{ tight: "Fast attack / controlled lows", raw: "Less polished / more edge", cold: "Metallic / restrained warmth", doom: "Heavy / slower bloom" }[value]}
                      onClick={() => setSound((current) => ({ ...current, character: value }))}
                    />
                  ))}
                </div>
              </div>

              <div className="wg-control-group">
                <label className="wg-field-label">LOW END</label>
                <div className="wg-option-grid wg-option-grid-3">
                  {(["controlled", "full", "huge"] as const).map((value) => (
                    <OptionButton
                      key={value}
                      active={sound.lowEnd === value}
                      label={value.toUpperCase()}
                      detail={{ controlled: "Tight mix placement", full: "Balanced body", huge: "Maximum weight request" }[value]}
                      onClick={() => setSound((current) => ({ ...current, lowEnd: value }))}
                    />
                  ))}
                </div>
              </div>

              <div className="wg-control-group">
                <label className="wg-field-label">IMAGE / SPACE</label>
                <div className="wg-option-grid">
                  <OptionButton active={sound.width === "mono-compatible"} label="MONO-COMPATIBLE" detail="Center-first width intent" onClick={() => setSound((current) => ({ ...current, width: "mono-compatible" }))} />
                  <OptionButton active={sound.width === "wide"} label="WIDE" detail="Expanded stereo intent" onClick={() => setSound((current) => ({ ...current, width: "wide" }))} />
                  <OptionButton active={sound.space === "dry"} label="DRY" detail="No added ambience requested" onClick={() => setSound((current) => ({ ...current, space: "dry" }))} />
                  <OptionButton active={sound.space === "atmospheric"} label="ATMOSPHERIC" detail="Space layer requested" onClick={() => setSound((current) => ({ ...current, space: "atmospheric" }))} />
                </div>
              </div>

              {mode === "pro" ? (
                <div className="wg-pro-note">
                  <strong>PRO MODE</strong>
                  <span>Advanced topology, filter ranges, oversampling and latency controls stay locked until the ProductSpec schema is connected to this workspace.</span>
                </div>
              ) : null}
            </div>
          ) : null}

          {step.id === "skin" ? (
            <div className="wg-panel">
              <div className="wg-panel-title">
                <ShieldCheck size={20} strokeWidth={1.7} aria-hidden="true" />
                <div>
                  <h2>Select a visual realm.</h2>
                  <p>This records visual intent only. Canonical WARLOCK geometry remains immutable.</p>
                </div>
              </div>
              <div className="wg-skin-grid">
                {([
                  ["auto", "AUTO SKIN", "Factory chooses from validated brand DNA"],
                  ["dark-steel", "DARK STEEL", "Machined black / steel / restrained ice"],
                  ["black-frost", "BLACK FROST", "Cold steel / ice-white treatment"],
                  ["biomechanical", "BIOMECHANICAL", "Organic machinery around canonical geometry"],
                ] as const).map(([value, label, detail]) => (
                  <button
                    key={value}
                    type="button"
                    className="wg-skin-card"
                    data-active={skin === value}
                    onClick={() => setSkin(value)}
                    aria-pressed={skin === value}
                  >
                    <span className={`wg-skin-swatch wg-skin-${value}`} aria-hidden="true" />
                    <strong>{label}</strong>
                    <small>{detail}</small>
                  </button>
                ))}
              </div>
              <div className="wg-honesty-note">
                <LockKeyhole size={17} aria-hidden="true" />
                <span>Custom uploads, generated artwork and personalized logo slots are not enabled in V1.</span>
              </div>
            </div>
          ) : null}

          {step.id === "preview" ? (
            <div className="wg-panel">
              <div className="wg-panel-title">
                <BadgeCheck size={20} strokeWidth={1.7} aria-hidden="true" />
                <div>
                  <h2>Review the exact request boundary.</h2>
                  <p>Staging creates an auditable Factory intake request only. It does not compile, charge, license, publish or approve a release.</p>
                </div>
              </div>

              <div className="wg-summary-grid">
                <div><span>PRODUCT</span><strong>EFFECT PEDAL</strong></div>
                <div><span>MODE</span><strong>{mode.toUpperCase()}</strong></div>
                <div><span>CHARACTER</span><strong>{sound.character.toUpperCase()}</strong></div>
                <div><span>LOW END</span><strong>{sound.lowEnd.toUpperCase()}</strong></div>
                <div><span>WIDTH</span><strong>{sound.width.toUpperCase()}</strong></div>
                <div><span>SPACE</span><strong>{sound.space.toUpperCase()}</strong></div>
                <div><span>SKIN</span><strong>{skin.toUpperCase()}</strong></div>
                <div><span>RELEASE</span><strong>LOCKED</strong></div>
              </div>

              <label className="wg-field-label">EXACT STAGED TEXT</label>
              <pre className="wg-payload">{factoryPrompt}</pre>

              <button
                type="button"
                className="wg-stage-button"
                onClick={stageRequest}
                disabled={!canContinue || intake.status === "pending" || intake.status === "staged"}
              >
                <Sparkles size={18} aria-hidden="true" />
                {intake.status === "pending"
                  ? "STAGING REQUEST..."
                  : intake.status === "staged"
                    ? "REQUEST STAGED"
                    : "STAGE FACTORY REQUEST"}
              </button>

              {intake.status !== "idle" ? (
                <div className="wg-intake-state" data-state={intake.status} role="status" aria-live="polite">
                  {intake.status === "staged" ? <ShieldCheck size={17} aria-hidden="true" /> : <LockKeyhole size={17} aria-hidden="true" />}
                  <span>
                    {intake.status === "staged"
                      ? `Request ${intake.requestId.slice(0, 8)} staged. Build has not started.`
                      : intake.status === "pending"
                        ? "Sending through the protected server-side intake boundary..."
                        : intake.message}
                  </span>
                </div>
              ) : null}
            </div>
          ) : null}

          <footer className="wg-actions">
            <button type="button" className="wg-back" onClick={goBack} disabled={stepIndex === 0}>
              <ArrowLeft size={17} aria-hidden="true" /> BACK
            </button>
            <div className="wg-action-status">
              <span>{stepIndex + 1} / {steps.length}</span>
              <i><span style={{ width: `${((stepIndex + 1) / steps.length) * 100}%` }} /></i>
            </div>
            {stepIndex < steps.length - 1 ? (
              <button type="button" className="wg-next" onClick={goNext} disabled={!canContinue}>
                NEXT <ArrowRight size={17} aria-hidden="true" />
              </button>
            ) : (
              <Link to="/" className="wg-home-link">BACK TO COMPANY SITE</Link>
            )}
          </footer>
        </section>
      </section>
    </main>
  );
}

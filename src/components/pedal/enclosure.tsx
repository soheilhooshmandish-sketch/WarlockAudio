import { PRESETS } from "@/lib/audio/presets";
import { RIFFS } from "@/lib/audio/riff-synth";
import {
  DEFAULT_PARAMS,
  fmtCold,
  fmtGain,
  fmtGate,
  fmtGrim,
  fmtLevel,
  fmtRelease,
  fmtTrim,
} from "@/lib/audio/types";
import { cn } from "@/lib/utils";
import { usePedal } from "@/store/pedal-store";
import { Knob } from "./knob";
import { PedalLcd } from "./lcd";
import { LedMeter } from "./meter";
import { StompSwitch } from "./stomp";

function Screw({ className }: { className?: string }) {
  return (
    <span className={cn("screw", className)} aria-hidden>
      <span className="screw-slot" />
    </span>
  );
}

function MiniSwitch({
  name,
  on,
  onToggle,
}: {
  name: string;
  on: boolean;
  onToggle: () => void;
}) {
  return (
    <button
      type="button"
      aria-pressed={on}
      aria-label={name}
      onClick={onToggle}
      className="flex flex-col items-center gap-1.5 focus-visible:ring-2 focus-visible:ring-frost/60"
    >
      <span className={cn("rocker", on && "rocker-on")}>
        <span className="rocker-knob" />
      </span>
      <span className="font-display text-[0.52rem] tracking-[0.14em] text-bone-dim uppercase">
        {name}
      </span>
    </button>
  );
}

export function PedalEnclosure() {
  const gain = usePedal((s) => s.gain);
  const cold = usePedal((s) => s.cold);
  const grim = usePedal((s) => s.grim);
  const level = usePedal((s) => s.level);
  const gate = usePedal((s) => s.gate);
  const gateRelease = usePedal((s) => s.gateRelease);
  const diode = usePedal((s) => s.diode);
  const cab = usePedal((s) => s.cab);
  const trim = usePedal((s) => s.trim);
  const lofi = usePedal((s) => s.lofi);
  const engaged = usePedal((s) => s.engaged);
  const awake = usePedal((s) => s.awake);
  const presetId = usePedal((s) => s.presetId);
  const activeRiff = usePedal((s) => s.activeRiff);
  const source = usePedal((s) => s.source);
  const fileName = usePedal((s) => s.fileName);
  const setParam = usePedal((s) => s.setParam);
  const toggleEngage = usePedal((s) => s.toggleEngage);

  const preset = PRESETS.find((p) => p.id === presetId) ?? PRESETS[0];
  const lcdTitle = !awake ? "Standby" : engaged ? preset.name : "Bypass";
  const lcdSub = !awake
    ? "Press the stomp"
    : source === "riff" && activeRiff
      ? `Riff · ${RIFFS.find((r) => r.id === activeRiff)?.name ?? activeRiff}`
      : source === "mic"
        ? "Input · microphone"
        : source === "file"
          ? `Input · ${fileName || "file"}`
          : preset.epithet;

  return (
    <div className="pedal-stage relative mx-auto w-full">
      <div className="pedal-jacks" aria-hidden>
        <span className="jack-label">In</span>
        <span className="jack" />
        <span className="jack-stack">
          <span className="jack-dc" />
          <span className="jack-polarity">9V DC −</span>
        </span>
        <span className="jack" />
        <span className="jack-label">Out</span>
      </div>

      <div className="pedal-shell relative w-full">
        <span className="shell-wall" aria-hidden />
        <span className="lid-seam" aria-hidden />
        <span className="uv-keepout" aria-hidden />
        <span className="face-bevel" aria-hidden />
        <span className="scratch" aria-hidden />
        <Screw className="absolute top-3.5 left-3.5 z-20" />
        <Screw className="absolute top-3.5 right-3.5 z-20" />
        <Screw className="absolute bottom-3.5 left-3.5 z-20" />
        <Screw className="absolute right-3.5 bottom-3.5 z-20" />
        <span className="ground-lug" title="Ground lug" aria-hidden />

        <div className="relative z-10 px-4 pt-4 pb-4 sm:px-6">
          <div className="nameplate mx-auto">
            <div className="logo-well">
              <img
                src="/warlock-logo.png"
                alt="The Warlock"
                width={349}
                height={349}
                className="pedal-logo"
                draggable={false}
              />
            </div>
            <p className="silkscreen-sub">Chainsaw Distortion</p>
            <p className="model-stamp">Warlock Audio · Model 1510</p>
          </div>

          <div className="mt-5 grid grid-cols-3 justify-items-center gap-x-2 gap-y-4">
            <Knob
              label="Gain"
              readout={fmtGain(gain)}
              value={gain}
              defaultValue={DEFAULT_PARAMS.gain}
              onChange={(v) => setParam("gain", v)}
              lit={engaged && awake}
            />
            <Knob
              label="Cold"
              readout={fmtCold(cold)}
              value={cold}
              defaultValue={DEFAULT_PARAMS.cold}
              onChange={(v) => setParam("cold", v)}
              lit={engaged && awake}
            />
            <Knob
              label="Grim"
              readout={fmtGrim(grim)}
              value={grim}
              defaultValue={DEFAULT_PARAMS.grim}
              onChange={(v) => setParam("grim", v)}
              lit={engaged && awake}
            />
            <Knob
              label="Gate"
              readout={fmtGate(gate)}
              value={gate}
              defaultValue={DEFAULT_PARAMS.gate}
              onChange={(v) => setParam("gate", v)}
              lit={engaged && awake}
            />
            <Knob
              label="Release"
              readout={fmtRelease(gateRelease)}
              value={gateRelease}
              defaultValue={DEFAULT_PARAMS.gateRelease}
              onChange={(v) => setParam("gateRelease", v)}
              lit={engaged && awake}
            />
            <Knob
              label="Level"
              readout={fmtLevel(level)}
              value={level}
              defaultValue={DEFAULT_PARAMS.level}
              onChange={(v) => setParam("level", v)}
              lit={engaged && awake}
              led="green"
            />
          </div>

          <div className="mt-5 flex items-center justify-center gap-6 sm:gap-8">
            <MiniSwitch
              name="Asym"
              on={diode >= 0.5}
              onToggle={() => setParam("diode", diode >= 0.5 ? 0 : 1)}
            />
            <MiniSwitch
              name="Cab IR"
              on={cab >= 0.5}
              onToggle={() => setParam("cab", cab >= 0.5 ? 0 : 1)}
            />
            <MiniSwitch
              name="Lo-Fi"
              on={lofi >= 0.5}
              onToggle={() => setParam("lofi", lofi >= 0.5 ? 0 : 1)}
            />
          </div>

          <div className="mt-5 flex items-end gap-3">
            <Knob
              label="Trim"
              readout={fmtTrim(trim)}
              value={trim}
              defaultValue={DEFAULT_PARAMS.trim}
              onChange={(v) => setParam("trim", v)}
              lit={engaged && awake}
              compact
            />
            <LedMeter engaged={engaged && awake} />
            <div className="min-w-0 flex-1">
              <PedalLcd title={lcdTitle} subtitle={lcdSub} engaged={engaged && awake} />
            </div>
            <StompSwitch engaged={engaged} awake={awake} onToggle={() => void toggleEngage()} />
          </div>

          <p className="chassis-legend" aria-hidden>
            Hammond 1590XX · powder coat · UV print
          </p>
          <p className="chassis-legend chassis-legend-dim" aria-hidden>
            High gain · 100 mA · 9V DC negative tip
          </p>
        </div>
      </div>

      <div className="pedal-feet" aria-hidden>
        <span />
        <span />
      </div>
    </div>
  );
}

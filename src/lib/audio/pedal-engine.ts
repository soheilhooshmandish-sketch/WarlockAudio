import { DEFAULT_PARAMS, type PedalParams } from "./types";

/**
 * Minimum-latency Warlock path (native IIRs, meter is a tap, gate optional):
 *   TRIM → [GATE] → HPF 320 → drive → diode → 1.2 kHz / Grim / Cold → cab LP → Level
 */

const GAIN_DB_MAX = 50;
const COLD_DB_MIN = -12;
const COLD_DB_SPAN = 27;
const GRIM_DB_MIN = -15;
const GRIM_DB_SPAN = 30;
const LEVEL_DB_MIN = -24;
const LEVEL_DB_SPAN = 30;

const SYM_CURVE = makeDiodeClip(0);
const ASYM_CURVE = makeDiodeClip(1);

function gainKnobToDb(n: number): number {
  return GAIN_DB_MAX * Math.pow(Math.min(1, Math.max(0, n)), 0.5);
}

function dbToGain(db: number): number {
  return Math.pow(10, db / 20);
}

function gateKnobToThresh(n: number): number {
  const db = -100 + Math.min(1, Math.max(0, n)) * 80;
  return Math.pow(10, db / 20);
}

function gateReleaseToSec(n: number): number {
  return (5 + 195 * Math.pow(Math.min(1, Math.max(0, n)), 0.5)) / 1000;
}

function makeDiodeClip(asym: number, samples = 256): Float32Array<ArrayBuffer> {
  const curve = new Float32Array(new ArrayBuffer(samples * 4));
  const useAsym = asym > 0.5;
  const pos = useAsym ? 0.35 : 0.65;
  const neg = useAsym ? 0.75 : 0.65;
  for (let i = 0; i < samples; i++) {
    const x = (i * 2) / samples - 1;
    curve[i] = Math.max(-neg, Math.min(pos, x));
  }
  return curve;
}

function snap(param: AudioParam, value: number, t: number) {
  param.cancelScheduledValues(t);
  param.setValueAtTime(value, t);
}

export class PedalEngine {
  readonly ctx: AudioContext;
  readonly input: GainNode;
  readonly analyser: AnalyserNode;

  private preHp: BiquadFilterNode;
  private preGain: GainNode;
  private diode: WaveShaperNode;
  private fixedPeak: BiquadFilterNode;
  private grimFilter: BiquadFilterNode;
  private coldFilter: BiquadFilterNode;
  private cabLp: BiquadFilterNode;
  private gate: AudioWorkletNode | null = null;
  private gateThresh: AudioParam | null = null;
  private gateRelease: AudioParam | null = null;
  private out: GainNode;
  private bypassGain: GainNode;
  private sum: GainNode;
  private params: PedalParams = { ...DEFAULT_PARAMS };
  private lastDiode = -1;
  private wetOnline = true;
  private dryOnline = false;

  constructor(ctx: AudioContext) {
    this.ctx = ctx;
    this.input = ctx.createGain();
    this.input.gain.value = 1;
    this.input.channelCount = 1;
    this.input.channelCountMode = "explicit";

    this.preHp = ctx.createBiquadFilter();
    this.preHp.type = "highpass";
    this.preHp.frequency.value = 320;
    this.preHp.Q.value = 0.707;

    this.preGain = ctx.createGain();

    this.diode = ctx.createWaveShaper();
    this.diode.curve = SYM_CURVE;
    this.diode.oversample = "none";

    this.fixedPeak = ctx.createBiquadFilter();
    this.fixedPeak.type = "peaking";
    this.fixedPeak.frequency.value = 1200;
    this.fixedPeak.Q.value = 3;
    this.fixedPeak.gain.value = 14;

    this.grimFilter = ctx.createBiquadFilter();
    this.grimFilter.type = "peaking";
    this.grimFilter.frequency.value = 950;
    this.grimFilter.Q.value = 2.2;

    this.coldFilter = ctx.createBiquadFilter();
    this.coldFilter.type = "peaking";
    this.coldFilter.frequency.value = 4100;
    this.coldFilter.Q.value = 1.8;

    this.cabLp = ctx.createBiquadFilter();
    this.cabLp.type = "lowpass";
    this.cabLp.frequency.value = 5600;
    this.cabLp.Q.value = 0.7;

    this.out = ctx.createGain();
    this.bypassGain = ctx.createGain();
    this.bypassGain.gain.value = 1;
    this.sum = ctx.createGain();

    this.analyser = ctx.createAnalyser();
    this.analyser.fftSize = 256;
    this.analyser.smoothingTimeConstant = 0.5;

    this.input.connect(this.preHp);
    this.preHp.connect(this.preGain);
    this.preGain.connect(this.diode);
    this.diode.connect(this.fixedPeak);
    this.fixedPeak.connect(this.grimFilter);
    this.grimFilter.connect(this.coldFilter);
    this.coldFilter.connect(this.cabLp);
    this.cabLp.connect(this.out);
    this.out.connect(this.sum);
    this.sum.connect(ctx.destination);
    this.sum.connect(this.analyser);

    this.apply(this.params);
  }

  installGate(node: AudioWorkletNode) {
    if (this.gate) return;
    this.gate = node;
    this.gateThresh = node.parameters.get("threshold") ?? null;
    this.gateRelease = node.parameters.get("release") ?? null;
    try {
      this.input.disconnect(this.preHp);
    } catch {
      /* */
    }
    this.input.connect(node);
    node.connect(this.preHp);
    this.apply(this.params);
  }

  apply(next: PedalParams) {
    this.params = next;
    const t = this.ctx.currentTime;
    const drive = dbToGain(gainKnobToDb(next.gain));
    const grimDb = GRIM_DB_MIN + next.grim * GRIM_DB_SPAN;
    const coldDb = COLD_DB_MIN + next.cold * COLD_DB_SPAN;
    const cabOn = next.cab >= 0.5;
    const lp = next.lofi > 0.5 ? 4200 : cabOn ? 5600 : 18000;

    snap(this.input.gain, dbToGain(-18 + next.trim * 36), t);
    snap(this.preGain.gain, drive, t);
    snap(this.grimFilter.gain, grimDb, t);
    snap(this.coldFilter.gain, coldDb, t);
    snap(this.cabLp.frequency, lp, t);

    if (next.diode !== this.lastDiode) {
      this.diode.curve = next.diode > 0.5 ? ASYM_CURVE : SYM_CURVE;
      this.lastDiode = next.diode;
    }

    this.gateThresh?.setValueAtTime(gateKnobToThresh(next.gate), t);
    this.gateRelease?.setValueAtTime(gateReleaseToSec(next.gateRelease), t);

    const levelLin = dbToGain(LEVEL_DB_MIN + next.level * LEVEL_DB_SPAN);
    snap(this.out.gain, next.engaged ? levelLin : 0, t);
    this.route(next.engaged);
  }

  private route(engaged: boolean) {
    if (engaged && !this.wetOnline) {
      this.out.connect(this.sum);
      this.wetOnline = true;
    } else if (!engaged && this.wetOnline) {
      try {
        this.out.disconnect(this.sum);
      } catch {
        /* */
      }
      this.wetOnline = false;
    }
    if (!engaged && !this.dryOnline) {
      this.input.connect(this.bypassGain);
      this.bypassGain.connect(this.sum);
      this.dryOnline = true;
    } else if (engaged && this.dryOnline) {
      try {
        this.bypassGain.disconnect();
      } catch {
        /* */
      }
      try {
        this.input.disconnect(this.bypassGain);
      } catch {
        /* */
      }
      this.dryOnline = false;
    }
  }

  click() {
    const t = this.ctx.currentTime;
    const o = this.ctx.createOscillator();
    const g = this.ctx.createGain();
    o.type = "square";
    o.frequency.value = 90;
    g.gain.setValueAtTime(0.12, t);
    g.gain.exponentialRampToValueAtTime(0.0001, t + 0.04);
    o.connect(g);
    g.connect(this.ctx.destination);
    o.start(t);
    o.stop(t + 0.05);
  }
}

let singleton: PedalEngine | null = null;

export function getEngine(): PedalEngine | null {
  return singleton;
}

async function attachGate(ctx: AudioContext, engine: PedalEngine) {
  try {
    await ctx.audioWorklet.addModule("/worklets/noise-gate.js?v=7");
    engine.installGate(new AudioWorkletNode(ctx, "noise-gate"));
  } catch (err) {
    console.warn("Noise gate worklet unavailable", err);
  }
}

export async function bootEngine(): Promise<PedalEngine> {
  if (singleton) {
    if (singleton.ctx.state !== "running") await singleton.ctx.resume();
    return singleton;
  }
  const ctx = new AudioContext({ latencyHint: 0.003 });
  singleton = new PedalEngine(ctx);
  const ready = ctx.state === "running" ? Promise.resolve() : ctx.resume();
  void attachGate(ctx, singleton);
  await ready;
  return singleton;
}

if (import.meta.hot) {
  import.meta.hot.dispose(() => {
    const engine = singleton;
    singleton = null;
    void engine?.ctx.close();
  });
}

import type { RiffDef, RiffId } from "./types";

const E5 = [82.41, 123.47, 164.81];
const G5 = [98.0, 146.83, 196.0];
const Fs5 = [92.5, 138.59, 185.0];
const D5 = [73.42, 110.0, 146.83];
const A5 = [110.0, 164.81, 220.0];
const Bb5 = [58.27, 116.54, 174.61];
const C5 = [65.41, 98.0, 130.81];

export const RIFFS: RiffDef[] = [
  { id: "tremolo", name: "Tremolo Wall", epithet: "E-minor blizzard", bpm: 168, bars: 4 },
  { id: "frostveil", name: "Frostveil", epithet: "Slow ice arpeggio", bpm: 76, bars: 4 },
  { id: "warpath", name: "Warpath", epithet: "Palm-mute charge", bpm: 152, bars: 4 },
  { id: "drone", name: "Ice Drone", epithet: "Tritone hymn", bpm: 60, bars: 4 },
];

type Hit = {
  beat: number;
  notes: number[];
  dur: number;
  mute?: boolean;
  vel?: number;
};

function tremolo(chord: number[], start: number, beats: number, subdiv = 4, vel = 0.82): Hit[] {
  const hits: Hit[] = [];
  const step = 1 / subdiv;
  for (let b = start; b < start + beats - 0.001; b += step) {
    hits.push({ beat: b, notes: chord, dur: step * 0.92, vel: vel * (0.88 + Math.random() * 0.2) });
  }
  return hits;
}

function pattern(id: RiffId): Hit[] {
  switch (id) {
    case "tremolo":
      return [
        ...tremolo(E5, 0, 2),
        ...tremolo(G5, 2, 1),
        ...tremolo(Fs5, 3, 1),
        ...tremolo(E5, 4, 2),
        ...tremolo(D5, 6, 1),
        ...tremolo(G5, 7, 1),
        ...tremolo(E5, 8, 4),
      ];
    case "frostveil": {
      const arp = [82.41, 123.47, 164.81, 196.0, 246.94, 329.63, 246.94, 196.0];
      const hits: Hit[] = [];
      arp.forEach((n, i) => {
        hits.push({ beat: i * 2, notes: [n, n * 2], dur: 2.1, vel: 0.7 });
      });
      return hits;
    }
    case "warpath": {
      const hits: Hit[] = [];
      for (let b = 0; b < 8; b += 0.5) {
        hits.push({ beat: b, notes: E5, dur: 0.22, mute: true, vel: 0.9 });
      }
      hits.push({ beat: 8, notes: G5, dur: 1.6, vel: 0.95 });
      hits.push({ beat: 10, notes: Fs5, dur: 0.9, vel: 0.9 });
      hits.push({ beat: 11, notes: E5, dur: 1.4, vel: 0.95 });
      for (let b = 12; b < 16; b += 0.5) {
        hits.push({ beat: b, notes: C5, dur: 0.22, mute: true, vel: 0.85 });
      }
      return hits;
    }
    case "drone":
      return [
        { beat: 0, notes: E5, dur: 7.5, vel: 0.62 },
        { beat: 0.4, notes: Bb5, dur: 7.2, vel: 0.48 },
        { beat: 8, notes: A5, dur: 3.8, vel: 0.58 },
        { beat: 8.3, notes: E5, dur: 3.6, vel: 0.5 },
        { beat: 12, notes: D5, dur: 3.8, vel: 0.6 },
        { beat: 12.3, notes: Bb5, dur: 3.6, vel: 0.45 },
      ];
  }
}

function playNote(
  ctx: AudioContext,
  dest: AudioNode,
  freq: number,
  when: number,
  dur: number,
  vel: number,
  mute: boolean,
) {
  const o1 = ctx.createOscillator();
  const g = ctx.createGain();
  const f = ctx.createBiquadFilter();
  o1.type = "sawtooth";
  o1.frequency.value = freq;
  f.type = "lowpass";
  f.Q.value = 0.85;
  const peak = mute ? 520 : 2400;
  f.frequency.setValueAtTime(Math.min(peak * 2.2, 6000), when);
  f.frequency.exponentialRampToValueAtTime(peak, when + 0.07);
  const peakG = vel * (mute ? 0.12 : 0.22);
  g.gain.setValueAtTime(peakG * 0.7, when);
  g.gain.exponentialRampToValueAtTime(peakG, when + 0.002);
  g.gain.exponentialRampToValueAtTime(peakG * (mute ? 0.18 : 0.45), when + Math.min(0.08, dur * 0.3));
  g.gain.exponentialRampToValueAtTime(0.0001, when + dur);
  o1.connect(f);
  f.connect(g);
  g.connect(dest);
  o1.start(when);
  o1.stop(when + dur + 0.02);
}

export class RiffPlayer {
  private bus: GainNode | null = null;
  private token = 0;
  private loopTimer: number | null = null;
  playing: RiffId | null = null;

  stop(ctx: AudioContext) {
    this.token += 1;
    this.playing = null;
    if (this.loopTimer != null) {
      window.clearTimeout(this.loopTimer);
      this.loopTimer = null;
    }
    if (this.bus) {
      const t = ctx.currentTime;
      this.bus.gain.cancelScheduledValues(t);
      this.bus.gain.setTargetAtTime(0, t, 0.02);
      const old = this.bus;
      window.setTimeout(() => {
        try {
          old.disconnect();
        } catch {
          /* already gone */
        }
      }, 80);
      this.bus = null;
    }
  }

  start(ctx: AudioContext, dest: AudioNode, id: RiffId, loop = true) {
    this.stop(ctx);
    const def = RIFFS.find((r) => r.id === id);
    if (!def) return;
    this.playing = id;
    const my = this.token;
    const run = () => {
      if (this.token !== my) return;
      const bus = ctx.createGain();
      bus.gain.value = 1;
      bus.connect(dest);
      this.bus = bus;
      const beat = 60 / def.bpm;
      const t0 = ctx.currentTime + 128 / ctx.sampleRate;
      const hits = pattern(id);
      for (const hit of hits) {
        const when = t0 + hit.beat * beat;
        const vel = (hit.vel ?? 0.8) / Math.sqrt(hit.notes.length);
        for (const n of hit.notes) {
          playNote(ctx, bus, n, when, hit.dur * beat, vel, Boolean(hit.mute));
        }
      }
      const totalMs = def.bars * 4 * beat * 1000 + 40;
      if (loop) {
        this.loopTimer = window.setTimeout(() => {
          if (this.token !== my) return;
          if (this.bus) {
            try {
              this.bus.disconnect();
            } catch {
              /* */
            }
          }
          run();
        }, totalMs);
      }
    };
    run();
  }
}

export const riffPlayer = new RiffPlayer();

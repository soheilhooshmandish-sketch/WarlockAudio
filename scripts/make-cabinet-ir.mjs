/**
 * Synthetic 4×12 guitar-cab IR (48 kHz, mono, 16-bit).
 * Close-mic SM57 on a Vintage 30-ish cone — not a third-party sample.
 */
import { writeFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const FS = 48000;
const N = 2048;

function mulberry32(seed) {
  return () => {
    seed |= 0;
    seed = (seed + 0x6d2b79f5) | 0;
    let t = Math.imul(seed ^ (seed >>> 15), 1 | seed);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

function makeBiquad(type, freq, Q, gainDb, fs) {
  const w0 = (2 * Math.PI * freq) / fs;
  const cos = Math.cos(w0);
  const sin = Math.sin(w0);
  const A = 10 ** (gainDb / 40);
  const alpha = sin / (2 * Q);
  let b0;
  let b1;
  let b2;
  let a0;
  let a1;
  let a2;
  if (type === "highpass") {
    b0 = (1 + cos) / 2;
    b1 = -(1 + cos);
    b2 = (1 + cos) / 2;
    a0 = 1 + alpha;
    a1 = -2 * cos;
    a2 = 1 - alpha;
  } else if (type === "lowpass") {
    b0 = (1 - cos) / 2;
    b1 = 1 - cos;
    b2 = (1 - cos) / 2;
    a0 = 1 + alpha;
    a1 = -2 * cos;
    a2 = 1 - alpha;
  } else {
    b0 = 1 + alpha * A;
    b1 = -2 * cos;
    b2 = 1 - alpha * A;
    a0 = 1 + alpha / A;
    a1 = -2 * cos;
    a2 = 1 - alpha / A;
  }
  return { b0: b0 / a0, b1: b1 / a0, b2: b2 / a0, a1: a1 / a0, a2: a2 / a0, z1: 0, z2: 0 };
}

function run(input, bq) {
  const out = new Float64Array(input.length);
  for (let i = 0; i < input.length; i++) {
    const x = input[i];
    const y = bq.b0 * x + bq.z1;
    bq.z1 = bq.b1 * x - bq.a1 * y + bq.z2;
    bq.z2 = bq.b2 * x - bq.a2 * y;
    out[i] = y;
  }
  return out;
}

function dftMag(ir, freq, fs) {
  let re = 0;
  let im = 0;
  for (let i = 0; i < ir.length; i++) {
    const w = (2 * Math.PI * freq * i) / fs;
    re += ir[i] * Math.cos(w);
    im -= ir[i] * Math.sin(w);
  }
  return Math.hypot(re, im);
}

const rand = mulberry32(0x4e49484c);
let x = new Float64Array(N);
x[0] = 1;
for (let i = 0; i < 96; i++) {
  x[i] += (rand() * 2 - 1) * 0.12 * Math.exp(-i / 22);
}

const stack = [
  ["highpass", 78, 0.72, 0],
  ["peaking", 130, 1.05, 3.2],
  ["peaking", 380, 1.25, -6.5],
  ["peaking", 820, 1.4, -2.8],
  ["peaking", 1250, 2.1, 2.2],
  ["peaking", 2450, 1.35, 5.0],
  ["peaking", 3350, 1.7, 6.5],
  ["peaking", 4900, 1.5, -3.5],
  ["lowpass", 5600, 0.82, 0],
  ["lowpass", 7400, 0.65, 0],
];
for (const [type, freq, Q, g] of stack) {
  x = run(x, makeBiquad(type, freq, Q, g, FS));
}

const ir = new Float64Array(N);
const taps = [
  [0, 1],
  [Math.round(0.00074 * FS), 0.32],
  [Math.round(0.00138 * FS), -0.2],
  [Math.round(0.00215 * FS), 0.14],
  [Math.round(0.00355 * FS), -0.09],
  [Math.round(0.0057 * FS), 0.06],
];
for (const [delay, gain] of taps) {
  for (let i = 0; i < N - delay; i++) ir[i + delay] += x[i] * gain;
}

const fade = 180;
for (let i = 0; i < fade; i++) {
  const w = 0.5 - 0.5 * Math.cos((Math.PI * (fade - i)) / fade);
  ir[N - fade + i] *= w;
}

const mag = dftMag(ir, 1200, FS) || 1;
const scale = 0.72 / mag;
for (let i = 0; i < N; i++) ir[i] *= scale;

const pcm = Buffer.alloc(N * 2);
for (let i = 0; i < N; i++) {
  const s = Math.max(-1, Math.min(1, ir[i]));
  pcm.writeInt16LE((s * 32767) | 0, i * 2);
}

const header = Buffer.alloc(44);
header.write("RIFF", 0);
header.writeUInt32LE(36 + pcm.length, 4);
header.write("WAVE", 8);
header.write("fmt ", 12);
header.writeUInt32LE(16, 16);
header.writeUInt16LE(1, 20);
header.writeUInt16LE(1, 22);
header.writeUInt32LE(FS, 24);
header.writeUInt32LE(FS * 2, 28);
header.writeUInt16LE(2, 32);
header.writeUInt16LE(16, 34);
header.write("data", 36);
header.writeUInt32LE(pcm.length, 40);

const out = join(dirname(fileURLToPath(import.meta.url)), "..", "public", "CabinetIR.wav");
writeFileSync(out, Buffer.concat([header, pcm]));

const peak = ir.reduce((m, v) => Math.max(m, Math.abs(v)), 0);
const rms = Math.sqrt(ir.reduce((s, v) => s + v * v, 0) / N);
const magDb = (f) => (20 * Math.log10(dftMag(ir, f, FS) || 1e-9)).toFixed(1);
console.log(
  JSON.stringify(
    {
      out,
      samples: N,
      peak: peak.toFixed(3),
      rms: rms.toFixed(4),
      "H1200": magDb(1200),
      "H950": magDb(950),
      "H4100": magDb(4100),
      "H8000": magDb(8000),
    },
    null,
    2,
  ),
);

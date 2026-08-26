import type { PedalPreset } from "./types";

/** JUCE NormalisableRange(5, 200, 1, 0.5) → 0–1. */
function rel(ms: number) {
  return ((ms - 5) / 195) ** 2;
}

const factory = {
  gain: 0.64,
  cold: 24 / 27,
  grim: 25 / 30,
  level: 0.8,
  gate: 0.375,
  gateRelease: rel(30),
  diode: 0,
  cab: 1,
  trim: 0.5,
  lofi: 0,
};

export const PRESETS: PedalPreset[] = [
  {
    id: "svartskog",
    name: "Svartskog",
    epithet: "Factory · 30 ms rel",
    params: { ...factory },
  },
  {
    id: "dodsvinter",
    name: "Dødsvinter",
    epithet: "Tight gate, 10 ms rel",
    params: { ...factory, gain: 0.7, cold: 1, grim: 0.28, level: 0.76, gate: 0.7, gateRelease: rel(10) },
  },
  {
    id: "isvind",
    name: "Isvind",
    epithet: "Icy top, gate open",
    params: { ...factory, gain: 0.42, cold: 0.92, grim: 0.42, level: 0.82, gate: 0.2, gateRelease: rel(80) },
  },
  {
    id: "nattlig",
    name: "Nattlig Trone",
    epithet: "Asym +0.35/−0.75",
    params: { ...factory, gain: 0.58, cold: 0.48, grim: 1, level: 0.78, gate: 0.45, gateRelease: rel(50), diode: 1 },
  },
  {
    id: "likblek",
    name: "Likblek",
    epithet: "All knobs maxed",
    params: { ...factory, gain: 1, cold: 1, grim: 1, level: 0.68, gate: 0.8, gateRelease: 1, diode: 1 },
  },
  {
    id: "katedral",
    name: "Katedral",
    epithet: "DI, no cab, stone",
    params: { ...factory, gain: 0.25, cold: 0.32, grim: 0.52, level: 0.86, gate: 0.15, gateRelease: rel(120), cab: 0, lofi: 1 },
  },
];

export function presetById(id: string) {
  return PRESETS.find((p) => p.id === id) ?? PRESETS[0];
}

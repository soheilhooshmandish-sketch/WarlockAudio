export type PedalParams = {
  gain: number;
  cold: number;
  grim: number;
  level: number;
  gate: number;
  gateRelease: number;
  diode: number;
  cab: number;
  trim: number;
  lofi: number;
  engaged: boolean;
};

export type PedalPreset = {
  id: string;
  name: string;
  epithet: string;
  params: Omit<PedalParams, "engaged">;
};

export type UserPreset = {
  id: string;
  name: string;
  savedAt: number;
  params: Omit<PedalParams, "engaged">;
};

export type RiffId = "tremolo" | "frostveil" | "warpath" | "drone";

export type RiffDef = {
  id: RiffId;
  name: string;
  epithet: string;
  bpm: number;
  bars: number;
};

export const DEFAULT_PARAMS: PedalParams = {
  gain: 0.64,
  cold: 24 / 27,
  grim: 25 / 30,
  level: 0.8,
  gate: 0.375,
  gateRelease: (25 / 195) ** 2,
  diode: 0,
  cab: 1,
  trim: 0.5,
  lofi: 0,
  engaged: true,
};

export function fmtGain(n: number) {
  return `${(50 * Math.pow(n, 0.5)).toFixed(1)} dB`;
}
export function fmtCold(n: number) {
  const db = -12 + n * 27;
  return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
export function fmtGrim(n: number) {
  const db = -15 + n * 30;
  return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
export function fmtGate(n: number) {
  return `${(-100 + n * 80).toFixed(0)} dB`;
}
export function fmtRelease(n: number) {
  return `${(5 + 195 * Math.pow(n, 0.5)).toFixed(0)} ms`;
}
export function fmtLevel(n: number) {
  const db = -24 + n * 30;
  return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
export function fmtTrim(n: number) {
  const db = -18 + n * 36;
  return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}

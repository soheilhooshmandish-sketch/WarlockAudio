import { create } from "zustand";
import { PRESETS } from "@/lib/audio/presets";
import { bootEngine, getEngine } from "@/lib/audio/pedal-engine";
import { riffPlayer } from "@/lib/audio/riff-synth";
import {
  DEFAULT_PARAMS,
  type PedalParams,
  type RiffId,
  type UserPreset,
} from "@/lib/audio/types";

type SourceKind = "idle" | "riff" | "mic" | "file";

const USER_PRESETS_KEY = "warlock-user-presets";

function snapshotParams(state: PedalParams): Omit<PedalParams, "engaged"> {
  return {
    gain: state.gain,
    cold: state.cold,
    grim: state.grim,
    level: state.level,
    gate: state.gate,
    gateRelease: state.gateRelease,
    diode: state.diode,
    cab: state.cab,
    trim: state.trim,
    lofi: state.lofi,
  };
}

function readUserPresets(): UserPreset[] {
  if (typeof window === "undefined") return [];
  try {
    const raw = window.localStorage.getItem(USER_PRESETS_KEY);
    if (!raw) return [];
    const parsed = JSON.parse(raw) as unknown;
    return Array.isArray(parsed) ? (parsed as UserPreset[]) : [];
  } catch {
    return [];
  }
}

function writeUserPresets(list: UserPreset[]) {
  if (typeof window === "undefined") return;
  window.localStorage.setItem(USER_PRESETS_KEY, JSON.stringify(list));
}

type PedalState = PedalParams & {
  awake: boolean;
  presetId: string;
  source: SourceKind;
  activeRiff: RiffId | null;
  micError: string | null;
  fileName: string | null;
  userPresets: UserPreset[];
  wake: () => Promise<void>;
  setParam: <K extends keyof PedalParams>(key: K, value: PedalParams[K]) => void;
  toggleEngage: () => Promise<void>;
  loadPreset: (id: string) => void;
  loadUserPreset: (id: string) => void;
  saveUserPreset: (name: string) => void;
  deleteUserPreset: (id: string) => void;
  playRiff: (id: RiffId) => Promise<void>;
  stopRiff: () => void;
  startMic: () => Promise<void>;
  stopMic: () => void;
  loadFile: (file: File) => Promise<void>;
  stopFile: () => void;
};

let micStream: MediaStream | null = null;
let micNode: MediaStreamAudioSourceNode | null = null;
let fileNode: AudioBufferSourceNode | null = null;
let fileGain: GainNode | null = null;

export const usePedal = create<PedalState>((set, get) => ({
  ...DEFAULT_PARAMS,
  awake: false,
  presetId: PRESETS[0].id,
  source: "idle",
  activeRiff: null,
  micError: null,
  fileName: null,
  userPresets: readUserPresets(),

  wake: async () => {
    const engine = await bootEngine();
    engine.apply(get());
    set({ awake: true });
  },

  setParam: (key, value) => {
    set({ [key]: value } as Partial<PedalState>);
    const engine = getEngine();
    if (engine) engine.apply(get());
  },

  toggleEngage: async () => {
    const wasAsleep = !get().awake;
    if (wasAsleep) await get().wake();
    const engine = getEngine();
    engine?.click();
    const engaged = wasAsleep ? true : !get().engaged;
    set({ engaged });
    if (engine) engine.apply(get());
  },

  loadPreset: (id) => {
    const preset = PRESETS.find((p) => p.id === id);
    if (!preset) return;
    set({ ...preset.params, presetId: id });
    const engine = getEngine();
    if (engine) engine.apply(get());
  },

  loadUserPreset: (id) => {
    const preset = get().userPresets.find((p) => p.id === id);
    if (!preset) return;
    set({ ...preset.params, presetId: id });
    const engine = getEngine();
    if (engine) engine.apply(get());
  },

  saveUserPreset: (name) => {
    const trimmed = name.trim().slice(0, 32);
    if (!trimmed) return;
    const next: UserPreset = {
      id: `user-${Date.now()}`,
      name: trimmed,
      savedAt: Date.now(),
      params: snapshotParams(get()),
    };
    const list = [next, ...get().userPresets].slice(0, 12);
    writeUserPresets(list);
    set({ userPresets: list, presetId: next.id });
  },

  deleteUserPreset: (id) => {
    const list = get().userPresets.filter((p) => p.id !== id);
    writeUserPresets(list);
    set({ userPresets: list });
  },

  playRiff: async (id) => {
    if (!get().awake) await get().wake();
    if (!get().engaged) {
      set({ engaged: true });
      const engine = getEngine();
      if (engine) engine.apply(get());
    }
    get().stopMic();
    get().stopFile();
    const engine = getEngine();
    if (!engine) return;
    riffPlayer.start(engine.ctx, engine.input, id, true);
    set({ source: "riff", activeRiff: id, micError: null });
  },

  stopRiff: () => {
    const engine = getEngine();
    if (engine) riffPlayer.stop(engine.ctx);
    set({ activeRiff: null, source: get().source === "riff" ? "idle" : get().source });
  },

  startMic: async () => {
    if (!get().awake) await get().wake();
    get().stopRiff();
    get().stopFile();
    try {
      micStream = await navigator.mediaDevices.getUserMedia({
        audio: {
          echoCancellation: false,
          noiseSuppression: false,
          autoGainControl: false,
          channelCount: 1,
          latency: 0,
        } as MediaTrackConstraints,
      });
      const engine = getEngine();
      if (!engine) return;
      micNode = engine.ctx.createMediaStreamSource(micStream);
      micNode.connect(engine.input);
      if (!get().engaged) {
        set({ engaged: true });
        engine.apply(get());
      }
      set({ source: "mic", micError: null, activeRiff: null });
    } catch {
      set({
        micError: "Microphone blocked — allow access, or play a riff instead.",
        source: "idle",
      });
    }
  },

  stopMic: () => {
    if (micNode) {
      try {
        micNode.disconnect();
      } catch {
        /* */
      }
      micNode = null;
    }
    if (micStream) {
      micStream.getTracks().forEach((t) => t.stop());
      micStream = null;
    }
    if (get().source === "mic") set({ source: "idle" });
  },

  loadFile: async (file) => {
    if (!get().awake) await get().wake();
    get().stopRiff();
    get().stopMic();
    const engine = getEngine();
    if (!engine) return;
    try {
      const buf = await file.arrayBuffer();
      const audio = await engine.ctx.decodeAudioData(buf.slice(0));
      get().stopFile();
      fileGain = engine.ctx.createGain();
      fileGain.gain.value = 0.9;
      fileGain.connect(engine.input);
      fileNode = engine.ctx.createBufferSource();
      fileNode.buffer = audio;
      fileNode.loop = true;
      fileNode.connect(fileGain);
      fileNode.start();
      if (!get().engaged) {
        set({ engaged: true });
        engine.apply(get());
      }
      set({ source: "file", fileName: file.name, activeRiff: null, micError: null });
    } catch {
      set({
        micError: "Couldn’t decode that file. Use WAV, MP3, OGG, or FLAC.",
        source: get().source === "file" ? "idle" : get().source,
      });
    }
  },

  stopFile: () => {
    if (fileNode) {
      try {
        fileNode.stop();
        fileNode.disconnect();
      } catch {
        /* */
      }
      fileNode = null;
    }
    if (fileGain) {
      try {
        fileGain.disconnect();
      } catch {
        /* */
      }
      fileGain = null;
    }
    if (get().source === "file") set({ source: "idle", fileName: null });
  },
}));

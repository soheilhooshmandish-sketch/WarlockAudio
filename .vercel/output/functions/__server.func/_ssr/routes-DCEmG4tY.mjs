import { i as __toESM } from "../_runtime.mjs";
import { L as require_react, v as require_jsx_runtime } from "../_libs/@tanstack/react-router+[...].mjs";
import { i as Mic, r as Square, t as Upload } from "../_libs/lucide-react.mjs";
import { n as clsx, t as cva } from "../_libs/class-variance-authority+clsx.mjs";
import { t as twMerge } from "../_libs/tailwind-merge.mjs";
import { t as create } from "../_libs/zustand.mjs";
//#region node_modules/.nitro/vite/services/ssr/assets/routes-DCEmG4tY.js
var import_react = /* @__PURE__ */ __toESM(require_react());
var import_jsx_runtime = require_jsx_runtime();
/** JUCE NormalisableRange(5, 200, 1, 0.5) → 0–1. */
function rel(ms) {
	return ((ms - 5) / 195) ** 2;
}
var factory = {
	gain: .64,
	cold: 24 / 27,
	grim: 25 / 30,
	level: .8,
	gate: .375,
	gateRelease: rel(30),
	diode: 0,
	cab: 1,
	trim: .5,
	lofi: 0
};
var PRESETS = [
	{
		id: "svartskog",
		name: "Svartskog",
		epithet: "Factory · 30 ms rel",
		params: { ...factory }
	},
	{
		id: "dodsvinter",
		name: "Dødsvinter",
		epithet: "Tight gate, 10 ms rel",
		params: {
			...factory,
			gain: .7,
			cold: 1,
			grim: .28,
			level: .76,
			gate: .7,
			gateRelease: rel(10)
		}
	},
	{
		id: "isvind",
		name: "Isvind",
		epithet: "Icy top, gate open",
		params: {
			...factory,
			gain: .42,
			cold: .92,
			grim: .42,
			level: .82,
			gate: .2,
			gateRelease: rel(80)
		}
	},
	{
		id: "nattlig",
		name: "Nattlig Trone",
		epithet: "Asym +0.35/−0.75",
		params: {
			...factory,
			gain: .58,
			cold: .48,
			grim: 1,
			level: .78,
			gate: .45,
			gateRelease: rel(50),
			diode: 1
		}
	},
	{
		id: "likblek",
		name: "Likblek",
		epithet: "All knobs maxed",
		params: {
			...factory,
			gain: 1,
			cold: 1,
			grim: 1,
			level: .68,
			gate: .8,
			gateRelease: 1,
			diode: 1
		}
	},
	{
		id: "katedral",
		name: "Katedral",
		epithet: "DI, no cab, stone",
		params: {
			...factory,
			gain: .25,
			cold: .32,
			grim: .52,
			level: .86,
			gate: .15,
			gateRelease: rel(120),
			cab: 0,
			lofi: 1
		}
	}
];
var E5 = [
	82.41,
	123.47,
	164.81
];
var G5 = [
	98,
	146.83,
	196
];
var Fs5 = [
	92.5,
	138.59,
	185
];
var D5 = [
	73.42,
	110,
	146.83
];
var A5 = [
	110,
	164.81,
	220
];
var Bb5 = [
	58.27,
	116.54,
	174.61
];
var C5 = [
	65.41,
	98,
	130.81
];
var RIFFS = [
	{
		id: "tremolo",
		name: "Tremolo Wall",
		epithet: "E-minor blizzard",
		bpm: 168,
		bars: 4
	},
	{
		id: "frostveil",
		name: "Frostveil",
		epithet: "Slow ice arpeggio",
		bpm: 76,
		bars: 4
	},
	{
		id: "warpath",
		name: "Warpath",
		epithet: "Palm-mute charge",
		bpm: 152,
		bars: 4
	},
	{
		id: "drone",
		name: "Ice Drone",
		epithet: "Tritone hymn",
		bpm: 60,
		bars: 4
	}
];
function tremolo(chord, start, beats, subdiv = 4, vel = .82) {
	const hits = [];
	const step = 1 / subdiv;
	for (let b = start; b < start + beats - .001; b += step) hits.push({
		beat: b,
		notes: chord,
		dur: step * .92,
		vel: vel * (.88 + Math.random() * .2)
	});
	return hits;
}
function pattern(id) {
	switch (id) {
		case "tremolo": return [
			...tremolo(E5, 0, 2),
			...tremolo(G5, 2, 1),
			...tremolo(Fs5, 3, 1),
			...tremolo(E5, 4, 2),
			...tremolo(D5, 6, 1),
			...tremolo(G5, 7, 1),
			...tremolo(E5, 8, 4)
		];
		case "frostveil": {
			const arp = [
				82.41,
				123.47,
				164.81,
				196,
				246.94,
				329.63,
				246.94,
				196
			];
			const hits = [];
			arp.forEach((n, i) => {
				hits.push({
					beat: i * 2,
					notes: [n, n * 2],
					dur: 2.1,
					vel: .7
				});
			});
			return hits;
		}
		case "warpath": {
			const hits = [];
			for (let b = 0; b < 8; b += .5) hits.push({
				beat: b,
				notes: E5,
				dur: .22,
				mute: true,
				vel: .9
			});
			hits.push({
				beat: 8,
				notes: G5,
				dur: 1.6,
				vel: .95
			});
			hits.push({
				beat: 10,
				notes: Fs5,
				dur: .9,
				vel: .9
			});
			hits.push({
				beat: 11,
				notes: E5,
				dur: 1.4,
				vel: .95
			});
			for (let b = 12; b < 16; b += .5) hits.push({
				beat: b,
				notes: C5,
				dur: .22,
				mute: true,
				vel: .85
			});
			return hits;
		}
		case "drone": return [
			{
				beat: 0,
				notes: E5,
				dur: 7.5,
				vel: .62
			},
			{
				beat: .4,
				notes: Bb5,
				dur: 7.2,
				vel: .48
			},
			{
				beat: 8,
				notes: A5,
				dur: 3.8,
				vel: .58
			},
			{
				beat: 8.3,
				notes: E5,
				dur: 3.6,
				vel: .5
			},
			{
				beat: 12,
				notes: D5,
				dur: 3.8,
				vel: .6
			},
			{
				beat: 12.3,
				notes: Bb5,
				dur: 3.6,
				vel: .45
			}
		];
	}
}
function playNote(ctx, dest, freq, when, dur, vel, mute) {
	const o1 = ctx.createOscillator();
	const g = ctx.createGain();
	const f = ctx.createBiquadFilter();
	o1.type = "sawtooth";
	o1.frequency.value = freq;
	f.type = "lowpass";
	f.Q.value = .85;
	const peak = mute ? 520 : 2400;
	f.frequency.setValueAtTime(Math.min(peak * 2.2, 6e3), when);
	f.frequency.exponentialRampToValueAtTime(peak, when + .07);
	const peakG = vel * (mute ? .12 : .22);
	g.gain.setValueAtTime(peakG * .7, when);
	g.gain.exponentialRampToValueAtTime(peakG, when + .002);
	g.gain.exponentialRampToValueAtTime(peakG * (mute ? .18 : .45), when + Math.min(.08, dur * .3));
	g.gain.exponentialRampToValueAtTime(1e-4, when + dur);
	o1.connect(f);
	f.connect(g);
	g.connect(dest);
	o1.start(when);
	o1.stop(when + dur + .02);
}
var RiffPlayer = class {
	bus = null;
	token = 0;
	loopTimer = null;
	playing = null;
	stop(ctx) {
		this.token += 1;
		this.playing = null;
		if (this.loopTimer != null) {
			window.clearTimeout(this.loopTimer);
			this.loopTimer = null;
		}
		if (this.bus) {
			const t = ctx.currentTime;
			this.bus.gain.cancelScheduledValues(t);
			this.bus.gain.setTargetAtTime(0, t, .02);
			const old = this.bus;
			window.setTimeout(() => {
				try {
					old.disconnect();
				} catch {}
			}, 80);
			this.bus = null;
		}
	}
	start(ctx, dest, id, loop = true) {
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
				const vel = (hit.vel ?? .8) / Math.sqrt(hit.notes.length);
				for (const n of hit.notes) playNote(ctx, bus, n, when, hit.dur * beat, vel, Boolean(hit.mute));
			}
			const totalMs = def.bars * 4 * beat * 1e3 + 40;
			if (loop) this.loopTimer = window.setTimeout(() => {
				if (this.token !== my) return;
				if (this.bus) try {
					this.bus.disconnect();
				} catch {}
				run();
			}, totalMs);
		};
		run();
	}
};
var riffPlayer = new RiffPlayer();
var DEFAULT_PARAMS = {
	gain: .64,
	cold: 24 / 27,
	grim: 25 / 30,
	level: .8,
	gate: .375,
	gateRelease: (25 / 195) ** 2,
	diode: 0,
	cab: 1,
	trim: .5,
	lofi: 0,
	engaged: true
};
function fmtGain(n) {
	return `${(50 * Math.pow(n, .5)).toFixed(1)} dB`;
}
function fmtCold(n) {
	const db = -12 + n * 27;
	return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
function fmtGrim(n) {
	const db = -15 + n * 30;
	return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
function fmtGate(n) {
	return `${(-100 + n * 80).toFixed(0)} dB`;
}
function fmtRelease(n) {
	return `${(5 + 195 * Math.pow(n, .5)).toFixed(0)} ms`;
}
function fmtLevel(n) {
	const db = -24 + n * 30;
	return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
function fmtTrim(n) {
	const db = -18 + n * 36;
	return `${db > 0 ? "+" : ""}${db.toFixed(1)} dB`;
}
function cn(...inputs) {
	return twMerge(clsx(inputs));
}
/**
* Minimum-latency Warlock path (native IIRs, meter is a tap, gate optional):
*   TRIM → [GATE] → HPF 320 → drive → diode → 1.2 kHz / Grim / Cold → cab LP → Level
*/
var GAIN_DB_MAX = 50;
var COLD_DB_MIN = -12;
var COLD_DB_SPAN = 27;
var GRIM_DB_MIN = -15;
var GRIM_DB_SPAN = 30;
var LEVEL_DB_MIN = -24;
var LEVEL_DB_SPAN = 30;
var SYM_CURVE = makeDiodeClip(0);
var ASYM_CURVE = makeDiodeClip(1);
function gainKnobToDb(n) {
	return GAIN_DB_MAX * Math.pow(Math.min(1, Math.max(0, n)), .5);
}
function dbToGain(db) {
	return Math.pow(10, db / 20);
}
function gateKnobToThresh(n) {
	const db = -100 + Math.min(1, Math.max(0, n)) * 80;
	return Math.pow(10, db / 20);
}
function gateReleaseToSec(n) {
	return (5 + 195 * Math.pow(Math.min(1, Math.max(0, n)), .5)) / 1e3;
}
function makeDiodeClip(asym, samples = 256) {
	const curve = new Float32Array(/* @__PURE__ */ new ArrayBuffer(samples * 4));
	const useAsym = asym > .5;
	const pos = useAsym ? .35 : .65;
	const neg = useAsym ? .75 : .65;
	for (let i = 0; i < samples; i++) {
		const x = i * 2 / samples - 1;
		curve[i] = Math.max(-neg, Math.min(pos, x));
	}
	return curve;
}
function snap(param, value, t) {
	param.cancelScheduledValues(t);
	param.setValueAtTime(value, t);
}
var PedalEngine = class {
	ctx;
	input;
	analyser;
	preHp;
	preGain;
	diode;
	fixedPeak;
	grimFilter;
	coldFilter;
	cabLp;
	gate = null;
	gateThresh = null;
	gateRelease = null;
	out;
	bypassGain;
	sum;
	params = { ...DEFAULT_PARAMS };
	lastDiode = -1;
	wetOnline = true;
	dryOnline = false;
	constructor(ctx) {
		this.ctx = ctx;
		this.input = ctx.createGain();
		this.input.gain.value = 1;
		this.input.channelCount = 1;
		this.input.channelCountMode = "explicit";
		this.preHp = ctx.createBiquadFilter();
		this.preHp.type = "highpass";
		this.preHp.frequency.value = 320;
		this.preHp.Q.value = .707;
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
		this.cabLp.Q.value = .7;
		this.out = ctx.createGain();
		this.bypassGain = ctx.createGain();
		this.bypassGain.gain.value = 1;
		this.sum = ctx.createGain();
		this.analyser = ctx.createAnalyser();
		this.analyser.fftSize = 256;
		this.analyser.smoothingTimeConstant = .5;
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
	installGate(node) {
		if (this.gate) return;
		this.gate = node;
		this.gateThresh = node.parameters.get("threshold") ?? null;
		this.gateRelease = node.parameters.get("release") ?? null;
		try {
			this.input.disconnect(this.preHp);
		} catch {}
		this.input.connect(node);
		node.connect(this.preHp);
		this.apply(this.params);
	}
	apply(next) {
		this.params = next;
		const t = this.ctx.currentTime;
		const drive = dbToGain(gainKnobToDb(next.gain));
		const grimDb = GRIM_DB_MIN + next.grim * GRIM_DB_SPAN;
		const coldDb = COLD_DB_MIN + next.cold * COLD_DB_SPAN;
		const cabOn = next.cab >= .5;
		const lp = next.lofi > .5 ? 4200 : cabOn ? 5600 : 18e3;
		snap(this.input.gain, dbToGain(-18 + next.trim * 36), t);
		snap(this.preGain.gain, drive, t);
		snap(this.grimFilter.gain, grimDb, t);
		snap(this.coldFilter.gain, coldDb, t);
		snap(this.cabLp.frequency, lp, t);
		if (next.diode !== this.lastDiode) {
			this.diode.curve = next.diode > .5 ? ASYM_CURVE : SYM_CURVE;
			this.lastDiode = next.diode;
		}
		this.gateThresh?.setValueAtTime(gateKnobToThresh(next.gate), t);
		this.gateRelease?.setValueAtTime(gateReleaseToSec(next.gateRelease), t);
		const levelLin = dbToGain(LEVEL_DB_MIN + next.level * LEVEL_DB_SPAN);
		snap(this.out.gain, next.engaged ? levelLin : 0, t);
		this.route(next.engaged);
	}
	route(engaged) {
		if (engaged && !this.wetOnline) {
			this.out.connect(this.sum);
			this.wetOnline = true;
		} else if (!engaged && this.wetOnline) {
			try {
				this.out.disconnect(this.sum);
			} catch {}
			this.wetOnline = false;
		}
		if (!engaged && !this.dryOnline) {
			this.input.connect(this.bypassGain);
			this.bypassGain.connect(this.sum);
			this.dryOnline = true;
		} else if (engaged && this.dryOnline) {
			try {
				this.bypassGain.disconnect();
			} catch {}
			try {
				this.input.disconnect(this.bypassGain);
			} catch {}
			this.dryOnline = false;
		}
	}
	click() {
		const t = this.ctx.currentTime;
		const o = this.ctx.createOscillator();
		const g = this.ctx.createGain();
		o.type = "square";
		o.frequency.value = 90;
		g.gain.setValueAtTime(.12, t);
		g.gain.exponentialRampToValueAtTime(1e-4, t + .04);
		o.connect(g);
		g.connect(this.ctx.destination);
		o.start(t);
		o.stop(t + .05);
	}
};
var singleton = null;
function getEngine() {
	return singleton;
}
async function attachGate(ctx, engine) {
	try {
		await ctx.audioWorklet.addModule("/worklets/noise-gate.js?v=7");
		engine.installGate(new AudioWorkletNode(ctx, "noise-gate"));
	} catch (err) {
		console.warn("Noise gate worklet unavailable", err);
	}
}
async function bootEngine() {
	if (singleton) {
		if (singleton.ctx.state !== "running") await singleton.ctx.resume();
		return singleton;
	}
	const ctx = new AudioContext({ latencyHint: .003 });
	singleton = new PedalEngine(ctx);
	const ready = ctx.state === "running" ? Promise.resolve() : ctx.resume();
	attachGate(ctx, singleton);
	await ready;
	return singleton;
}
var USER_PRESETS_KEY = "warlock-user-presets";
function snapshotParams(state) {
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
		lofi: state.lofi
	};
}
function readUserPresets() {
	if (typeof window === "undefined") return [];
	try {
		const raw = window.localStorage.getItem(USER_PRESETS_KEY);
		if (!raw) return [];
		const parsed = JSON.parse(raw);
		return Array.isArray(parsed) ? parsed : [];
	} catch {
		return [];
	}
}
function writeUserPresets(list) {
	if (typeof window === "undefined") return;
	window.localStorage.setItem(USER_PRESETS_KEY, JSON.stringify(list));
}
var micStream = null;
var micNode = null;
var fileNode = null;
var fileGain = null;
var usePedal = create((set, get) => ({
	...DEFAULT_PARAMS,
	awake: false,
	presetId: PRESETS[0].id,
	source: "idle",
	activeRiff: null,
	micError: null,
	fileName: null,
	userPresets: readUserPresets(),
	wake: async () => {
		(await bootEngine()).apply(get());
		set({ awake: true });
	},
	setParam: (key, value) => {
		set({ [key]: value });
		const engine = getEngine();
		if (engine) engine.apply(get());
	},
	toggleEngage: async () => {
		const wasAsleep = !get().awake;
		if (wasAsleep) await get().wake();
		const engine = getEngine();
		engine?.click();
		set({ engaged: wasAsleep ? true : !get().engaged });
		if (engine) engine.apply(get());
	},
	loadPreset: (id) => {
		const preset = PRESETS.find((p) => p.id === id);
		if (!preset) return;
		set({
			...preset.params,
			presetId: id
		});
		const engine = getEngine();
		if (engine) engine.apply(get());
	},
	loadUserPreset: (id) => {
		const preset = get().userPresets.find((p) => p.id === id);
		if (!preset) return;
		set({
			...preset.params,
			presetId: id
		});
		const engine = getEngine();
		if (engine) engine.apply(get());
	},
	saveUserPreset: (name) => {
		const trimmed = name.trim().slice(0, 32);
		if (!trimmed) return;
		const next = {
			id: `user-${Date.now()}`,
			name: trimmed,
			savedAt: Date.now(),
			params: snapshotParams(get())
		};
		const list = [next, ...get().userPresets].slice(0, 12);
		writeUserPresets(list);
		set({
			userPresets: list,
			presetId: next.id
		});
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
		set({
			source: "riff",
			activeRiff: id,
			micError: null
		});
	},
	stopRiff: () => {
		const engine = getEngine();
		if (engine) riffPlayer.stop(engine.ctx);
		set({
			activeRiff: null,
			source: get().source === "riff" ? "idle" : get().source
		});
	},
	startMic: async () => {
		if (!get().awake) await get().wake();
		get().stopRiff();
		get().stopFile();
		try {
			micStream = await navigator.mediaDevices.getUserMedia({ audio: {
				echoCancellation: false,
				noiseSuppression: false,
				autoGainControl: false,
				channelCount: 1,
				latency: 0
			} });
			const engine = getEngine();
			if (!engine) return;
			micNode = engine.ctx.createMediaStreamSource(micStream);
			micNode.connect(engine.input);
			if (!get().engaged) {
				set({ engaged: true });
				engine.apply(get());
			}
			set({
				source: "mic",
				micError: null,
				activeRiff: null
			});
		} catch {
			set({
				micError: "Microphone blocked — allow access, or play a riff instead.",
				source: "idle"
			});
		}
	},
	stopMic: () => {
		if (micNode) {
			try {
				micNode.disconnect();
			} catch {}
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
			fileGain.gain.value = .9;
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
			set({
				source: "file",
				fileName: file.name,
				activeRiff: null,
				micError: null
			});
		} catch {
			set({
				micError: "Couldn’t decode that file. Use WAV, MP3, OGG, or FLAC.",
				source: get().source === "file" ? "idle" : get().source
			});
		}
	},
	stopFile: () => {
		if (fileNode) {
			try {
				fileNode.stop();
				fileNode.disconnect();
			} catch {}
			fileNode = null;
		}
		if (fileGain) {
			try {
				fileGain.disconnect();
			} catch {}
			fileGain = null;
		}
		if (get().source === "file") set({
			source: "idle",
			fileName: null
		});
	}
}));
function Knob({ label, hint, readout, value, onChange, defaultValue = .5, compact = false, lit = true, led = "blood" }) {
	const drag = (0, import_react.useRef)(null);
	const angle = -135 + Math.min(1, Math.max(0, value)) * 270;
	const lampOn = lit && value > .02;
	const setFromPointer = (clientY) => {
		const start = drag.current;
		if (!start) return;
		onChange(Math.min(1, Math.max(0, start.v + (start.y - clientY) / 132)));
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex w-full max-w-24 flex-col items-center gap-1.5",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "knob-well",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "button",
					"aria-label": `${label} ${readout ?? `${Math.round(value * 100)} percent`}`,
					"aria-valuemin": 0,
					"aria-valuemax": 100,
					"aria-valuenow": Math.round(value * 100),
					role: "slider",
					className: cn("relative focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-frost/60", compact ? "size-14" : "size-16 sm:size-[4.5rem]"),
					style: { touchAction: "none" },
					onPointerDown: (e) => {
						e.preventDefault();
						e.currentTarget.setPointerCapture(e.pointerId);
						drag.current = {
							y: e.clientY,
							v: value
						};
					},
					onPointerMove: (e) => {
						if (drag.current) setFromPointer(e.clientY);
					},
					onPointerUp: () => {
						drag.current = null;
					},
					onPointerCancel: () => {
						drag.current = null;
					},
					onDoubleClick: () => onChange(defaultValue),
					onKeyDown: (e) => {
						const step = e.shiftKey ? .1 : .04;
						if (e.key === "ArrowUp" || e.key === "ArrowRight") {
							e.preventDefault();
							onChange(Math.min(1, value + step));
						}
						if (e.key === "ArrowDown" || e.key === "ArrowLeft") {
							e.preventDefault();
							onChange(Math.max(0, value - step));
						}
						if (e.key === "Home") onChange(0);
						if (e.key === "End") onChange(1);
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "knob-photo pointer-events-none absolute inset-0",
						style: { transform: `rotate(${angle}deg)` },
						"aria-hidden": true
					})
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: cn("led-lamp size-2 rounded-full", lampOn ? led === "green" ? "led-green" : "led-on" : "led-off"),
				"aria-hidden": true
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
				className: "flex flex-col items-center gap-0.5",
				children: [
					readout ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "knob-readout font-display text-[0.62rem] tracking-wide text-bone",
						children: readout
					}) : null,
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "font-display text-[0.62rem] tracking-[0.22em] text-bone-dim uppercase",
						children: label
					}),
					hint ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "font-body text-[0.6rem] text-frost/70 italic",
						children: hint
					}) : null
				]
			})
		]
	});
}
function PedalLcd({ title, subtitle, engaged }) {
	const lineRef = (0, import_react.useRef)(null);
	(0, import_react.useEffect)(() => {
		const line = lineRef.current;
		if (!line) return;
		const wave = /* @__PURE__ */ new Uint8Array(256);
		const w = 280;
		const h = 56;
		let raf = 0;
		const draw = () => {
			const engine = getEngine();
			const pts = [];
			if (engine && engaged) {
				engine.analyser.getByteTimeDomainData(wave);
				const step = Math.max(1, Math.floor(wave.length / 70));
				let x = 0;
				for (let i = 0; i < wave.length; i += step) {
					const y = wave[i] / 255 * h;
					pts.push(`${x.toFixed(1)},${y.toFixed(1)}`);
					x += w / (wave.length / step);
				}
			} else pts.push(`0,${h / 2}`, `${w},${h / 2}`);
			line.setAttribute("points", pts.join(" "));
			raf = requestAnimationFrame(draw);
		};
		raf = requestAnimationFrame(draw);
		return () => cancelAnimationFrame(raf);
	}, [engaged]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "lcd-bezel mx-auto w-full max-w-[15.5rem] overflow-hidden rounded-sm",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "lcd-screen relative h-14 px-2 py-1.5",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("svg", {
				viewBox: "0 0 280 56",
				className: "absolute inset-0 size-full opacity-60",
				preserveAspectRatio: "none",
				"aria-hidden": true,
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("polyline", {
					ref: lineRef,
					fill: "none",
					stroke: "rgb(231, 244, 246)",
					strokeWidth: "1.4",
					points: "0,28 280,28"
				})
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "relative z-10 flex h-full flex-col justify-center",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-display text-xs tracking-[0.22em] text-frost-bright uppercase",
					children: title
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-body text-xs tracking-wide text-frost/80 italic",
					children: subtitle
				})]
			})]
		})
	});
}
var SEGMENTS = 8;
function LedMeter({ engaged }) {
	const refs = (0, import_react.useRef)([]);
	(0, import_react.useEffect)(() => {
		const wave = /* @__PURE__ */ new Uint8Array(256);
		let timer = 0;
		const draw = () => {
			const engine = getEngine();
			let peak = 0;
			if (engine && engaged) {
				engine.analyser.getByteTimeDomainData(wave);
				for (let i = 0; i < wave.length; i += 4) peak = Math.max(peak, Math.abs(wave[i] - 128) / 128);
			}
			const lit = Math.round(peak * SEGMENTS);
			refs.current.forEach((el, i) => {
				if (!el) return;
				el.classList.toggle("on", i < lit);
			});
		};
		timer = window.setInterval(draw, 80);
		draw();
		return () => window.clearInterval(timer);
	}, [engaged]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "led-meter",
		"aria-hidden": true,
		children: Array.from({ length: SEGMENTS }).map((_, i) => {
			const idx = 7 - i;
			return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				ref: (el) => {
					refs.current[idx] = el;
				},
				className: cn("led-seg", idx >= 6 ? "red" : idx >= 4 ? "amber" : "green")
			}, idx);
		})
	});
}
function StompSwitch({ engaged, awake, onToggle }) {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex flex-col items-center gap-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: cn("led-lamp size-3 rounded-full transition-[box-shadow,background-color,opacity] duration-(--motion-fast)", engaged && awake ? "led-on" : "led-off"),
			"aria-hidden": true
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "stomp-well",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "button",
				"aria-pressed": engaged && awake,
				"aria-label": engaged && awake ? "Bypass pedal" : "Engage pedal",
				onClick: onToggle,
				className: cn("stomp-cap relative size-16 rounded-full sm:size-[4.75rem]", "focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-frost/60", "active:translate-y-0.5"),
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "sr-only",
					children: engaged && awake ? "On" : "Off"
				})
			})
		})]
	});
}
function Screw({ className }) {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
		className: cn("screw", className),
		"aria-hidden": true,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "screw-slot" })
	});
}
function MiniSwitch({ name, on, onToggle }) {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
		type: "button",
		"aria-pressed": on,
		"aria-label": name,
		onClick: onToggle,
		className: "flex flex-col items-center gap-1.5 focus-visible:ring-2 focus-visible:ring-frost/60",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: cn("rocker", on && "rocker-on"),
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "rocker-knob" })
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: "font-display text-[0.52rem] tracking-[0.14em] text-bone-dim uppercase",
			children: name
		})]
	});
}
function PedalEnclosure() {
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
	const lcdSub = !awake ? "Press the stomp" : source === "riff" && activeRiff ? `Riff · ${RIFFS.find((r) => r.id === activeRiff)?.name ?? activeRiff}` : source === "mic" ? "Input · microphone" : source === "file" ? `Input · ${fileName || "file"}` : preset.epithet;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "pedal-stage relative mx-auto w-full max-w-[28rem]",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "pedal-jacks",
				"aria-hidden": true,
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "jack-label",
						children: "In"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "jack" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "jack jack-dc" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "jack" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "jack-label",
						children: "Out"
					})
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "pedal-shell relative w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "scratch",
						"aria-hidden": true
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Screw, { className: "absolute top-3 left-3 z-20" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Screw, { className: "absolute top-3 right-3 z-20" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Screw, { className: "absolute bottom-3 left-3 z-20" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Screw, { className: "absolute right-3 bottom-3 z-20" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "relative z-10 px-4 pt-5 pb-5 sm:px-6",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "nameplate mx-auto",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "flex items-center justify-center gap-3",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "invert-cross",
											"aria-hidden": true
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h1", {
											className: "warlock-title font-deco text-center text-[1.85rem] leading-none sm:text-[2.15rem]",
											children: "Warlock"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "invert-cross",
											"aria-hidden": true
										})
									]
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
									className: "font-display mt-1.5 text-center text-[0.58rem] tracking-[0.38em] text-frost uppercase",
									children: "Chainsaw Distortion"
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mt-5 grid grid-cols-3 justify-items-center gap-x-2 gap-y-4",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Gain",
										readout: fmtGain(gain),
										value: gain,
										defaultValue: DEFAULT_PARAMS.gain,
										onChange: (v) => setParam("gain", v),
										lit: engaged && awake
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Cold",
										readout: fmtCold(cold),
										value: cold,
										defaultValue: DEFAULT_PARAMS.cold,
										onChange: (v) => setParam("cold", v),
										lit: engaged && awake
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Grim",
										readout: fmtGrim(grim),
										value: grim,
										defaultValue: DEFAULT_PARAMS.grim,
										onChange: (v) => setParam("grim", v),
										lit: engaged && awake
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Gate",
										readout: fmtGate(gate),
										value: gate,
										defaultValue: DEFAULT_PARAMS.gate,
										onChange: (v) => setParam("gate", v),
										lit: engaged && awake
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Release",
										readout: fmtRelease(gateRelease),
										value: gateRelease,
										defaultValue: DEFAULT_PARAMS.gateRelease,
										onChange: (v) => setParam("gateRelease", v),
										lit: engaged && awake
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Level",
										readout: fmtLevel(level),
										value: level,
										defaultValue: DEFAULT_PARAMS.level,
										onChange: (v) => setParam("level", v),
										lit: engaged && awake,
										led: "green"
									})
								]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mt-5 flex items-center justify-center gap-6 sm:gap-8",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MiniSwitch, {
										name: "Asym",
										on: diode >= .5,
										onToggle: () => setParam("diode", diode >= .5 ? 0 : 1)
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MiniSwitch, {
										name: "Cab IR",
										on: cab >= .5,
										onToggle: () => setParam("cab", cab >= .5 ? 0 : 1)
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MiniSwitch, {
										name: "Lo-Fi",
										on: lofi >= .5,
										onToggle: () => setParam("lofi", lofi >= .5 ? 0 : 1)
									})
								]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mt-5 flex items-end gap-3",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Knob, {
										label: "Trim",
										readout: fmtTrim(trim),
										value: trim,
										defaultValue: DEFAULT_PARAMS.trim,
										onChange: (v) => setParam("trim", v),
										lit: engaged && awake,
										compact: true
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(LedMeter, { engaged: engaged && awake }),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
										className: "min-w-0 flex-1",
										children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(PedalLcd, {
											title: lcdTitle,
											subtitle: lcdSub,
											engaged: engaged && awake
										})
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(StompSwitch, {
										engaged,
										awake,
										onToggle: () => void toggleEngage()
									})
								]
							})
						]
					})
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "pedal-feet",
				"aria-hidden": true,
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {})]
			})
		]
	});
}
var buttonVariants = cva("inline-flex items-center justify-center gap-2 font-display tracking-[0.16em] uppercase text-xs font-medium transition-[transform,background-color,color,border-color,opacity] duration-(--motion-quick) ease-(--ease-out) focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-frost/50 disabled:pointer-events-none disabled:opacity-40", {
	variants: {
		variant: {
			primary: "bg-blood text-bone hover:bg-blood/80 active:not-disabled:scale-[0.96]",
			ghost: "border border-line text-bone hover:border-frost/50 hover:text-frost-bright active:not-disabled:scale-[0.96]",
			subtle: "bg-iron text-bone hover:bg-slate-metal active:not-disabled:scale-[0.96]"
		},
		size: {
			sm: "h-10 px-3",
			md: "h-11 px-4",
			lg: "h-12 px-5"
		}
	},
	defaultVariants: {
		variant: "ghost",
		size: "md"
	}
});
var Button = (0, import_react.forwardRef)(({ className, variant, size, ...props }, ref) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
	ref,
	className: cn(buttonVariants({
		variant,
		size
	}), className),
	...props
}));
Button.displayName = "Button";
var BAR_COUNT = 24;
function Spectrum({ engaged }) {
	const barsRef = (0, import_react.useRef)(null);
	(0, import_react.useEffect)(() => {
		const group = barsRef.current;
		if (!group) return;
		const rects = Array.from(group.querySelectorAll("rect"));
		const freq = /* @__PURE__ */ new Uint8Array(128);
		let timer = 0;
		const draw = () => {
			const engine = getEngine();
			if (engine && engaged) {
				engine.analyser.getByteFrequencyData(freq);
				for (let i = 0; i < rects.length; i++) {
					const v = freq[Math.floor(i * 3.2)] / 255;
					const bh = Math.max(2, v * 56);
					rects[i].setAttribute("y", String(56 - bh));
					rects[i].setAttribute("height", String(bh));
				}
			} else for (const r of rects) {
				r.setAttribute("y", "54");
				r.setAttribute("height", "2");
			}
		};
		timer = window.setInterval(draw, 80);
		draw();
		return () => window.clearInterval(timer);
	}, [engaged]);
	const bw = 211 / BAR_COUNT;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("svg", {
		viewBox: "0 0 280 56",
		className: "h-12 w-full",
		"aria-hidden": true,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("g", {
			ref: barsRef,
			fill: "rgb(225, 29, 46)",
			children: Array.from({ length: BAR_COUNT }).map((_, i) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)("rect", {
				x: i * 11.791666666666666,
				y: 54,
				width: bw,
				height: 2
			}, i))
		})
	});
}
function StudioDock() {
	const fileRef = (0, import_react.useRef)(null);
	const [fileReady, setFileReady] = (0, import_react.useState)(false);
	const awake = usePedal((s) => s.awake);
	const engaged = usePedal((s) => s.engaged);
	const source = usePedal((s) => s.source);
	const activeRiff = usePedal((s) => s.activeRiff);
	const presetId = usePedal((s) => s.presetId);
	const micError = usePedal((s) => s.micError);
	const fileName = usePedal((s) => s.fileName);
	const playRiff = usePedal((s) => s.playRiff);
	const stopRiff = usePedal((s) => s.stopRiff);
	const startMic = usePedal((s) => s.startMic);
	const stopMic = usePedal((s) => s.stopMic);
	const loadFile = usePedal((s) => s.loadFile);
	const stopFile = usePedal((s) => s.stopFile);
	const loadPreset = usePedal((s) => s.loadPreset);
	const loadUserPreset = usePedal((s) => s.loadUserPreset);
	const saveUserPreset = usePedal((s) => s.saveUserPreset);
	const deleteUserPreset = usePedal((s) => s.deleteUserPreset);
	const userPresets = usePedal((s) => s.userPresets);
	const wake = usePedal((s) => s.wake);
	const live = awake && engaged;
	const [presetName, setPresetName] = (0, import_react.useState)("");
	(0, import_react.useEffect)(() => {
		setFileReady(true);
	}, []);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("aside", {
		className: "studio-panel flex w-full max-w-xl flex-col gap-5 lg:max-w-sm",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("section", { children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h2", {
					className: "font-display text-xs tracking-[0.28em] text-frost uppercase",
					children: "Play a riff"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-body mt-1 text-sm text-bone-dim",
					children: "Stomp to wake it. Send a wall of ice through the chainsaw."
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "mt-3 grid grid-cols-2 gap-2",
					children: RIFFS.map((riff) => {
						const on = activeRiff === riff.id;
						return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
							type: "button",
							onClick: () => {
								if (on) stopRiff();
								else playRiff(riff.id);
							},
							className: cn("flex min-h-12 flex-col items-start justify-center border px-3 py-2 text-left transition-colors duration-(--motion-quick)", on ? "border-blood bg-blood/15 text-bone" : "border-line text-bone hover:border-blood/50"),
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "font-display text-xs tracking-[0.16em] uppercase",
								children: riff.name
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "font-body text-xs text-bone-dim italic",
								children: on ? "Playing — tap to stop" : riff.epithet
							})]
						}, riff.id);
					})
				}),
				!awake ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Button, {
					className: "mt-3 w-full",
					variant: "primary",
					onClick: () => void wake(),
					children: "Power the pedal"
				}) : null
			] }),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("section", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h2", {
				className: "font-display text-xs tracking-[0.28em] text-frost uppercase",
				children: "Voicings"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "mt-3 grid grid-cols-2 gap-1.5",
				children: PRESETS.map((preset) => {
					const on = presetId === preset.id;
					return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
						type: "button",
						onClick: () => loadPreset(preset.id),
						className: cn("min-h-11 border px-2.5 py-2 text-left transition-colors duration-(--motion-quick)", on ? "border-blood bg-blood text-bone" : "border-line text-bone hover:border-frost/40"),
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "font-display block text-xs tracking-[0.12em] uppercase",
							children: preset.name
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: cn("font-body block text-xs italic", on ? "text-bone/70" : "text-bone-dim"),
							children: preset.epithet
						})]
					}, preset.id);
				})
			})] }),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("section", { children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h2", {
					className: "font-display text-xs tracking-[0.28em] text-frost uppercase",
					children: "User banks"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
					className: "mt-3 flex gap-2",
					onSubmit: (e) => {
						e.preventDefault();
						saveUserPreset(presetName || "Unnamed");
						setPresetName("");
					},
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
						value: presetName,
						onChange: (e) => setPresetName(e.target.value),
						maxLength: 32,
						placeholder: "Name this voicing",
						className: "min-w-0 flex-1 border border-line bg-void/50 px-2 py-1.5 font-display text-xs tracking-wide text-bone placeholder:text-bone-dim"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Button, {
						type: "submit",
						size: "sm",
						variant: "primary",
						children: "Save"
					})]
				}),
				userPresets.length ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "mt-2 grid grid-cols-1 gap-1.5",
					children: userPresets.map((preset) => {
						const on = presetId === preset.id;
						return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "flex gap-1.5",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
								type: "button",
								onClick: () => loadUserPreset(preset.id),
								className: cn("min-h-10 flex-1 border px-2.5 py-1.5 text-left", on ? "border-blood bg-blood/20 text-bone" : "border-line text-bone"),
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
									className: "font-display block text-xs tracking-[0.12em] uppercase",
									children: preset.name
								})
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
								type: "button",
								"aria-label": `Delete ${preset.name}`,
								onClick: () => deleteUserPreset(preset.id),
								className: "border border-line px-2 font-display text-[0.6rem] tracking-wide text-bone-dim uppercase hover:text-blood",
								children: "X"
							})]
						}, preset.id);
					})
				}) : /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-body mt-2 text-xs text-bone-dim italic",
					children: "Save a bank after you carve a tone."
				})
			] }),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("section", { children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h2", {
					className: "font-display text-xs tracking-[0.28em] text-frost uppercase",
					children: "External signal"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "mt-3 grid grid-cols-2 gap-2",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Button, {
						variant: source === "mic" ? "primary" : "ghost",
						size: "sm",
						onClick: () => {
							if (source === "mic") stopMic();
							else startMic();
						},
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Mic, { className: "size-3.5" }), source === "mic" ? "Mute mic" : "Microphone"]
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Button, {
						variant: source === "file" ? "primary" : "ghost",
						size: "sm",
						onClick: () => {
							if (source === "file") stopFile();
							else fileRef.current?.click();
						},
						children: [source === "file" ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Square, { className: "size-3.5" }) : /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Upload, { className: "size-3.5" }), source === "file" ? "Eject" : "Load file"]
					})]
				}),
				fileReady ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
					ref: fileRef,
					type: "file",
					accept: "audio/*,.wav,.mp3,.ogg,.flac,.m4a",
					className: "sr-only",
					onChange: (e) => {
						const file = e.target.files?.[0];
						if (file) loadFile(file);
						e.target.value = "";
					}
				}) : null,
				fileName ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-body mt-2 truncate text-xs text-frost",
					children: fileName
				}) : null,
				micError ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-body mt-2 text-xs text-blood",
					children: micError
				}) : null,
				source === "mic" ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
					className: "font-body mt-2 text-xs text-bone-dim",
					children: "Use headphones to avoid feedback."
				}) : null
			] }),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("section", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h2", {
				className: "font-display text-xs tracking-[0.28em] text-frost uppercase",
				children: "Output"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "mt-2 border border-line bg-void/50 px-2 py-1",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Spectrum, { engaged: live })
			})] })
		]
	});
}
function Home() {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("main", {
		className: "page-bg relative min-h-dvh overflow-x-hidden",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "relative z-10 mx-auto flex max-w-6xl flex-col px-4 pt-5 pb-8 sm:px-6 lg:px-8",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("header", {
					className: "mb-5 text-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
						className: "font-display text-xs tracking-[0.42em] text-blood uppercase",
						children: "Nihil Audio · VST3"
					})
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex flex-1 flex-col items-center gap-6 lg:flex-row lg:items-start lg:justify-center lg:gap-10",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(PedalEnclosure, {}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StudioDock, {})]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("footer", {
					className: "mt-8 border-t border-line pt-4 text-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
						className: "font-body text-xs tracking-wide text-bone-dim",
						children: [
							"Nihil Audio · Warlock Chainsaw Distortion VST3 1.1.0 · x64. Processing stays in your browser. Drag knobs, stomp to engage, double-click a knob to reset.",
							" ",
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
								href: "/Contents/Resources/legal.txt",
								className: "text-frost underline-offset-2 hover:underline",
								children: "Legal"
							})
						]
					})
				})
			]
		})
	});
}
//#endregion
export { Home as component };

# VST3 standards — how Warlock maps

Steinberg VST 3.7.x. JUCE 7 implements the SDK; this file is the checklist we actually obey.

## Bundle (Windows)

A VST3 is a **folder** named `Warlock.vst3`, not a lone DLL.

```
Warlock.vst3/
├── Contents/
│   ├── x86_64-win/
│   │   └── Warlock.vst3      ← PE DLL, SAME name as the folder
│   └── Resources/
│       ├── moduleinfo.json   ← SDK 3.7.8+ lives HERE (not Contents/)
│       ├── CabinetIR.wav
│       └── legal.txt
├── desktop.ini               ← optional Explorer icon
└── Plugin.ico                ← optional
```

| Rule | Warlock |
|---|---|
| Folder + inner DLL share the name `Warlock.vst3` | JUCE artefact does this |
| Install to `C:\Program Files\Common Files\VST3\` | installer.iss `{commoncf64}\VST3\Warlock.vst3` |
| No vendor subfolder | hosts only scan the VST3 root |
| 32-bit tree `Program Files (x86)\Common Files\VST3` | not used (x64 only) |
| `x86_64-win` vs `arm64ec-win` | we ship x86_64-win; ARM hosts use x64 emulation (`x64compatible`) |

Deprecated: single-file `.vst3` DLL at the VST3 root (pre-3.6.10). Cubase wants the bundle.

## moduleinfo.json (SDK 3.7.5, path fixed in 3.7.8)

Optional but hosts can list the plug-in **without loading the DLL**. Schema: `Name`, `Version`, `Factory Info` (Vendor, URL, E-Mail, Unicode flag), `Classes[]` with **real CID**, Category `Audio Module Class`, `Sub Categories`.

CID is a GUID JUCE derives from manufacturer `Nhil` + plugin `Wrck`. Do not invent one. After a Release build, copy JUCE’s generated manifest, or run Steinberg `moduleinfotool`. Template: `Resources/moduleinfo.example.json`.

## Processor vs controller

VST3 splits **component** (audio) and **edit controller** (UI). JUCE wraps both in `AudioProcessor` + `AudioProcessorEditor`. Do not touch the audio thread from `paint()` / sliders except through APVTS.

## Parameters

- Stable string IDs (`gain`, `cold`, …). Renaming = broken sessions.
- Units in the parameter (dB, ms) so hosts can show them.
- **Bypass** is a dedicated automatable bool with `kIsBypass`. Warlock exposes `bypass` via `getBypassParameter()` so Cubase/Live’s bypass is *our* dry path, latency-compensated.
- Sample-accurate automation is optional; Warlock reads k-rate once per block (pedal knobs).

## I/O

- `IAudioProcessor::setBusArrangements` — we allow **mono→mono** and **stereo→stereo** only (`isBusesLayoutSupported`). No sidechain, no 5.1.
- Category: `Fx|Distortion` (`VST3_CATEGORIES Fx Distortion`).

## Timing the host must know

| Call | Why | Warlock |
|---|---|---|
| `getLatencySamples` | PDC. 4× half-band OS is not zero-latency | `setLatencySamples(oversampler->getLatencyInSamples())` in `prepareToPlay` |
| `getTailLengthSeconds` | how long to keep processing after stop | ~6 ms when Cab IR on, else 0 |
| `processContext` tempo/ppq | sync FX | unused (no time-based FX) |

## Threading

- `processBlock`: no alloc, no lock, no file, no UI.
- Coefficient rebuilds only when Cold/Grim/Lo-Fi change (`updateToneCoeffs`).
- IR load in `prepareToPlay`, not in process.

## State

VST3 `IEditController::setState` / `getState` ← JUCE `getStateInformation` XML of APVTS. Chunk must round-trip in pluginval.

## Validation

```
pluginval --strictness-level 5 --validate-in-process Warlock.vst3
```

pluginval checks layouts, bypass, state, editor open/close, denormals. Ship only if it is green.

Skins (chassis, knob, stomp) and CabinetIR.wav are compiled into the VST3 via `juce_add_binary_data`. The host never loads images from disk.

## What we do not claim

- AAX / AU / VST2
- `arm64ec-win` native binary
- Sidechain
- MIDI
- Ambisonics / 3D speaker sets

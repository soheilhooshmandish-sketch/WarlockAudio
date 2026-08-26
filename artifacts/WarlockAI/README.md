# Warlock AI

Production-oriented guitar and instrument distortion plugin with a modular DSP engine and offline tone analysis.

**Black Forge Audio**  
Formats: VST3, Standalone, AU (macOS)  
Language: C++20  
Framework: JUCE 8.0.15

## Signal chain

Input Gain → Noise Gate → Compressor → Distortion (oversampled) → Amp → Cabinet → EQ → Harmonizer → Delay → Reverb → Stereo Processor → Limiter → Output Gain

Amp is a nonlinear preamp plus tone stack (Clean through Djent). Cabinet uses a speaker coloration filter and a synthetic IR with a WAV-load path for future impulses. Harmonizer is a dual-voice grain pitch shifter with fixed latency.

## Distortion modes

Tube, Vintage, Modern, Metal, Djent, Extreme, Adaptive AI

Adaptive AI reads offline feature hints (brightness, density, dynamics) written by the analysis thread. It never runs model inference on the audio thread.

## Project layout

```
WarlockAI/
├── CMakeLists.txt
├── README.md
├── Source/
│   ├── PluginProcessor.*
│   ├── PluginEditor.*
│   ├── Core/          parameters, presets
│   ├── Engine/        signal chain
│   ├── DSP/           realtime processors
│   ├── AI/            offline analysis
│   ├── UI/            BLACK FORGE look and feel
│   └── Tests/         DspSmoke mono/stereo harness
└── Resources/
```

## Build

CMake 3.22 or newer is required. JUCE 8.0.15 is fetched automatically.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target WarlockAI
```

Windows (Visual Studio 2022):

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target WarlockAI
```

Targets produced:

- `WarlockAI_VST3`
- `WarlockAI_Standalone`
- `WarlockAI_AU` on Apple hosts
- `WarlockAIDspSmoke` — headless mono/stereo DSP check
- `WarlockAITests` — offline AI feature, health, style, mix-fit, and adaptive-smoothing checks

Linux artefacts from this workspace:

- `bin/VST3/Warlock AI.vst3`
- `bin/Standalone/Warlock AI`

## Realtime constraints

- All DSP modules implement `prepare`, `reset`, `process`, and bypass.
- IIR coefficients are assigned from stack `ArrayCoefficients` — no heap allocation on the audio thread after `prepareToPlay`.
- Delay/reverb/cabinet scratch buffers are allocated in `prepare` and never resized in `process`.
- Parameter changes are smoothed to avoid zipper noise.
- Oversampling (off / 2× / 4×) wraps only the nonlinear distortion stage.
- Plugin state is stored and restored through `AudioProcessorValueTreeState`.
- Phase 1 parameter IDs are unchanged. Phase 2 adds unique IDs only.

## AI phases

| Phase | Status | Notes |
| --- | --- | --- |
| 1 | Implemented | RMS, peak, centroid, rolloff, ZCR, dynamic range, tone class |
| 2 | Implemented | Tone Health, Mix Fit, Auto Build Tone, amp/cab/harmony/delay/reverb/stereo/compressor modules |
| 3 | Implemented | Offline AIEngine, expanded features, Tone Designer (rule-based), Style Detector, Mix Fit preview/apply/undo, Adaptive AI smoothing. No network. Ready for a future ONNX model. |
| 4 | Placeholder | Tone Evolution, personalized recommendations |

Analyze Tone and Auto Build Tone copy the most recent captured audio into a background thread, then publish results to the editor. Mix Fit applies suggested parameter values on the message thread.

## GUI

Theme: **BLACK FORGE** — matte black panels, brushed-steel knobs, bronze structure, red distortion accents, blue/purple analysis accents.

- Simple mode: Input, Gain, Tone, Tight, Gate, Output, Analyze, Auto Build, Tone Health, Mix Fit, Apply, Undo, AI Dashboard
- Advanced mode: collapsible CORE / AMP / CAB / EQ / MOD / HARMONY / DELAY / REVERB / STEREO / LIMITER, bypasses, GR / delay / reverb / width meters, spectrum, Tone AI dashboard, Tone Health, Mix Fit, CPU and latency

The AI engine runs on a background thread. `processBlock` only writes a lock-free capture ring and reads Adaptive AI hints from atomics. No network, no ONNX Runtime dependency. `ONNXToneModel` is a header-only future hook.

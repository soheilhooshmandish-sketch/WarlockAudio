# Steinberg VST3 SDK — what Warlock actually uses

You do **not** clone [steinbergmedia/vst3sdk](https://github.com/steinbergmedia/vst3sdk) for this project.

JUCE 7.0.12 already vendors the VST3 interfaces under:

```
juce_audio_processors/format_types/VST3_SDK/
```

`FetchContent` pulls JUCE; the wrapper (`juce_audio_plugin_client`) implements the SDK so `AudioProcessor` / `AudioProcessorEditor` become a VST3 module. Pointing Projucer/CMake at an extra `VST3_SDK` path is how you get “dragons.” Leave it empty.

Official SDK (if you ever go native, no JUCE): `git clone --recursive https://github.com/steinbergmedia/vst3sdk.git` — current line is **3.8.x**, **MIT**. Older GPLv3 / Steinberg proprietary dual-license is retired on GitHub. JUCE 7.0.12 still ships the SDK version *it* was tested with; do not mix trees.

## SDK layout (Steinberg)

| Tree | Role |
|---|---|
| `pluginterfaces/` | COM-style APIs (`FUnknown`, `IPluginFactory`, `IComponent`, `IAudioProcessor`, `IEditController`) |
| `public.sdk/` | helpers, `moduleinfotool`, VST3 wrapper samples |
| `base/` | Steinberg C++ base |
| `cmake/` | `smtg_add_vst3plugin` |
| `vstgui4/` | Steinberg’s GUI (we use JUCE, not VSTGUI) |
| validator / editorhost | full zip from [steinberg.net/vst3sdk](https://www.steinberg.net/vst3sdk), not always in the git meta-repo |

## Runtime objects (what the host calls)

```
GetPluginFactory()
    └─ IPluginFactory::createInstance(CID)
           ├─ IComponent + IAudioProcessor     ← realtime
           └─ IEditController                  ← UI + parameters
                    ⇅  IConnectionPoint messages / IDataExchange
```

| SDK | JUCE (this repo) |
|---|---|
| `IPluginFactory` | generated; codes `Nhil` / `Wrck` |
| `IAudioProcessor::process` | `WarlockAudioProcessor::processBlock` |
| `IAudioProcessor::setProcessing` / `setupProcessing` | `prepareToPlay` / `releaseResources` |
| `setBusArrangements` | `isBusesLayoutSupported` (mono, stereo) |
| `IEditController` parameters | `apvts` + `SliderAttachment` |
| `kIsBypass` | `getBypassParameter()` → `bypass` |
| `IAudioProcessor::getLatencySamples` | `setLatencySamples(oversampler)` |
| `getTailSamples` | `getTailLengthSeconds()` |
| `IComponent::getState` / `setState` | `getStateInformation` XML |
| Units / program lists | unused (one FX, no banks) |
| MIDI event bus | none (`NEEDS_MIDI_* FALSE`) |
| VSTGUI | unused |

Threading (SDK workflow): **audio thread** only in `process`. UI thread owns the controller. Data from process → controller goes through a lock-free queue (`IDataExchange` / JUCE atomics). Warlock’s meter is `std::atomic<float> meterPeak`.

Strings in the SDK are UTF-16. JUCE converts `juce::String`. Factory Unicode flag must be on (it is).

## What we deliberately skip

- `smtg_add_vst3plugin` — CMake from Steinberg; we use `juce_add_plugin`
- VSTGUI snapshots via `smtg_add_vst3_snapshot` — we copy PNG ourselves (`PACKAGING.md`)
- Side-chain buses, 3D speaker sets, note expression, MIDI learn
- VST 2.x wrapper (license dead; `JUCE_VST3_CAN_REPLACE_VST2=0`)

## Validate with Steinberg’s tools

After `build.bat`:

```
pluginval --strictness-level 5 --validate-in-process Warlock.vst3
```

Optional: Steinberg **validator** / **editorhost** from the full SDK zip. Same bundle path: `Common Files\VST3\Warlock.vst3`.

## License footnote

Shipping a VST3 still requires following [VST 3 usage guidelines](https://steinbergmedia.github.io/vst3_dev_portal/) (trademark “VST”, no claiming Steinberg endorsement). SDK code MIT ≠ permission to misuse the VST logo. Keep `legal.txt` in the bundle.

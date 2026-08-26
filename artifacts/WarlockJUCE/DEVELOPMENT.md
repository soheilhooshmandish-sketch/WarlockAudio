# Warlock — VST3 Plugin Development

Windows x64 · JUCE 7.0.12 · VST 3.7 · C++17

VST3 spec mapping: [STANDARDS.md](STANDARDS.md). Bypass is `bypass` (`getBypassParameter`). Latency is reported from the oversampler after `prepareToPlay`.

```
Warlock/
├── CMakeLists.txt
├── CMakePresets.json
├── installer.iss
├── INSTALL.md
├── DEVELOPMENT.md          ← you are here
├── Source/
│   ├── PluginProcessor.h   parameters + DSP objects
│   ├── PluginProcessor.cpp processBlock (the sound)
│   ├── PluginEditor.h      UI widgets
│   └── PluginEditor.cpp    pedal skin (BinaryData images + 30 Hz LED)
└── Resources/              compiled INTO the VST3, not loaded from disk
    ├── chassis.jpg
    ├── knob.jpg
    ├── stomp.jpg
    ├── CabinetIR.wav
    └── legal.txt
```

## 1. One-time setup

| Tool | Why |
|---|---|
| Visual Studio 2022, Desktop C++ x64 | compiler |
| CMake ≥ 3.22 | `juce_add_plugin` |
| git | FetchContent pulls JUCE 7.0.12 |
| [Inno Setup 6.3+](https://jrsoftware.org/isdl.php) | installer only |
| [pluginval](https://github.com/Tracktion/pluginval/releases) | VST3 compliance |

VS Installer → **Desktop development with C++** + **Windows 10/11 SDK**.

## 2. Configure and build

Preset `vs2022-x64` writes a Visual Studio solution.

```
cmake --preset vs2022-x64
cmake --build --preset warlock-release
```

Or without presets:

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

`build.bat` does the same.

Artefact:

```
build\Warlock_artefacts\Release\VST3\Warlock.vst3\
  Contents\x86_64-win\Warlock.vst3
  Contents\Resources\
```

First configure clones JUCE (~2 min). Later builds are incremental.

Debug build: `cmake --build build --config Debug`  
Attach VS to your DAW, set breakpoints in `processBlock`.

## 3. Load it while developing

Do **not** fight the installer during DSP work.

1. In the DAW, add a custom VST3 path:  
   `…\Warlock\build\Warlock_artefacts\Release\VST3`
2. Or copy the bundle over the installed one  
   `C:\Program Files\Common Files\VST3\Warlock.vst3`  
   (admin, DAW closed).
3. Rescan. Close the editor window before rebuilding or the DLL stays locked.

Optional: in `CMakeLists.txt` set `COPY_PLUGIN_AFTER_BUILD TRUE` so JUCE copies into the user’s VST3 folder after every Release build.

## 4. Signal path (keep this order)

`PluginProcessor::processBlock`

1. Trim (dB → linear, per sample)
2. Gate — envelope follower, 1 ms attack, 5–200 ms release, 4:1 expander
3. `oversampler->processSamplesUp` (4×, half-band polyphase allpass)
4. HPF 320 Hz Q0.707
5. Drive 0–50 dB, range skew 0.5
6. Hard clip: SYM ±0.65 or ASYM +0.35 / −0.75
7. Peak 1200 Hz Q3 +14 dB → Grim 950 Hz Q2.2 → Cold 4100 Hz Q1.8
8. `processSamplesDown`
9. Lo-Fi LP 4.2 kHz if engaged, else 18 kHz
10. Cabinet convolution if Cab IR on (`BinaryData::CabinetIR_wav`)
11. Level −24…+6 dB

Do not allocate, lock, or `printf` on the audio thread. Coefficient rebuilds belong in `updateToneCoeffs()` (already gated on last Cold/Grim/Lo-Fi).

Oversampling factor is `2` stages = **4×**. Changing it changes latency; tell the host via `getLatencySamples()` if you add that.

## 5. Parameters (APVTS)

IDs are the automation names. **Never rename an existing ID** — sessions break. Add new IDs with a new `ParameterID (name, versionHint)`.

| ID | Type | Range | Default |
|---|---|---|---|
| `trim` | float | −18…+18 dB | 0 |
| `gain` | float | 0…50 dB, skew 0.5 | 40 |
| `cold` | float | −12…+15 dB | 12 |
| `grim` | float | −15…+15 dB | 10 |
| `gate` | float | −100…−20 dB | −70 |
| `gaterelease` | float | 5…200 ms, skew 0.5 | 30 |
| `diode` | bool | asym off/on | off |
| `cab` | bool | IR off/on | on |
| `lofi` | bool | off/on | off |
| `level` | float | −24…+6 dB | 0 |

Read with `apvts.getRawParameterValue (PARAM_GAIN_ID)->load()` in `processBlock`.  
UI binds through `SliderAttachment` / `ButtonAttachment` in `PluginEditor.cpp`.

State: `getStateInformation` / `setStateInformation` XML of the ValueTree. Do not roll your own preset format unless you version it.

Manufacturer code `Nhil`, plugin code `Wrck` — four characters, unique. Changing them makes hosts treat it as a new plug-in.

## 6. Editor

`PluginEditor` is 550×400, rotaries + three toggles.  
`paint()` is the faceplate; `resized()` is layout. Keep hit targets ≥ 90 px for knobs.

To skin it: load a `Image` from BinaryData and `g.drawImage` in `paint()`. Do not block on disk I/O in the editor constructor if you can embed the PNG.

## 7. Validate before shipping

```
pluginval.exe --strictness-level 5 --validate-in-process ^
  build\Warlock_artefacts\Release\VST3\Warlock.vst3
```

Must pass: no alloc in audio thread (as far as pluginval can see), mono+stereo layouts, state save/restore, editor open/close, denormals.

Then compile `installer.iss` (see [INSTALL.md](INSTALL.md)). Bump `VERSION` in:

- `CMakeLists.txt` (`project` + `juce_add_plugin`)
- `installer.iss` (`MyAppVersion`)
- `Resources/legal.txt`

Keep `AppId` GUID **stable** forever or Windows will not upgrade in place.

## 8. Common failures

| Symptom | Cause |
|---|---|
| DAW does not list Warlock | Bundle not under `Common Files\VST3`, or only the inner DLL was copied |
| Rebuild fails “file locked” | DAW still loaded the editor — close the project |
| Automation names change | You renamed a `PARAM_*_ID` |
| Cab silent | `juce_add_binary_data` not linked, or `loadImpulseResponse` after `prepare` |
| Harsh digital hash | Oversampler not prepared, or processUp/Down skipped |
| 32-bit host | This project is x64 only (`ArchitecturesAllowed=x64compatible`) |

## 9. Adding a parameter (checklist)

1. `#define PARAM_FOO_ID` / `_NAME` in `PluginProcessor.h`
2. `layout.add` in `createParameterLayout()`
3. Read it in `processBlock` (or `updateToneCoeffs`)
4. Slider/button + attachment in the editor
5. Default in the layout matches the browser `DEFAULT_PARAMS` if you care
6. Do not reuse an old ID

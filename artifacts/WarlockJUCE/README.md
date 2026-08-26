# Warlock Chainsaw Distortion — JUCE VST3 1.1.0

Nihil Audio. Windows x64. Same DSP as the browser pedal.

| Doc | For |
|---|---|
| [INSTALL.md](INSTALL.md) | Put it in a DAW (screenshots) |
| [CONVOLUTION.md](CONVOLUTION.md) | Cab IR: FIR vs FFT, min-phase, tail |
| [OVERSAMPLING.md](OVERSAMPLING.md) | 4× half-band IIR vs FIR, where we clip |
| [ENCLOSURE.md](ENCLOSURE.md) | 1590XX lid, powder coat, UV/silkscreen, drill Ø |

```
build.bat
```

or

```
cmake --preset vs2022-x64
cmake --build --preset warlock-release
```

Needs git, CMake ≥ 3.22, VS 2022. JUCE 7.0.12 is fetched by CMake.

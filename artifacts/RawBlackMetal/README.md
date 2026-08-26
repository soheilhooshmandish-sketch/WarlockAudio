# Raw Black Metal Distortion

JUCE 7 C++17 VST3. Three parameters. Chain is fixed.

```
HPF 130 Hz Butterworth
  → drive 0–36 dB
  → asym clip  (x>0: tanh(x) ; x≤0: x / (1 + 0.5|x|))
  → auto-gain  1 / (1 + 0.3·DriveLin)
  → LPF 7 kHz Butterworth
  → cavern reverb (size 0.85, damp 0.4, width 1)
  → output trim
  → lookahead brickwall −0.3 dBFS
```

| ID | Range | Default |
|---|---|---|
| `drive` | 0–100 % | 72 |
| `reverb_mix` | 0–100 % | 15 |
| `output` | −24 … +6 dB | 0 |

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target RawBlackMetal_VST3
```

# Impulse response convolution (Warlock cab)

An IR is the cabinet’s answer to a click. Convolve guitar with that click and you get the speaker + mic, without a room.

Warlock’s IRs are **16-bit PCM mono, minimal RIFF** (no LIST/fact chunks):

| File | Rate | Length | On disk |
|---|---|---|---|
| `Cab_Center.wav` | 44.1 kHz | 63.5 ms (2800) | 5644 B |
| `Cab_Edge.wav` | 44.1 kHz | 63.5 ms (2800) | 5644 B |
| `CabinetIR.wav` | 48 kHz | 10.7 ms (512) | 1068 B |

Stereo sources were L≈R (corr > 0.998); folded to mono. 5 ms cosine fade-out. Loaded as `Stereo::no` so JUCE does not keep a dummy second channel.

## Math

```
y[n] = Σ h[k] · x[n − k]     k = 0 .. N−1
```

Time-domain FIR is O(N) per sample. Fine at N = 256. Painful at N = 48 000 (1 s reverb). Then you use **uniform partitioned FFT convolution** (overlap-add on blocks). JUCE `dsp::Convolution` does the latter even for short IRs.

| Kind | Peak of h[k] | Extra latency | Cab use |
|---|---|---|---|
| Minimum phase | sample 0 | ~0 | live / this pedal |
| Linear phase | N/2 | N/2 | mix, not live |
| Max phase | end | worst | don’t |

Trim leading silence (`Convolution::Trim::yes`) so a padded file does not add delay. Do **not** normalise a cab IR (`Normalise::no`) — that would flatten the 4×12’s mid scoop.

## Where it sits

```
clip → tone stack → [Cab IR] → Level
```

Always **after** distortion. A cab is a speaker. Speakers do not clip; they low-pass and honk.

JUCE (`PluginProcessor.cpp`): `cabinetConvolution.loadImpulseResponse(BinaryData::CabinetIR_wav, …)` then `process` when Cab IR is on. Tail reported as 6 ms.

Browser: **no convolver**. `ConvolverNode` is FFT-based and added too much latency, so Cab IR there is a 4.2 kHz biquad stand-in. Same idea (dark 4×12), not the same math.

## Host facts

- Latency of a min-phase cab ≈ IR peak index (here ~0) plus the FFT partition size JUCE picks. Oversampling latency dominates; we report that, not the 5 ms tail.
- Tail (`getTailLengthSeconds`) is how long to keep processing after stop = IR length.
- Stereo::no — IRs are mono; JUCE upmixes at the cab if the buffer is stereo.

## Swapping the WAV

Replace `Resources/CabinetIR.wav`, rebuild. Keep it short (≤ 2048 samples) and 44.1/48 kHz. Long room IRs make a pedal feel late and wash the gate.

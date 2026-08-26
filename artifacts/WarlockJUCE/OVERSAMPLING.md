# Oversampling (Warlock clipper)

Hard clip is nonlinear. At 48 kHz a square edge makes harmonics past Nyquist; they fold back as digital hash. Raise the rate, clip, then drop back. That is oversampling.

Warlock: **4×**, two cascaded **2× half-band** stages. JUCE ctor `factor` is a power of two: `2` → \(2^2 = 4\).

```
48 kHz  →  upsample 4×  →  192 kHz
            HPF 320 → drive → diode clip → 1.2 k / Grim / Cold
192 kHz  →  downsample 4×  →  48 kHz  →  Lo-Fi / Cab IR
```

Only the **nonlinear** section (clip + the EQ that sits on the clipped wave) runs at 4×. Gate, cab, and level stay at host rate.

## Filter families

| Type | JUCE enum | Phase | Latency | Sound |
|---|---|---|---|---|
| FIR equiripple half-band | `filterHalfBandFIREquiripple` | linear | high (tens of samples × stages) | clean, late |
| IIR / polyphase allpass half-band | `filterHalfBandPolyphaseIIR` (`Allpass` in 7.0.x) | nonlinear near Nyquist | low | tight, a little smear at the top |

Warlock uses **polyphase IIR, max quality, integer latency**. Feel stays live; `setLatencySamples` reports a whole number so the DAW can compensate.

`isMaxQuality = true` tightens transition bands (~10 dB more stopband per stage) at extra CPU.

## Other techniques (not in this plugin)

- **Zero-stuff + FIR low-pass** — classic upsample; linear phase; same as JUCE FIR path.
- **Polyphase IIR 2× cascade** — what we ship; cheapest decent 4×/8×.
- **8× / 16×** — quieter alias on brutal clip; more latency and CPU. Diminishing returns after 4× for guitar.
- **Oversample only the shaper** — Waveshaper at 4×, filters at 1×. Cheaper; EQ then aliases if it is after a dirty clip. We keep the Swedish stack inside the 4× block on purpose.
- **Antiderivative antialiasing (ADAA)** — analytic; almost no latency. Good for tanh, awkward for a hard diode clamp.
- **MinBLEP / PolyBLEP** — for oscillators, not pedals.

## Host contract

```
initProcessing(blockSize)
setLatencySamples(oversampler->getLatencyInSamples())
processSamplesUp → clip → processSamplesDown
```

IIRs inside the 4× block must be `prepare()`d at `sampleRate * 4`. That is now done in `prepareToPlay`.

Browser replica does **not** oversample (WaveShaper at 1×) — latency first. The VST3 pays for 4×.

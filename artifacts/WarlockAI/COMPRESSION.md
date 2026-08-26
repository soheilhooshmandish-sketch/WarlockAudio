# Dynamic range compression algorithms

A compressor is four blocks. The sound lives in how you wire them, not in the ratio knob.

```
level detect → gain computer → ballistics → VCA
```

WarlockAI is **feed-forward, peak, log-domain, decoupled ballistics, soft knee**, plus the 90 Hz sidechain HPF from [SIDECHAIN.md](SIDECHAIN.md).

## 1. Topology

| | Feed-forward | Feedback |
|---|---|---|
| Detector sees | input | output (after VCA) |
| Ratio | exact, including ∞:1 | limited; high ratios go unstable |
| Lookahead / limiter | yes | no |
| Feel | tight, punchy | self-correcting, “breathes” |
| Analog cousins | SSL G bus, dbx 160 | 1176, LA-2A, vari-mu |

Digital designs should be feed-forward. Detector dynamic range is free in float. Feedback is a colour, not a requirement. Giannoulis / Massberg / Reiss (JAES 2012) recommend FF for that reason.

## 2. Detector

| | Peak | RMS |
|---|---|---|
| Measures | `|x|` | short-window power |
| Speed | catches every pick transient | closer to loudness |
| Analog cousins | FET / 1176 | opto / LA-2A |

Peak on a chainsaw guitar = the compressor slams every chug. RMS = smoother sustain, late attack. Warlock uses **peak** after the sidechain HPF so palm-mute rumble does not own the detector.

## 3. Domain

Linear envelope (`y += coeff * (x - y)` on `|x|`) then convert to dB: attack lags, knee is messy.

Log envelope (dB first, then smooth the **gain reduction**): no attack lag, knee is a quadratic in dB, time constants mean what the labels say. That is the JAES default.

## 4. Ballistics

**Branching** (what we had): one envelope, switch attack/release when the signal crosses it. Release time is not the labeled τ — it mixes with attack.

**Decoupled** (what we use now):

```
yR = max(x, αR·yR + (1-αR)·x)   // release path
y  = αA·y  + (1-αA)·yR           // attack path
```

Attack and release are independent. Smooth variant uses one-pole on both sides so the GR curve has no slope discontinuity.

α = exp(−1 / (τ_seconds · sampleRate))

## 5. Gain computer

Hard knee: if level > thresh, GR = −(level − thresh) · (1 − 1/ratio)

Soft knee (width W dB): quadratic blend in ±W/2 around threshold. Warlock default W = 6 dB.

Makeup is a static dB after GR. Mix is dry/wet on the VCA output (New York / parallel).

## 6. Analog circuit families (colour, not the math)

| Family | Element | Digital stand-in |
|---|---|---|
| VCA | THAT / DBX chip | this compressor |
| FET | 1176 | peak, 1–5 ms attack, feed-forward or FB |
| Opto | LA-2A | RMS-ish, 10–50 ms attack, program-dependent release |
| Vari-mu | Fairchild | feedback, soft knee, no real ratio knob |
| Diode bridge | Neve 2254 | harmonic grit on GR |

We do not model photocells or FETs. The algorithm above is the VCA/digital core. Distortion already lives in `DistortionProcessor`.

## 7. What Warlock does *not* do

- Lookahead (delay the audio so GR starts before the transient) — that is the limiter’s job if we add it
- Upward compression / expanders — the gate is the expander
- Multiband — use the EQ + this comp, or the cab HPF
- Feedback mode — not needed on a high-gain guitar bus

# Slew-rate parameters

Slew is how fast a control signal is allowed to move. It is not EQ. Three different slews live in this project and they are easy to mix up.

## 1. Exponential one-pole (Warlock gate / Raw BM limiter)

```
α = 1 − exp(−1 / (τ_seconds · sampleRate))
y += α · (x − y)
```

τ is the time to get ~63 % of the way to the target. Bigger jump → faster *initial* slope, then it eases. That is why a 4 ms gate slew still opens quicker on a loud pick than on a ghost note.

| Control | τ now | If you shorten it | If you lengthen it |
|---|---|---|---|
| Gate detector attack | 1 ms | clicks on the pick | late chugs |
| Gate detector release | **Release** knob | chops the tail | hiss between notes |
| **Gate gain slew** | **4 ms** (`gateslew`) | tick on open/close | smeared first note |
| Raw BM limiter attack | 0.8 ms | clicks on peaks | overshoots −0.3 dBFS |
| Raw BM limiter release | 20 ms | pumping | slow recover |

`gateslew` is now an APVTS parameter, 0.5–20 ms, default 4.

## 2. Linear rate (dB/s)

Clamp `|y − x|` per sample to `rate / sampleRate`. Same slope on every transient. Compressors that advertise “dB/s attack” use this. WarlockAI `ParameterSmoother` is the linear cousin: a fixed-time ramp in seconds, not a one-pole.

Use linear rate for **knob automation** (drive, level) so zipper noise is a constant fade, not a program-dependent whoosh.

## 3. Audio-rate slew limiter

Limit `dx/dt` of the *waveform*. That is a nonlinear low-pass / analog-modelling trick, not a gate. Do not put it on the guitar unless you want extra IMD.

## Black-metal starting points

- Blast beats: slew 1–2 ms (tight, a little click is acceptable)
- Tremolo wall: slew 3–5 ms (default)
- Atmospheric / DSBM: slew 8–15 ms (gate becomes a fade)

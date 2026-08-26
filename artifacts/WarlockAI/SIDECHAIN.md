# Compressor sidechain filtering

The compressor gain cell still works on the **full audio**. Sidechain filters only change **what the detector hears**.

```
audio ──────────────────────────────► VCA / multiply ► out
   │
   └── HPF ── LPF ── envelope ── GR ─┘
         ↑ detector only
```

Without an HPF, kick / palm-mute / 82 Hz guitar fundamentals hit the threshold first. The whole riff ducks. That is pumping, not “too much ratio.”

## What to use on Warlock

| Mode | HPF | LPF | Why |
|---|---|---|---|
| Off | — | — | Detector = raw peak (current code before this change) |
| Guitar glue | **90 Hz** | off | Ignores open-E rumble and cab thump; still hears pick attack |
| High-gain / BM | **120–150 Hz** | **8 kHz** | Stops sub and fizz from slamming GR; midrange chugs drive it |
| De-ess detector | 6 kHz | 10 kHz | Listen only to harsh band (not used here) |

SSL-style bus comps put the HPF around 60–185 Hz. Mastering often wants ~120 Hz. Guitar cabs dump energy at 80–120 Hz, so 90–150 Hz is the useful range. Slope 12 dB/oct is enough; 6 dB/oct is what a series capacitor in an insert cable gives you.

The audio path is **not** high-passed. Lows still get compressed when mids trigger GR. They just stop *causing* GR.

## WarlockAI

`Compressor` now has `setSidechainHpf` / `setSidechainLpf` (Hz). `0` = that filter off.

Defaults in the chain: HPF **90 Hz**, LPF off. Detector is a mono max of the filtered L/R, then the same peak envelope + soft knee as before.

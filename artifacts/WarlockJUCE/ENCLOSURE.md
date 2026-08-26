# Custom stompbox enclosure designs — Warlock

Warlock is a **Hammond 1590XX** portrait lid, not a 125B compact. Six rotaries + trim + three rockers + LCD + 3PDT will not fit a 1590B.

## Casting sizes (industry standard)

| Box | Lid ≈ mm | Typical load |
|---|---|---|
| 1590A | 92 × 38 | 1–2 knobs, mini fuzz |
| 1590B | 112 × 61 | 3 knobs + stomp (Tube Screamer class) |
| 125B / 1590N1 | 119 × 64 | 3–4 knobs, room for top jacks |
| 1590BB | 119 × 94 | 6 knobs or 4 knobs + toggles |
| **1590XX** | **145 × 121** | **Warlock: 6 knobs + trim + rockers + LCD + stomp** |
| 1590DD | 189 × 120 | dual-stomp / amp-in-a-box |

Die-cast aluminum, lap-joint lid, four corner screws into tapped bosses, IP54-class dust/splash. Hammond and Tayda clones share these footprints so drill jigs transfer. {render_inline_citation citation_id="83"}

## Finish stack (what shops actually do)

1. **Vibra-finish** the raw casting (no sharp flash).
2. **Powder coat** — matte black epoxy for Warlock. UV ink will not bond to bare aluminum; coat first. {render_inline_citation citation_id="88"}
3. **Graphics** (pick one):
   - **Silkscreen** — 1–2 spot colors (bone + frost). Cheap at 25+ units. Best for “GAIN / COLD / GRIM” legends.
   - **UV digital** — full-color seal (the circular THE WARLOCK logo). Cured ink, no clear-coat needed if the shop is Mimaki-class. {render_inline_citation citation_id="90"}
   - **CNC engrave / deboss** — logo well and model stamp. Survives floor abuse; no color unless you fill.
4. Optional gloss varnish only over the seal, not over knobs.

Keepout: **4 mm inside the curved lip** and **clear of the four screw bosses**. Tayda / Dazatronyx clip anything on the radius.

## Face layout we already follow

```
        [ IN ]  [ 9V − ]  [ OUT ]     ← top-end Neutrik + 2.1 mm DC
   ┌─────────────────────────────────┐
   │ ●                           ●   │  M3.5 lid screws
   │         [ circular seal ]       │  UV print in a stamped well
   │      CHAINSAW DISTORTION        │  silkscreen
   │      NIHIL AUDIO · 1510         │  silkscreen
   │   GAIN     COLD      GRIM       │  16 mm pots, 7 mm holes
   │   GATE    RELEASE   LEVEL       │
   │    ASYM     CAB IR    LO-FI     │  mini rockers, 6 mm
   │  TRIM  LED  [ LCD ]  [STOMP]    │  3PDT 12 mm, LED 5–8 mm
   │ ●                           ●   │
   └─────────────────────────────────┘
        rubber feet on the lid underside
```

Drill Ø (print 1:1, no page scaling): pots 7 mm, 3PDT 12 mm, LED 5–8 mm, ¼″ jacks 10 mm, DC 8–12 mm. {render_inline_citation citation_id="94"}

Hardware: nickel 6.35 mm jacks, 2.1 mm DC **centre-negative**, aluminum 1510 knobs, mushroom 3PDT, ground lug to the chassis (the box *is* the shield).

## Three design directions (if we ever reskin)

| Concept | Finish | Graphic | Feel |
|---|---|---|---|
| **1510 Black Forge** (current) | matte black powder | UV seal + frost silkscreen | Norwegian bedroom, 1993 |
| **Bare 1590** | vibra raw aluminum | black laser engrave only | workshop / proto |
| **Hammertone Relic** | hammered silver coat | one-color black screen | vintage fuzz, less BM |

Do not put art on the side walls (jacks live there). Do not UV-print under knob skirts — the skirt will grind it off.

## Plugin vs physical

The VST3 editor and the web pedal are the **same lid**: powder-coat texture, inner bevel, logo well, 9V − print, four screws, 1510 knobs, 3PDT well. They are not a drill file.

Physical artboard if you ever send this to a shop: **121 × 145 mm**, 300 dpi, 4 mm inner margin, logo as a circular PNG with transparency, legends as vector.

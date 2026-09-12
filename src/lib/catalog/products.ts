export type ProductStatus = "in-development" | "concept";

export type WarlockProduct = {
  slug: string;
  name: string;
  realm: string;
  category: string;
  character: string;
  status: ProductStatus;
  statusLabel: string;
  summary: string;
  evidenceNote: string;
  releaseBlocker: string;
};

export const products: WarlockProduct[] = [
  {
    slug: "void",
    name: "VOID",
    realm: "void",
    category: "SPATIAL / TEXTURE EFFECT",
    character: "Pitch, granular, reverse, freeze and spatial motion",
    status: "in-development",
    statusLabel: "GUITAR TEST DEVELOPMENT",
    summary:
      "A founder-directed WARLOCK Signature effect built around pitch, granular and reverse processing, freeze behavior, spatial motion and controlled wet/dry output.",
    evidenceNote:
      "The Factory repository identifies VOID as Guitar Test Development and explicitly says its first delivery is not a commercial final.",
    releaseBlocker:
      "Founder Guitar Test PASS and Engineering QC PASS remain required before commercial final status.",
  },
  {
    slug: "thall",
    name: "THALL",
    realm: "thall",
    category: "HEAVY GUITAR EFFECT",
    character: "Low-tuned transient control and extreme rhythmic weight",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A WARLOCK Signature direction for low-tuned modern heavy guitar. Public commercial readiness has not been proven yet.",
    evidenceNote:
      "Current company planning names THALL as a Signature family, but no commercial release evidence is published on the website.",
    releaseBlocker: "Product build and release evidence must be proven before status can advance.",
  },
  {
    slug: "abyss",
    name: "ABYSS",
    realm: "abyss",
    category: "ATMOSPHERIC EFFECT",
    character: "Deep cold space and heavy ambience",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A dark atmospheric Signature direction. It is presented as product DNA, not as a sellable release.",
    evidenceNote: "No commercial release package is currently claimed for ABYSS.",
    releaseBlocker: "Validated ProductSpec, build evidence and release gates are still required.",
  },
  {
    slug: "chimera",
    name: "CHIMERA",
    realm: "chimera",
    category: "MULTI-EFFECT CONCEPT",
    character: "Hybrid organic-metal processing",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A hybrid WARLOCK Signature product direction with a biomechanical visual realm and multi-effect identity.",
    evidenceNote: "Current status is concept/DNA only; no sellable binary is claimed.",
    releaseBlocker: "Validated DSP, UI, host and commercial release evidence remain required.",
  },
  {
    slug: "chimera-disto",
    name: "CHIMERA DISTO",
    realm: "chimera",
    category: "DISTORTION / HARMONY CONCEPT",
    character: "Layered drive, hybrid octave movement and controlled harmonic weight",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A distortion-led CHIMERA direction: controlled gain, layered interval colour and the same hybrid WARLOCK visual realm.",
    evidenceNote: "CHIMERA DISTO is restored to the private catalogue as a product direction; no release binary is claimed.",
    releaseBlocker: "Validated DSP, UI, host testing and complete release evidence are required before it can be sold.",
  },
  {
    slug: "alien",
    name: "ALIEN",
    realm: "alien",
    category: "EXPERIMENTAL EFFECT CONCEPT",
    character: "Unfamiliar textures and aggressive transformation",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "An experimental Signature direction for unconventional processing and biomechanical identity.",
    evidenceNote: "Current status is concept/DNA only; no sellable binary is claimed.",
    releaseBlocker: "Validated DSP architecture and full release evidence remain required.",
  },
  {
    slug: "black-frost",
    name: "BLACK FROST",
    realm: "frost",
    category: "COLD HEAVY EFFECT CONCEPT",
    character: "Cold attack, steel texture and restrained space",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A cold steel WARLOCK Signature realm intended for precise heavy processing.",
    evidenceNote: "Current status is concept/DNA only; no sellable binary is claimed.",
    releaseBlocker: "Product implementation and release evidence remain required.",
  },
  {
    slug: "fracture",
    name: "FRACTURE",
    realm: "fracture",
    category: "DISTORTION CONCEPT",
    character: "Broken transients, heat and controlled violence",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A distortion-focused Signature direction with a fractured industrial identity.",
    evidenceNote: "Current status is concept/DNA only; no sellable binary is claimed.",
    releaseBlocker: "Validated implementation and release gates remain required.",
  },
  {
    slug: "disto-synth",
    name: "DISTO/SYNTH",
    realm: "synth",
    category: "HYBRID EFFECT CONCEPT",
    character: "Distortion and synthesis crossover",
    status: "concept",
    statusLabel: "CONCEPT / DNA",
    summary:
      "A hybrid distortion/synthesis Signature direction. It remains a future product family rather than a commercial claim.",
    evidenceNote: "Current status is concept/DNA only; no sellable binary is claimed.",
    releaseBlocker: "DSP architecture, host validation and release evidence remain required.",
  },
];

export function productBySlug(slug: string): WarlockProduct | undefined {
  return products.find((product) => product.slug === slug);
}

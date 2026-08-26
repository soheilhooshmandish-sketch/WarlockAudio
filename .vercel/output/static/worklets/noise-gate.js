/**
 * Input gate — JUCE envelope follower, 1 ms attack, 5–200 ms release.
 * Below threshold: 8:1 expander (hard choke without zipper clicks).
 */
class NoiseGateProcessor extends AudioWorkletProcessor {
  static get parameterDescriptors() {
    return [
      {
        name: "threshold",
        defaultValue: 0.00031622776,
        minValue: 0,
        maxValue: 1,
        automationRate: "k-rate",
      },
      {
        name: "release",
        defaultValue: 0.03,
        minValue: 0.005,
        maxValue: 0.2,
        automationRate: "k-rate",
      },
    ];
  }

  constructor() {
    super();
    this.env = [];
  }

  process(inputs, outputs, parameters) {
    const input = inputs[0];
    const output = outputs[0];
    if (!output || !output[0]) return true;

    const frames = output[0].length;
    const th = Math.max(1e-8, parameters.threshold[0]);
    const relSec = Math.max(0.005, parameters.release[0]);
    const attackCoef = 1 - Math.exp(-1 / (sampleRate * 0.001));
    const releaseCoef = 1 - Math.exp(-1 / (sampleRate * relSec));

    if (!input || !input[0]) {
      for (const ch of output) ch.fill(0);
      return true;
    }

    const chs = Math.min(input.length, output.length);
    while (this.env.length < chs) this.env.push(0);

    for (let c = 0; c < chs; c++) {
      const inp = input[c];
      const out = output[c];
      let env = this.env[c];
      for (let i = 0; i < frames; i++) {
        const x = inp[i];
        const mag = Math.abs(x);
        if (mag > env) env += attackCoef * (mag - env);
        else env += releaseCoef * (mag - env);
        const over = env / th;
        const g = over >= 1 ? 1 : over * over * over * over;
        out[i] = x * g;
      }
      this.env[c] = env;
    }
    for (let c = chs; c < output.length; c++) {
      output[c].set(output[0]);
    }
    return true;
  }
}

registerProcessor("noise-gate", NoiseGateProcessor);

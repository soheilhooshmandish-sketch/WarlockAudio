# Warlock Occult Plugin Project Rules for Grok Build CLI
- **Language Stack**: C++17, JUCE 7 DSP Module APIs, Inno Setup Compiler.
- **Visual Design Identity**: Satanic Black Metal aesthetic. UI elements utilize deep crimson, ash black, and rusted steel tones with raw jagged spikes.
- **Custom LookAndFeel Drawing**: Overrides JUCE rotary dials to render programmatic inverted pentagram vector lines pointing downward at a 180-degree offset.
- **Thematic Parameter Mapping**:
  * Input Gain = INVOCATION
  * Distortion Saturation = POSSESSION
  * Dynamic Noise Gate = SACRIFICE
  * Sub-Harmonic Generator = DEMONIC SUB
  * Random Voltage Fluctuation = PSYCHOSIS
  * Post EQ Bands = DESPAIR (Low), AGONY (Mid), WRATH (High)
  * Spatial Channel Matrix = SOUL CAPTURE
- **Audio Routing Constraints**: Maintain True Stereo and Mono-to-Stereo 12ms phase widening arrays across compile phases.
- **Optimization Gate**: Embed `/O2 /Oi /Ot /Oy /AVX2 /FP:fast` code optimization targets on Windows MSVC.
- **Asset Boundaries**: Ensure all graphic files (`PedalFaceplate.png`) and convolution impulse targets (`Cab_Center.wav`, `Cab_Edge.wav`) resolve out of the `Resources/` workspace tree without local system hardcoding.

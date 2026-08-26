#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
struct DistortionSettings
{
    float drive = 0.45f;
    float tone = 0.55f;
    float tightness = 0.4f;
    float saturation = 0.35f;
    float warmth = 0.3f;
    float bite = 0.25f;
    float outputDb = 0.0f;
    int mode = static_cast<int> (DistortionMode::Metal);
    int oversampleIndex = 2; // 0 = off, 1 = 2x, 2 = 4x
    bool bypassed = false;

    // Adaptive AI hints written from the analysis thread via atomics.
    float adaptiveBrightness = 0.5f;
    float adaptiveDensity = 0.5f;
    float adaptiveDynamics = 0.5f;
    float adaptiveRmsDb = -20.0f;
    float adaptivePeakDb = -6.0f;
    float adaptivePicking = 0.5f;
};

class DistortionProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { settings.bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return settings.bypassed; }

    void setSettings (const DistortionSettings& newSettings) noexcept { settings = newSettings; }

    int getLatencySamples() const noexcept;

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;

    void processNonlinear (juce::dsp::AudioBlock<float> block) noexcept;
    float shapeSample (float x, DistortionMode mode, float gain, float sat, float warm, float biteAmt) const noexcept;
    void updateFilters (double rate) noexcept;

    DistortionSettings settings;
    ParameterSmoother driveSmoother;
    ParameterSmoother satSmoother;
    ParameterSmoother outSmoother;
    ParameterSmoother adaptiveDriveSmoother;
    ParameterSmoother adaptiveTightSmoother;
    double sampleRate = 44100.0;
    int numChannels = 2;

    juce::dsp::Oversampling<float> oversampler2x { 2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };
    juce::dsp::Oversampling<float> oversampler4x { 2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };

    std::array<Filter, 2> tightnessFilters;
    std::array<Filter, 2> biteFilters;
    std::array<Filter, 2> toneFilters;
    std::array<Filter, 2> warmthFilters;

    int lastMode = -1;
    float lastTightness = -1.0f;
    float lastTone = -1.0f;
    float lastWarmth = -1.0f;
    float lastBite = -1.0f;
    float filterTightness = 0.4f;
};
} // namespace WarlockAI

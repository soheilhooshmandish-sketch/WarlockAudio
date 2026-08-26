#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
class CabinetProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setEnabled (bool shouldEnable) noexcept { bypassed = ! shouldEnable; }
    void setMix (float amount) noexcept { mix = juce::jlimit (0.0f, 1.0f, amount); }
    void setLowCut (float hz) noexcept { lowCutHz = hz; dirty = true; }
    void setHighCut (float hz) noexcept { highCutHz = hz; dirty = true; }
    void setStereo (bool shouldBeStereo) noexcept { stereoCab = shouldBeStereo; }

    // Future WAV IR loader — safe to call from the message thread only.
    bool loadImpulseFile (const juce::File& file);
    void clearLoadedImpulse();

    int getLatencySamples() const noexcept;

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;

    void updateFilters() noexcept;
    void generateSyntheticImpulse();

    double sampleRate = 44100.0;
    juce::uint32 maximumBlockSize = 512;
    juce::uint32 numChannels = 2;
    bool bypassed = false;
    bool stereoCab = true;
    bool dirty = true;
    bool irReady = false;

    float mix = 1.0f;
    float lowCutHz = 80.0f;
    float highCutHz = 6500.0f;

    ParameterSmoother mixSmoother;

    std::array<Filter, 2> lowCut;
    std::array<Filter, 2> highCut;
    std::array<Filter, 2> scoop;
    std::array<Filter, 2> presence;

    juce::dsp::Convolution convolution { juce::dsp::Convolution::NonUniform { 128 } };
    juce::AudioBuffer<float> dryScratch;
};
} // namespace WarlockAI

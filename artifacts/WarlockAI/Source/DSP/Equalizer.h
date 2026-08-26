#pragma once

#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
class Equalizer
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setLowShelf (float gainDb, float freqHz) noexcept;
    void setLowMid (float gainDb, float freqHz, float q) noexcept;
    void setHighMid (float gainDb, float freqHz, float q) noexcept;
    void setHighShelf (float gainDb, float freqHz) noexcept;

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;

    void updateIfNeeded() noexcept;

    double sampleRate = 44100.0;
    bool bypassed = false;
    bool dirty = true;

    float lowGain = 0.0f, lowFreq = 110.0f;
    float lowMidGain = 0.0f, lowMidFreq = 450.0f, lowMidQ = 1.0f;
    float highMidGain = 0.0f, highMidFreq = 1800.0f, highMidQ = 1.1f;
    float highGain = 0.0f, highFreq = 6500.0f;

    std::array<Filter, 2> lowShelf;
    std::array<Filter, 2> lowMid;
    std::array<Filter, 2> highMid;
    std::array<Filter, 2> highShelf;
};
} // namespace WarlockAI

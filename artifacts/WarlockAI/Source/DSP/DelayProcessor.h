#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <array>
#include <atomic>

namespace WarlockAI
{
class DelayProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setTimeMs (float ms) noexcept { timeMs = juce::jlimit (1.0f, 800.0f, ms); }
    void setFeedback (float amount) noexcept { feedback = juce::jlimit (0.0f, 0.95f, amount); }
    void setMix (float amount) noexcept { mix = juce::jlimit (0.0f, 1.0f, amount); }
    void setLowCut (float hz) noexcept { lowCutHz = hz; dirty = true; }
    void setHighCut (float hz) noexcept { highCutHz = hz; dirty = true; }
    void setWidth (float amount) noexcept { width = juce::jlimit (0.0f, 2.0f, amount); }
    void setMode (int newMode) noexcept { mode = juce::jlimit (0, static_cast<int> (DelayMode::NumModes) - 1, newMode); }
    void setSync (int newSync) noexcept { sync = juce::jlimit (0, static_cast<int> (DelaySync::NumModes) - 1, newSync); }
    void setTempoBpm (float bpm) noexcept { tempoBpm = juce::jlimit (20.0f, 300.0f, bpm); }

    float getWetLevel() const noexcept { return wetLevel.load (std::memory_order_relaxed); }

private:
    using Delay = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;

    void updateFilters() noexcept;
    float syncedTimeMs() const noexcept;

    double sampleRate = 44100.0;
    bool bypassed = true;
    bool dirty = true;
    float timeMs = 320.0f;
    float feedback = 0.25f;
    float mix = 0.0f;
    float lowCutHz = 80.0f;
    float highCutHz = 8000.0f;
    float width = 1.0f;
    int mode = static_cast<int> (DelayMode::Stereo);
    int sync = 0;
    float tempoBpm = 120.0f;

    ParameterSmoother mixSmoother;
    ParameterSmoother timeSmoother;

    std::array<Delay, 2> lines;
    std::array<Filter, 2> hp;
    std::array<Filter, 2> lp;
    std::atomic<float> wetLevel { 0.0f };
};
} // namespace WarlockAI

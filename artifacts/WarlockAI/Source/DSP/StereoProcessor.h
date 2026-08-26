#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <atomic>

namespace WarlockAI
{
class StereoProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setWidth (float amount) noexcept { width = juce::jlimit (0.0f, 2.0f, amount); }
    void setBalance (float amount) noexcept { balance = juce::jlimit (-1.0f, 1.0f, amount); }
    void setHaasMs (float ms) noexcept { haasMs = juce::jlimit (0.0f, 20.0f, ms); }
    void setMonoMode (bool shouldFold) noexcept { monoMode = shouldFold; }
    void setMidGainDb (float db) noexcept { midGainDb = juce::jlimit (-24.0f, 24.0f, db); }
    void setSideGainDb (float db) noexcept { sideGainDb = juce::jlimit (-24.0f, 24.0f, db); }

    float getWidthMeter() const noexcept { return widthMeter.load (std::memory_order_relaxed); }

private:
    using Delay = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;

    double sampleRate = 44100.0;
    bool bypassed = false;
    bool monoMode = false;
    float width = 1.0f;
    float balance = 0.0f;
    float haasMs = 0.0f;
    float midGainDb = 0.0f;
    float sideGainDb = 0.0f;

    ParameterSmoother widthSmoother;
    ParameterSmoother midSmoother;
    ParameterSmoother sideSmoother;
    Delay haasDelay;
    std::atomic<float> widthMeter { 0.0f };
};
} // namespace WarlockAI

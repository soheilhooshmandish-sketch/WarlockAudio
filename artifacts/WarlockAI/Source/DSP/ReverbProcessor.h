#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <array>
#include <atomic>

namespace WarlockAI
{
class ReverbProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setRoomSize (float size) noexcept { roomSize = juce::jlimit (0.0f, 1.0f, size); }
    void setDamping (float amount) noexcept { damping = juce::jlimit (0.0f, 1.0f, amount); }
    void setWidth (float amount) noexcept { width = juce::jlimit (0.0f, 1.0f, amount); }
    void setMix (float amount) noexcept { mix = juce::jlimit (0.0f, 1.0f, amount); }
    void setPreDelayMs (float ms) noexcept { preDelayMs = juce::jlimit (0.0f, 200.0f, ms); }

    float getWetLevel() const noexcept { return wetLevel.load (std::memory_order_relaxed); }

private:
    using Delay = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;

    double sampleRate = 44100.0;
    bool bypassed = true;
    float roomSize = 0.35f;
    float damping = 0.5f;
    float width = 1.0f;
    float mix = 0.0f;
    float preDelayMs = 18.0f;

    ParameterSmoother mixSmoother;
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters parameters;
    std::array<Delay, 2> preDelay;
    juce::AudioBuffer<float> wetScratch;
    std::atomic<float> wetLevel { 0.0f };
};
} // namespace WarlockAI

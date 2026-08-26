#pragma once

#include <JuceHeader.h>

namespace WarlockAI
{
class Limiter
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setCeilingDb (float db) noexcept { ceilingDb = juce::jlimit (-24.0f, 0.0f, db); }
    void setReleaseMs (float ms) noexcept { releaseMs = juce::jmax (1.0f, ms); }

private:
    juce::dsp::Limiter<float> limiter;
    bool bypassed = false;
    float ceilingDb = -0.3f;
    float releaseMs = 50.0f;
};
} // namespace WarlockAI

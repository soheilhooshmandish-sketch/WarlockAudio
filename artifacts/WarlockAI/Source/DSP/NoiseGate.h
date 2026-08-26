#pragma once

#include <JuceHeader.h>

namespace WarlockAI
{
class NoiseGate
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setThresholdDb (float db) noexcept { thresholdDb = db; }
    void setAttackMs (float ms) noexcept { attackMs = juce::jmax (0.05f, ms); }
    void setReleaseMs (float ms) noexcept { releaseMs = juce::jmax (1.0f, ms); }
    void setRatio (float newRatio) noexcept { ratio = juce::jmax (1.0f, newRatio); }

private:
    void updateCoefficients() noexcept;

    double sampleRate = 44100.0;
    int numChannels = 2;
    bool bypassed = false;

    float thresholdDb = -48.0f;
    float attackMs = 1.5f;
    float releaseMs = 80.0f;
    float ratio = 8.0f;

    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float envelope = 0.0f;
};
} // namespace WarlockAI

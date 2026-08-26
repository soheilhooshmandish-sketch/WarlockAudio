#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <array>
#include <vector>

namespace WarlockAI
{
class Harmonizer
{
public:
    static constexpr int grainSize = 1024;

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setEnabled (bool shouldEnable) noexcept { bypassed = ! shouldEnable; }
    void setInterval1 (int index) noexcept { interval1 = index; }
    void setInterval2 (int index) noexcept { interval2 = index; }
    void setMix (float amount) noexcept { mix = juce::jlimit (0.0f, 1.0f, amount); }
    void setDetuneCents (float cents) noexcept { detuneCents = juce::jlimit (0.0f, 50.0f, cents); }
    void setPan1 (float amount) noexcept { pan1 = juce::jlimit (-1.0f, 1.0f, amount); }
    void setPan2 (float amount) noexcept { pan2 = juce::jlimit (-1.0f, 1.0f, amount); }

    int getLatencySamples() const noexcept { return bypassed ? 0 : grainSize; }

private:
    struct Voice
    {
        std::vector<float> buffer;
        int write = 0;
        float phase = 0.0f;

        void prepare (int size);
        void reset() noexcept;
        float process (float input, float ratio) noexcept;
    };

    double sampleRate = 44100.0;
    bool bypassed = true;
    int interval1 = static_cast<int> (HarmonyInterval::PerfectFifth);
    int interval2 = static_cast<int> (HarmonyInterval::Octave);
    float mix = 0.25f;
    float detuneCents = 4.0f;
    float pan1 = -0.4f;
    float pan2 = 0.4f;

    ParameterSmoother mixSmoother;
    std::array<Voice, 2> voices;
};
} // namespace WarlockAI

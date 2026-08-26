#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
class AmpProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setEnabled (bool shouldEnable) noexcept { bypassed = ! shouldEnable; }
    void setMode (int newMode) noexcept;
    void setGain (float amount) noexcept { gain = juce::jlimit (0.0f, 1.0f, amount); }
    void setBass (float db) noexcept { bassDb = db; dirty = true; }
    void setMid (float db) noexcept { midDb = db; dirty = true; }
    void setTreble (float db) noexcept { trebleDb = db; dirty = true; }
    void setPresence (float db) noexcept { presenceDb = db; dirty = true; }
    void setResonance (float amount) noexcept { resonance = juce::jlimit (0.0f, 1.0f, amount); dirty = true; }
    void setMasterDb (float db) noexcept { masterDb = db; }

    int getLatencySamples() const noexcept { return 0; }

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;

    void updateToneStack() noexcept;
    float shape (float x, AmpMode mode, float driveGain) const noexcept;

    double sampleRate = 44100.0;
    bool bypassed = false;
    bool dirty = true;
    int mode = static_cast<int> (AmpMode::Metal);
    float gain = 0.55f;
    float bassDb = 1.5f;
    float midDb = -1.0f;
    float trebleDb = 2.0f;
    float presenceDb = 1.0f;
    float resonance = 0.35f;
    float masterDb = 0.0f;

    ParameterSmoother gainSmoother;
    ParameterSmoother masterSmoother;

    std::array<Filter, 2> tightHp;
    std::array<Filter, 2> bassFilter;
    std::array<Filter, 2> midFilter;
    std::array<Filter, 2> trebleFilter;
    std::array<Filter, 2> presenceFilter;
    std::array<Filter, 2> resonanceFilter;

    int lastMode = -1;
};
} // namespace WarlockAI

#pragma once

#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <atomic>

namespace WarlockAI
{
class Compressor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::dsp::AudioBlock<float> block) noexcept;

    void setBypassed (bool shouldBypass) noexcept { bypassed = shouldBypass; }
    bool isBypassed() const noexcept { return bypassed; }

    void setThresholdDb (float db) noexcept { thresholdDb = db; }
    void setRatio (float newRatio) noexcept { ratio = juce::jmax (1.0f, newRatio); }
    void setAttackMs (float ms) noexcept { attackMs = juce::jmax (0.05f, ms); }
    void setReleaseMs (float ms) noexcept { releaseMs = juce::jmax (1.0f, ms); }
    void setMakeupDb (float db) noexcept { makeupDb = db; }
    void setMix (float amount) noexcept { mix = juce::jlimit (0.0f, 1.0f, amount); }
    void setKneeDb (float db) noexcept { kneeDb = juce::jlimit (0.0f, 24.0f, db); }

    // 0 Hz = filter off. Filters the detector only.
    void setSidechainHpf (float hz) noexcept { scHpfHz = hz; }
    void setSidechainLpf (float hz) noexcept { scLpfHz = hz; }

    float getGainReductionDb() const noexcept { return gainReductionDb.load (std::memory_order_relaxed); }

private:
    void updateCoefficients() noexcept;
    void updateSidechainFilters() noexcept;
    float computeGainDb (float levelDb) const noexcept;

    double sampleRate = 44100.0;
    bool bypassed = true;

    float thresholdDb = -18.0f;
    float ratio = 3.0f;
    float attackMs = 8.0f;
    float releaseMs = 120.0f;
    float makeupDb = 0.0f;
    float mix = 1.0f;
    float kneeDb = 6.0f;

    float scHpfHz = 90.0f;
    float scLpfHz = 0.0f;
    float lastScHpf = -1.0f;
    float lastScLpf = -1.0f;

    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float envelope = 0.0f;   // smoothed GR magnitude (dB, >= 0)
    float releaseState = 0.0f;

    juce::dsp::IIR::Filter<float> scHpf;
    juce::dsp::IIR::Filter<float> scLpf;
    bool scHpfOn = false;
    bool scLpfOn = false;

    ParameterSmoother mixSmoother;
    ParameterSmoother makeupSmoother;

    std::atomic<float> gainReductionDb { 0.0f };
};
} // namespace WarlockAI

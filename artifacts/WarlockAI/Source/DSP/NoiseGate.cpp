#include "DSP/NoiseGate.h"

namespace WarlockAI
{
void NoiseGate::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    numChannels = static_cast<int> (spec.numChannels);
    updateCoefficients();
    reset();
}

void NoiseGate::reset() noexcept
{
    envelope = 0.0f;
}

void NoiseGate::updateCoefficients() noexcept
{
    const auto sr = static_cast<float> (sampleRate);
    attackCoeff  = std::exp (-1.0f / juce::jmax (1.0f, attackMs * 0.001f * sr));
    releaseCoeff = std::exp (-1.0f / juce::jmax (1.0f, releaseMs * 0.001f * sr));
}

void NoiseGate::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed || block.getNumSamples() == 0)
        return;

    updateCoefficients();

    const float thresholdLin = juce::Decibels::decibelsToGain (thresholdDb);
    const float slope = 1.0f - (1.0f / ratio);
    const auto numSamples = block.getNumSamples();
    const auto channels = block.getNumChannels();

    for (size_t i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;

        for (size_t ch = 0; ch < channels; ++ch)
            peak = juce::jmax (peak, std::abs (block.getSample (static_cast<int> (ch), static_cast<int> (i))));

        const float coeff = peak > envelope ? attackCoeff : releaseCoeff;
        envelope = peak + coeff * (envelope - peak);

        float gain = 1.0f;

        if (envelope <= 1.0e-8f)
        {
            gain = 0.0f;
        }
        else if (envelope < thresholdLin)
        {
            const float envDb = juce::Decibels::gainToDecibels (envelope, -100.0f);
            const float reductionDb = slope * (envDb - thresholdDb);
            gain = juce::Decibels::decibelsToGain (reductionDb, -100.0f);
        }

        for (size_t ch = 0; ch < channels; ++ch)
            block.setSample (static_cast<int> (ch), static_cast<int> (i),
                             block.getSample (static_cast<int> (ch), static_cast<int> (i)) * gain);
    }
}
} // namespace WarlockAI

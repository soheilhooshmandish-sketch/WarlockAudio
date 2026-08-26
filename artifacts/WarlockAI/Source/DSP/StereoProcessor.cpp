#include "DSP/StereoProcessor.h"

namespace WarlockAI
{
void StereoProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    haasDelay.setMaximumDelayInSamples (juce::roundToInt (sampleRate * 0.03));
    haasDelay.prepare (spec);
    widthSmoother.reset (sampleRate, 0.03f, width);
    midSmoother.reset (sampleRate, 0.03f, juce::Decibels::decibelsToGain (midGainDb));
    sideSmoother.reset (sampleRate, 0.03f, juce::Decibels::decibelsToGain (sideGainDb));
    reset();
}

void StereoProcessor::reset() noexcept
{
    haasDelay.reset();
    widthMeter.store (0.0f, std::memory_order_relaxed);
}

void StereoProcessor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed)
        return;

    widthSmoother.setTarget (width);
    midSmoother.setTarget (juce::Decibels::decibelsToGain (midGainDb));
    sideSmoother.setTarget (juce::Decibels::decibelsToGain (sideGainDb));

    const float widthNow = widthSmoother.getNext();
    const float midG = midSmoother.getNext();
    const float sideG = sideSmoother.getNext();
    widthSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));
    midSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));
    sideSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));

    const auto channels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    if (channels == 1 || monoMode)
    {
        if (channels > 1)
        {
            auto* left = block.getChannelPointer (0);
            auto* right = block.getChannelPointer (1);
            for (size_t i = 0; i < numSamples; ++i)
            {
                const float m = 0.5f * (left[i] + right[i]) * midG;
                left[i] = m;
                right[i] = m;
            }
        }
        else
        {
            block.multiplyBy (midG);
        }

        widthMeter.store (0.0f, std::memory_order_relaxed);
        return;
    }

    auto* left  = block.getChannelPointer (0);
    auto* right = block.getChannelPointer (1);

    const float haasSamples = haasMs * 0.001f * static_cast<float> (sampleRate);
    const bool useHaas = haasSamples > 0.25f;
    if (useHaas)
        haasDelay.setDelay (haasSamples);

    const float leftGain  = balance <= 0.0f ? 1.0f : 1.0f - balance;
    const float rightGain = balance >= 0.0f ? 1.0f : 1.0f + balance;

    float sideAcc = 0.0f;
    float midAcc = 0.0f;

    for (size_t i = 0; i < numSamples; ++i)
    {
        float l = left[i];
        float r = right[i];

        if (useHaas)
        {
            haasDelay.pushSample (0, r);
            r = haasDelay.popSample (0);
        }

        const float mid  = 0.5f * (l + r) * midG;
        const float side = 0.5f * (l - r) * sideG * widthNow;
        left[i]  = (mid + side) * leftGain;
        right[i] = (mid - side) * rightGain;
        midAcc  += std::abs (mid);
        sideAcc += std::abs (side);
    }

    const float ratio = sideAcc / (midAcc + sideAcc + 1.0e-6f);
    const float previous = widthMeter.load (std::memory_order_relaxed);
    widthMeter.store (previous * 0.85f + ratio * 0.15f, std::memory_order_relaxed);
}
} // namespace WarlockAI

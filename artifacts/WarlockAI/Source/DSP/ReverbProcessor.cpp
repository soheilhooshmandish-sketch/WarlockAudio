#include "DSP/ReverbProcessor.h"

namespace WarlockAI
{
void ReverbProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    reverb.prepare (spec);
    mixSmoother.reset (sampleRate, 0.03f, mix);

    auto one = spec;
    one.numChannels = 1;
    const int maxDelay = juce::roundToInt (sampleRate * 0.25);

    for (auto& line : preDelay)
    {
        line.setMaximumDelayInSamples (maxDelay);
        line.prepare (one);
        line.reset();
    }

    wetScratch.setSize (static_cast<int> (juce::jmax<juce::uint32> (2, spec.numChannels)),
                        static_cast<int> (spec.maximumBlockSize), false, false, true);
    reset();
}

void ReverbProcessor::reset() noexcept
{
    reverb.reset();
    for (auto& line : preDelay)
        line.reset();
    wetLevel.store (0.0f, std::memory_order_relaxed);
}

void ReverbProcessor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed || mix <= 0.0001f)
    {
        wetLevel.store (wetLevel.load (std::memory_order_relaxed) * 0.9f, std::memory_order_relaxed);
        return;
    }

    mixSmoother.setTarget (mix);
    const float wetAmt = mixSmoother.getNext();
    mixSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));

    const auto channels = juce::jmin (static_cast<int> (block.getNumChannels()),
                                      juce::jmin (2, wetScratch.getNumChannels()));
    const auto numSamples = juce::jmin (static_cast<int> (block.getNumSamples()),
                                        wetScratch.getNumSamples());
    const float preSamples = juce::jlimit (0.0f,
                                           static_cast<float> (preDelay[0].getMaximumDelayInSamples() - 1),
                                           preDelayMs * 0.001f * static_cast<float> (sampleRate));

    for (int ch = 0; ch < channels; ++ch)
    {
        preDelay[static_cast<size_t> (ch)].setDelay (juce::jmax (1.0f, preSamples));
        auto* src = block.getChannelPointer (static_cast<size_t> (ch));
        auto* dst = wetScratch.getWritePointer (ch);
        auto& line = preDelay[static_cast<size_t> (ch)];

        for (int i = 0; i < numSamples; ++i)
        {
            line.pushSample (0, src[i]);
            dst[i] = line.popSample (0);
        }
    }

    juce::dsp::AudioBlock<float> wetBlock (wetScratch.getArrayOfWritePointers(),
                                           static_cast<size_t> (channels),
                                           static_cast<size_t> (numSamples));
    parameters.roomSize   = roomSize;
    parameters.damping    = damping;
    parameters.width      = width;
    parameters.wetLevel   = 1.0f;
    parameters.dryLevel   = 0.0f;
    parameters.freezeMode = 0.0f;
    reverb.setParameters (parameters);

    juce::dsp::ProcessContextReplacing<float> context (wetBlock);
    reverb.process (context);

    const float dryAmt = 1.0f - wetAmt * 0.7f;
    float peak = 0.0f;

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* dry = block.getChannelPointer (static_cast<size_t> (ch));
        const auto* wet = wetScratch.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
        {
            peak = juce::jmax (peak, std::abs (wet[i]));
            dry[i] = dry[i] * dryAmt + wet[i] * wetAmt;
        }
    }

    const float previous = wetLevel.load (std::memory_order_relaxed);
    wetLevel.store (previous * 0.8f + peak * 0.2f, std::memory_order_relaxed);
}
} // namespace WarlockAI

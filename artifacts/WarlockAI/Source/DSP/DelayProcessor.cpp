#include "DSP/DelayProcessor.h"
#include "DSP/BiquadAssign.h"

namespace WarlockAI
{
void DelayProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    const int maxDelay = juce::roundToInt (sampleRate * 2.0);

    auto oneChannel = spec;
    oneChannel.numChannels = 1;

    for (auto& line : lines)
    {
        line.setMaximumDelayInSamples (maxDelay);
        line.prepare (oneChannel);
        line.reset();
    }

    for (auto* bank : { &hp, &lp })
    {
        for (auto& filter : *bank)
        {
            filter.prepare (oneChannel);
            filter.reset();
        }
    }

    mixSmoother.reset (sampleRate, 0.02f, mix);
    timeSmoother.reset (sampleRate, 0.05f, timeMs);
    dirty = true;
    updateFilters();
}

void DelayProcessor::reset() noexcept
{
    for (auto& line : lines)
        line.reset();
    for (auto* bank : { &hp, &lp })
        for (auto& filter : *bank)
            filter.reset();
    wetLevel.store (0.0f, std::memory_order_relaxed);
}

void DelayProcessor::updateFilters() noexcept
{
    const float sr = static_cast<float> (sampleRate);
    assignIirBank (hp, ArrayCoeffs::makeHighPass (sr, clampAudioHz (lowCutHz, sr), 0.707f));
    assignIirBank (lp, ArrayCoeffs::makeLowPass  (sr, clampAudioHz (highCutHz, sr), 0.707f));
    dirty = false;
}

float DelayProcessor::syncedTimeMs() const noexcept
{
    switch (static_cast<DelaySync> (sync))
    {
        case DelaySync::Off:            return timeMs;
        case DelaySync::Quarter:        return 60000.0f / juce::jmax (20.0f, tempoBpm);
        case DelaySync::Eighth:         return (60000.0f / juce::jmax (20.0f, tempoBpm)) * 0.5f;
        case DelaySync::EighthDotted:   return (60000.0f / juce::jmax (20.0f, tempoBpm)) * 0.75f;
        case DelaySync::EighthTriplet:  return (60000.0f / juce::jmax (20.0f, tempoBpm)) / 3.0f;
        case DelaySync::Sixteenth:      return (60000.0f / juce::jmax (20.0f, tempoBpm)) * 0.25f;
        case DelaySync::NumModes:       return timeMs;
    }

    return timeMs;
}

void DelayProcessor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed || mix <= 0.0001f)
    {
        wetLevel.store (wetLevel.load (std::memory_order_relaxed) * 0.9f, std::memory_order_relaxed);
        return;
    }

    if (dirty)
        updateFilters();

    mixSmoother.setTarget (mix);
    timeSmoother.setTarget (syncedTimeMs());
    const float wetAmt = mixSmoother.getNext();
    const float timeNow = timeSmoother.getNext();
    mixSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));
    timeSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));

    const float delaySamples = juce::jlimit (1.0f,
                                             static_cast<float> (lines[0].getMaximumDelayInSamples() - 1),
                                             timeNow * 0.001f * static_cast<float> (sampleRate));
    const float delayR = delaySamples * (width > 1.0f ? 1.0f + 0.012f * (width - 1.0f) : 1.0f);

    lines[0].setDelay (delaySamples);
    lines[1].setDelay (delayR);

    const auto channels = juce::jmin (static_cast<int> (block.getNumChannels()), 2);
    const auto numSamples = block.getNumSamples();
    const float dryAmt = 1.0f - wetAmt;
    auto delayMode = static_cast<DelayMode> (mode);
    float peakWet = 0.0f;

    auto* left = block.getChannelPointer (0);
    auto* right = channels > 1 ? block.getChannelPointer (1) : left;

    for (size_t i = 0; i < numSamples; ++i)
    {
        float inL = left[i];
        float inR = right[i];

        if (delayMode == DelayMode::Mono || channels == 1)
        {
            const float mono = 0.5f * (inL + inR);
            float delayed = lines[0].popSample (0);
            delayed = hp[0].processSample (delayed);
            delayed = lp[0].processSample (delayed);
            lines[0].pushSample (0, mono + delayed * feedback);
            lines[1].pushSample (0, 0.0f);
            left[i] = inL * dryAmt + delayed * wetAmt;
            if (channels > 1)
                right[i] = inR * dryAmt + delayed * wetAmt;
            peakWet = juce::jmax (peakWet, std::abs (delayed));
        }
        else if (delayMode == DelayMode::PingPong && channels > 1)
        {
            float dL = lines[0].popSample (0);
            float dR = lines[1].popSample (0);
            dL = lp[0].processSample (hp[0].processSample (dL));
            dR = lp[1].processSample (hp[1].processSample (dR));
            lines[0].pushSample (0, inL + dR * feedback);
            lines[1].pushSample (0, inR + dL * feedback);
            left[i]  = inL * dryAmt + dL * wetAmt;
            right[i] = inR * dryAmt + dR * wetAmt;
            peakWet = juce::jmax (peakWet, std::abs (dL), std::abs (dR));
        }
        else
        {
            float dL = lines[0].popSample (0);
            float dR = channels > 1 ? lines[1].popSample (0) : dL;
            dL = lp[0].processSample (hp[0].processSample (dL));
            if (channels > 1)
                dR = lp[1].processSample (hp[1].processSample (dR));

            const float mid  = 0.5f * (dL + dR);
            const float side = 0.5f * (dL - dR) * width;
            const float wL = mid + side;
            const float wR = mid - side;

            lines[0].pushSample (0, inL + dL * feedback);
            if (channels > 1)
                lines[1].pushSample (0, inR + dR * feedback);

            left[i] = inL * dryAmt + wL * wetAmt;
            if (channels > 1)
                right[i] = inR * dryAmt + wR * wetAmt;
            peakWet = juce::jmax (peakWet, std::abs (wL), std::abs (wR));
        }
    }

    const float previous = wetLevel.load (std::memory_order_relaxed);
    wetLevel.store (previous * 0.8f + peakWet * 0.2f, std::memory_order_relaxed);
}
} // namespace WarlockAI

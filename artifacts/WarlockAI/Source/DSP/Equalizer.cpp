#include "DSP/Equalizer.h"
#include "DSP/BiquadAssign.h"

namespace WarlockAI
{
void Equalizer::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    dirty = true;

    auto filterSpec = spec;
    filterSpec.numChannels = 1;

    for (auto* bank : { &lowShelf, &lowMid, &highMid, &highShelf })
    {
        for (auto& filter : *bank)
        {
            filter.prepare (filterSpec);
            filter.reset();
        }
    }

    updateIfNeeded();
}

void Equalizer::reset() noexcept
{
    for (auto* bank : { &lowShelf, &lowMid, &highMid, &highShelf })
        for (auto& filter : *bank)
            filter.reset();
}

void Equalizer::setLowShelf (float gainDb, float freqHz) noexcept
{
    lowGain = gainDb;
    lowFreq = freqHz;
    dirty = true;
}

void Equalizer::setLowMid (float gainDb, float freqHz, float q) noexcept
{
    lowMidGain = gainDb;
    lowMidFreq = freqHz;
    lowMidQ = q;
    dirty = true;
}

void Equalizer::setHighMid (float gainDb, float freqHz, float q) noexcept
{
    highMidGain = gainDb;
    highMidFreq = freqHz;
    highMidQ = q;
    dirty = true;
}

void Equalizer::setHighShelf (float gainDb, float freqHz) noexcept
{
    highGain = gainDb;
    highFreq = freqHz;
    dirty = true;
}

void Equalizer::updateIfNeeded() noexcept
{
    if (! dirty)
        return;

    const auto sr = static_cast<float> (sampleRate);

    assignIirBank (lowShelf,  ArrayCoeffs::makeLowShelf  (sr, clampAudioHz (lowFreq, sr), 0.707f,
                                                          juce::Decibels::decibelsToGain (lowGain)));
    assignIirBank (lowMid,    ArrayCoeffs::makePeakFilter (sr, clampAudioHz (lowMidFreq, sr), juce::jmax (0.1f, lowMidQ),
                                                          juce::Decibels::decibelsToGain (lowMidGain)));
    assignIirBank (highMid,   ArrayCoeffs::makePeakFilter (sr, clampAudioHz (highMidFreq, sr), juce::jmax (0.1f, highMidQ),
                                                          juce::Decibels::decibelsToGain (highMidGain)));
    assignIirBank (highShelf, ArrayCoeffs::makeHighShelf (sr, clampAudioHz (highFreq, sr), 0.707f,
                                                          juce::Decibels::decibelsToGain (highGain)));

    dirty = false;
}

void Equalizer::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed)
        return;

    updateIfNeeded();

    const auto channels = juce::jmin (static_cast<int> (block.getNumChannels()), 2);

    for (int ch = 0; ch < channels; ++ch)
    {
        auto channelBlock = block.getSingleChannelBlock (static_cast<size_t> (ch));
        juce::dsp::ProcessContextReplacing<float> context (channelBlock);
        lowShelf[static_cast<size_t> (ch)].process (context);
        lowMid[static_cast<size_t> (ch)].process (context);
        highMid[static_cast<size_t> (ch)].process (context);
        highShelf[static_cast<size_t> (ch)].process (context);
    }
}
} // namespace WarlockAI

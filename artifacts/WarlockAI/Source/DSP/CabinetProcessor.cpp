#include "DSP/CabinetProcessor.h"
#include "DSP/BiquadAssign.h"

namespace WarlockAI
{
void CabinetProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    maximumBlockSize = spec.maximumBlockSize;
    numChannels = juce::jmax<juce::uint32> (1, spec.numChannels);

    mixSmoother.reset (sampleRate, 0.02f, mix);
    dryScratch.setSize (static_cast<int> (numChannels), static_cast<int> (maximumBlockSize), false, false, true);

    auto filterSpec = spec;
    filterSpec.numChannels = 1;
    for (auto* bank : { &lowCut, &highCut, &scoop, &presence })
    {
        for (auto& filter : *bank)
        {
            filter.prepare (filterSpec);
            filter.reset();
        }
    }

    convolution.prepare (spec);
    generateSyntheticImpulse();
    dirty = true;
    updateFilters();
}

void CabinetProcessor::reset() noexcept
{
    convolution.reset();
    for (auto* bank : { &lowCut, &highCut, &scoop, &presence })
        for (auto& filter : *bank)
            filter.reset();
}

int CabinetProcessor::getLatencySamples() const noexcept
{
    if (bypassed)
        return 0;

    return static_cast<int> (convolution.getLatency());
}

void CabinetProcessor::updateFilters() noexcept
{
    const float sr = static_cast<float> (sampleRate);

    assignIirBank (lowCut,   ArrayCoeffs::makeHighPass (sr, clampAudioHz (lowCutHz, sr), 0.707f));
    assignIirBank (highCut,  ArrayCoeffs::makeLowPass  (sr, clampAudioHz (highCutHz, sr), 0.707f));
    assignIirBank (scoop,    ArrayCoeffs::makePeakFilter (sr, 380.0f, 0.85f, juce::Decibels::decibelsToGain (-3.5f)));
    assignIirBank (presence, ArrayCoeffs::makePeakFilter (sr, 4200.0f, 1.1f, juce::Decibels::decibelsToGain (2.5f)));
    dirty = false;
}

void CabinetProcessor::generateSyntheticImpulse()
{
    constexpr int irLength = 2048;
    juce::AudioBuffer<float> ir (2, irLength);
    ir.clear();

    const float sr = static_cast<float> (sampleRate);
    for (int i = 0; i < irLength; ++i)
    {
        const float t = static_cast<float> (i) / sr;
        const float env = std::exp (-t * 48.0f);
        const float body =
            0.42f * std::sin (juce::MathConstants<float>::twoPi * 95.0f * t) * std::exp (-t * 28.0f)
          + 0.22f * std::sin (juce::MathConstants<float>::twoPi * 420.0f * t) * std::exp (-t * 55.0f)
          + 0.38f * std::sin (juce::MathConstants<float>::twoPi * 1850.0f * t) * std::exp (-t * 70.0f)
          + 0.16f * std::sin (juce::MathConstants<float>::twoPi * 4300.0f * t) * std::exp (-t * 110.0f);

        const float sample = env * body;
        ir.setSample (0, i, sample);
        ir.setSample (1, i, sample * 0.96f + (i > 11 ? ir.getSample (0, i - 11) * 0.08f : 0.0f));
    }

    ir.applyGain (0.9f / juce::jmax (0.001f, ir.getMagnitude (0, irLength)));

    convolution.loadImpulseResponse (std::move (ir),
                                     sampleRate,
                                     juce::dsp::Convolution::Stereo::yes,
                                     juce::dsp::Convolution::Trim::no,
                                     juce::dsp::Convolution::Normalise::yes);
    irReady = true;
}

bool CabinetProcessor::loadImpulseFile (const juce::File& file)
{
    if (! file.existsAsFile())
        return false;

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader (formats.createReaderFor (file));
    if (reader == nullptr)
        return false;

    juce::AudioBuffer<float> ir (static_cast<int> (reader->numChannels),
                                 static_cast<int> (reader->lengthInSamples));
    reader->read (&ir, 0, ir.getNumSamples(), 0, true, true);

    convolution.loadImpulseResponse (std::move (ir),
                                     reader->sampleRate,
                                     juce::dsp::Convolution::Stereo::yes,
                                     juce::dsp::Convolution::Trim::yes,
                                     juce::dsp::Convolution::Normalise::yes);
    irReady = true;
    return true;
}

void CabinetProcessor::clearLoadedImpulse()
{
    generateSyntheticImpulse();
}

void CabinetProcessor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed)
        return;

    if (dirty)
        updateFilters();

    mixSmoother.setTarget (mix);
    const float wetAmt = mixSmoother.getNext();
    mixSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));

    const auto channels = juce::jmin (static_cast<int> (block.getNumChannels()),
                                      juce::jmin (2, dryScratch.getNumChannels()));
    const auto numSamples = juce::jmin (static_cast<int> (block.getNumSamples()),
                                        dryScratch.getNumSamples());

    for (int ch = 0; ch < channels; ++ch)
        dryScratch.copyFrom (ch, 0, block.getChannelPointer (static_cast<size_t> (ch)), numSamples);

    if (! stereoCab && channels > 1)
    {
        auto* l = block.getChannelPointer (0);
        auto* r = block.getChannelPointer (1);
        for (int i = 0; i < numSamples; ++i)
        {
            const float m = 0.5f * (l[i] + r[i]);
            l[i] = m;
            r[i] = m;
        }
    }

    for (int ch = 0; ch < channels; ++ch)
    {
        auto channelBlock = block.getSingleChannelBlock (static_cast<size_t> (ch));
        juce::dsp::ProcessContextReplacing<float> context (channelBlock);
        lowCut[static_cast<size_t> (ch)].process (context);
        scoop[static_cast<size_t> (ch)].process (context);
        presence[static_cast<size_t> (ch)].process (context);
        highCut[static_cast<size_t> (ch)].process (context);
    }

    if (irReady && convolution.getCurrentIRSize() > 0)
    {
        juce::dsp::ProcessContextReplacing<float> context (block);
        convolution.process (context);
    }

    const float dryAmt = 1.0f - wetAmt;
    for (int ch = 0; ch < channels; ++ch)
    {
        auto* wet = block.getChannelPointer (static_cast<size_t> (ch));
        const auto* dry = dryScratch.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            wet[i] = dry[i] * dryAmt + wet[i] * wetAmt;
    }
}
} // namespace WarlockAI

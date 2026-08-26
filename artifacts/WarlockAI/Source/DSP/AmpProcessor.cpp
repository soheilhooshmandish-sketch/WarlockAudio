#include "DSP/AmpProcessor.h"
#include "DSP/BiquadAssign.h"

namespace WarlockAI
{
namespace
{
    float fastTanh (float x) noexcept
    {
        const float x2 = x * x;
        return x * (27.0f + x2) / (27.0f + 9.0f * x2);
    }

    float softClip (float x) noexcept
    {
        return x / (1.0f + std::abs (x));
    }
}

void AmpProcessor::setMode (int newMode) noexcept
{
    mode = juce::jlimit (0, static_cast<int> (AmpMode::NumModes) - 1, newMode);
    dirty = true;
}

void AmpProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    gainSmoother.reset (sampleRate, 0.03f, gain);
    masterSmoother.reset (sampleRate, 0.03f, juce::Decibels::decibelsToGain (masterDb));

    auto filterSpec = spec;
    filterSpec.numChannels = 1;

    for (auto* bank : { &tightHp, &bassFilter, &midFilter, &trebleFilter, &presenceFilter, &resonanceFilter })
    {
        for (auto& filter : *bank)
        {
            filter.prepare (filterSpec);
            filter.reset();
        }
    }

    lastMode = -1;
    dirty = true;
    updateToneStack();
}

void AmpProcessor::reset() noexcept
{
    for (auto* bank : { &tightHp, &bassFilter, &midFilter, &trebleFilter, &presenceFilter, &resonanceFilter })
        for (auto& filter : *bank)
            filter.reset();
}

void AmpProcessor::updateToneStack() noexcept
{
    const float sr = static_cast<float> (sampleRate);
    auto amp = static_cast<AmpMode> (mode);

    float hpHz = 40.0f;
    switch (amp)
    {
        case AmpMode::Clean:    hpHz = 30.0f;  break;
        case AmpMode::Crunch:   hpHz = 45.0f;  break;
        case AmpMode::Rock:     hpHz = 45.0f;  break;
        case AmpMode::Modern:   hpHz = 55.0f;  break;
        case AmpMode::Metal:    hpHz = 70.0f;  break;
        case AmpMode::Death:    hpHz = 90.0f;  break;
        case AmpMode::Black:    hpHz = 120.0f; break;
        case AmpMode::Djent:    hpHz = 110.0f; break;
        case AmpMode::NumModes: hpHz = 45.0f;  break;
    }

    const float midHz = amp == AmpMode::Death ? 800.0f : 650.0f;

    assignIirBank (tightHp, ArrayCoeffs::makeHighPass (sr, clampAudioHz (hpHz, sr), 0.707f));
    assignIirBank (bassFilter, ArrayCoeffs::makeLowShelf (sr, 120.0f, 0.7f, juce::Decibels::decibelsToGain (bassDb)));
    assignIirBank (midFilter, ArrayCoeffs::makePeakFilter (sr, clampAudioHz (midHz, sr), 0.9f,
                                                           juce::Decibels::decibelsToGain (midDb)));
    assignIirBank (trebleFilter, ArrayCoeffs::makeHighShelf (sr, 3200.0f, 0.7f, juce::Decibels::decibelsToGain (trebleDb)));
    assignIirBank (presenceFilter, ArrayCoeffs::makeHighShelf (sr, 5500.0f, 0.7f, juce::Decibels::decibelsToGain (presenceDb)));
    assignIirBank (resonanceFilter, ArrayCoeffs::makePeakFilter (sr, 95.0f, 1.8f,
                                                                juce::Decibels::decibelsToGain (resonance * 8.0f)));

    lastMode = mode;
    dirty = false;
}

float AmpProcessor::shape (float x, AmpMode amp, float driveGain) const noexcept
{
    float y = x * driveGain;

    switch (amp)
    {
        case AmpMode::Clean:
            y = fastTanh (y * 0.85f);
            y += 0.04f * y * y;
            break;
        case AmpMode::Crunch:
            y = fastTanh (y * 1.35f);
            y = y - 0.12f * y * y * y;
            break;
        case AmpMode::Rock:
            y = softClip (y * 1.6f);
            y = fastTanh (y * 1.15f);
            break;
        case AmpMode::Modern:
            y = fastTanh (y * 1.8f);
            y = juce::jlimit (-1.05f, 1.05f, y * 1.1f);
            break;
        case AmpMode::Metal:
        {
            const float pos = fastTanh (y * 2.1f);
            const float neg = softClip (y * 1.5f);
            y = y >= 0.0f ? pos : 0.65f * pos + 0.35f * neg;
            break;
        }
        case AmpMode::Death:
            y = juce::jlimit (-1.0f, 1.0f, y * 2.4f);
            y = fastTanh (y * 1.5f);
            break;
        case AmpMode::Black:
            y = fastTanh (y * 2.0f);
            y = y - 0.18f * y * y * y;
            break;
        case AmpMode::Djent:
            y = juce::jlimit (-0.95f, 0.95f, y * 2.6f);
            y = fastTanh (y * 1.4f);
            break;
        case AmpMode::NumModes:
            y = fastTanh (y);
            break;
    }

    return juce::jlimit (-1.5f, 1.5f, y);
}

void AmpProcessor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed)
        return;

    if (dirty || mode != lastMode)
        updateToneStack();

    gainSmoother.setTarget (gain);
    masterSmoother.setTarget (juce::Decibels::decibelsToGain (masterDb));

    const float driveNorm = gainSmoother.getNext();
    const float master = masterSmoother.getNext();
    gainSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));
    masterSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));

    const float driveGain = juce::Decibels::decibelsToGain (juce::jmap (driveNorm, 6.0f, 42.0f));
    auto amp = static_cast<AmpMode> (mode);
    const auto channels = juce::jmin (static_cast<int> (block.getNumChannels()), 2);
    const auto numSamples = block.getNumSamples();

    for (int ch = 0; ch < channels; ++ch)
    {
        auto channelBlock = block.getSingleChannelBlock (static_cast<size_t> (ch));
        juce::dsp::ProcessContextReplacing<float> context (channelBlock);

        tightHp[static_cast<size_t> (ch)].process (context);
        resonanceFilter[static_cast<size_t> (ch)].process (context);

        auto* data = block.getChannelPointer (static_cast<size_t> (ch));
        for (size_t i = 0; i < numSamples; ++i)
            data[i] = shape (data[i], amp, driveGain);

        bassFilter[static_cast<size_t> (ch)].process (context);
        midFilter[static_cast<size_t> (ch)].process (context);
        trebleFilter[static_cast<size_t> (ch)].process (context);
        presenceFilter[static_cast<size_t> (ch)].process (context);

        juce::FloatVectorOperations::multiply (data, master, static_cast<int> (numSamples));
    }
}
} // namespace WarlockAI

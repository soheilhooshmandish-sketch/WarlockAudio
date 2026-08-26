#include "DSP/DistortionProcessor.h"
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

    float hardClip (float x, float ceiling) noexcept
    {
        return juce::jlimit (-ceiling, ceiling, x);
    }

    float wavefold (float x) noexcept
    {
        const float wrapped = std::fmod (x + 1.0f, 4.0f);
        if (wrapped < 0.0f)
            return wavefold (x + 4.0f);

        if (wrapped < 2.0f)
            return wrapped - 1.0f;

        return 3.0f - wrapped;
    }

    bool paramChanged (float a, float b) noexcept
    {
        return std::abs (a - b) > 1.0e-6f;
    }
}

void DistortionProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    numChannels = static_cast<int> (juce::jmax<juce::uint32> (1, spec.numChannels));

    oversampler2x.initProcessing (spec.maximumBlockSize);
    oversampler4x.initProcessing (spec.maximumBlockSize);

    auto filterSpec = spec;
    filterSpec.numChannels = 1;
    for (auto* bank : { &tightnessFilters, &biteFilters, &toneFilters, &warmthFilters })
    {
        for (auto& filter : *bank)
        {
            filter.prepare (filterSpec);
            filter.reset();
        }
    }

    lastMode = -1;
    lastTightness = lastTone = lastWarmth = lastBite = -1.0f;
    driveSmoother.reset (sampleRate, 0.02f, settings.drive);
    satSmoother.reset (sampleRate, 0.02f, settings.saturation);
    outSmoother.reset (sampleRate, 0.02f, juce::Decibels::decibelsToGain (settings.outputDb));
    adaptiveDriveSmoother.reset (sampleRate, 0.08f, settings.drive);
    adaptiveTightSmoother.reset (sampleRate, 0.08f, settings.tightness);
    updateFilters (sampleRate);
    reset();
}

void DistortionProcessor::reset() noexcept
{
    oversampler2x.reset();
    oversampler4x.reset();

    for (auto* bank : { &tightnessFilters, &biteFilters, &toneFilters, &warmthFilters })
        for (auto& filter : *bank)
            filter.reset();
}

int DistortionProcessor::getLatencySamples() const noexcept
{
    if (settings.bypassed)
        return 0;

    if (settings.oversampleIndex >= 2)
        return static_cast<int> (oversampler4x.getLatencyInSamples());

    if (settings.oversampleIndex == 1)
        return static_cast<int> (oversampler2x.getLatencyInSamples());

    return 0;
}

void DistortionProcessor::updateFilters (double rate) noexcept
{
    const float sr = static_cast<float> (rate);

    const float tightHz = juce::jmap (filterTightness, 40.0f, 280.0f);
    const float biteHz  = juce::jmap (settings.bite, 2500.0f, 7000.0f);
    const float toneHz  = juce::jmap (settings.tone, 600.0f, 5500.0f);
    const float warmHz  = juce::jmap (1.0f - settings.warmth, 1800.0f, 9000.0f);

    assignIirBank (tightnessFilters, ArrayCoeffs::makeHighPass (sr, clampAudioHz (tightHz, sr), 0.707f));
    assignIirBank (biteFilters, ArrayCoeffs::makeHighShelf (sr, clampAudioHz (biteHz, sr), 0.707f,
                                                            juce::Decibels::decibelsToGain (juce::jmap (settings.bite, 0.0f, 8.0f))));
    assignIirBank (toneFilters, ArrayCoeffs::makeHighShelf (sr, clampAudioHz (toneHz, sr), 0.707f,
                                                            juce::Decibels::decibelsToGain (juce::jmap (settings.tone, -6.0f, 8.0f))));
    assignIirBank (warmthFilters, ArrayCoeffs::makeLowPass (sr, clampAudioHz (warmHz, sr), 0.707f));

    lastTightness = filterTightness;
    lastTone = settings.tone;
    lastWarmth = settings.warmth;
    lastBite = settings.bite;
    lastMode = settings.mode;
}

float DistortionProcessor::shapeSample (float x,
                                        DistortionMode mode,
                                        float gain,
                                        float sat,
                                        float warm,
                                        float biteAmt) const noexcept
{
    float y = x * gain;

    switch (mode)
    {
        case DistortionMode::Tube:
        {
            y += warm * 0.18f * y * y;
            y = fastTanh (y);
            y = juce::jmap (sat, y, fastTanh (y * 1.6f));
            break;
        }
        case DistortionMode::Vintage:
        {
            y = y - (1.0f / 3.0f) * y * y * y;
            y = softClip (y * (1.0f + sat));
            break;
        }
        case DistortionMode::Modern:
        {
            y = fastTanh (y * (1.2f + sat));
            y = juce::jmap (biteAmt, y, hardClip (y * 1.15f, 0.95f));
            break;
        }
        case DistortionMode::Metal:
        {
            const float pos = fastTanh (y * (1.6f + sat));
            const float neg = softClip (y * (1.2f + 0.4f * sat));
            y = y >= 0.0f ? pos : juce::jmap (0.35f + 0.4f * warm, pos, neg);
            break;
        }
        case DistortionMode::Djent:
        {
            y = hardClip (y * (2.0f + sat), 1.0f);
            y = fastTanh (y * 1.35f);
            y = y - 0.12f * y * y * y;
            break;
        }
        case DistortionMode::Extreme:
        {
            y = hardClip (y * (2.4f + sat * 1.5f), 1.15f);
            y = juce::jmap (sat, y, wavefold (y));
            y = fastTanh (y);
            break;
        }
        case DistortionMode::AdaptiveAI:
        {
            const float brightness = settings.adaptiveBrightness;
            const float density = settings.adaptiveDensity;
            const float picking = settings.adaptivePicking;
            const float tube = fastTanh (y * (1.1f + sat));
            const float metal = fastTanh (y * (1.7f + sat));
            const float djent = hardClip (y * (2.0f + sat), 1.0f);
            y = juce::jmap (density, tube, juce::jmap (brightness, metal, djent));
            y = juce::jmap (warm, y, y + 0.12f * y * y);
            y = juce::jmap (picking, y, juce::jmap (0.35f, y, hardClip (y * 1.08f, 1.0f)));
            break;
        }
        case DistortionMode::NumModes:
            y = fastTanh (y);
            break;
    }

    return juce::jlimit (-1.5f, 1.5f, y);
}

void DistortionProcessor::processNonlinear (juce::dsp::AudioBlock<float> block) noexcept
{
    const auto channels = juce::jmin (static_cast<int> (block.getNumChannels()), 2);
    const auto numSamples = block.getNumSamples();

    auto mode = static_cast<DistortionMode> (juce::jlimit (0, static_cast<int> (DistortionMode::NumModes) - 1, settings.mode));

    float targetDrive = settings.drive;
    float targetTight = settings.tightness;
    if (mode == DistortionMode::AdaptiveAI)
    {
        const float rmsTrim = juce::jlimit (-0.14f, 0.10f, (-18.0f - settings.adaptiveRmsDb) / 50.0f);
        const float dynTrim = (0.45f - settings.adaptiveDynamics) * 0.08f;
        const float peakTrim = settings.adaptivePeakDb > -0.8f ? -0.08f : 0.0f;
        const float pickBoost = (settings.adaptivePicking - 0.45f) * 0.06f;
        targetDrive = juce::jlimit (0.0f, 1.0f, settings.drive + rmsTrim + dynTrim + peakTrim + pickBoost);
        targetTight = juce::jlimit (0.0f, 1.0f, settings.tightness + (settings.adaptivePicking - 0.4f) * 0.12f
                                               + (1.0f - settings.adaptiveDynamics) * 0.08f);
    }

    driveSmoother.setTarget (targetDrive);
    adaptiveDriveSmoother.setTarget (targetDrive);
    adaptiveTightSmoother.setTarget (targetTight);
    satSmoother.setTarget (settings.saturation);
    outSmoother.setTarget (juce::Decibels::decibelsToGain (settings.outputDb));

    const float driveNorm = (mode == DistortionMode::AdaptiveAI ? adaptiveDriveSmoother.getNext()
                                                               : driveSmoother.getNext());
    const float sat = satSmoother.getNext();
    const float outGain = outSmoother.getNext();
    driveSmoother.skip (juce::jmax (0, static_cast<int> (numSamples) - 1));
    adaptiveDriveSmoother.skip (juce::jmax (0, static_cast<int> (numSamples) - 1));
    adaptiveTightSmoother.skip (juce::jmax (0, static_cast<int> (numSamples) - 1));
    satSmoother.skip (juce::jmax (0, static_cast<int> (numSamples) - 1));
    outSmoother.skip (juce::jmax (0, static_cast<int> (numSamples) - 1));

    const float driveGain = juce::Decibels::decibelsToGain (juce::jmap (driveNorm, 0.0f, 36.0f));
    const float warm = settings.warmth;
    const float biteAmt = settings.bite;

    for (int ch = 0; ch < channels; ++ch)
    {
        auto channelBlock = block.getSingleChannelBlock (static_cast<size_t> (ch));
        juce::dsp::ProcessContextReplacing<float> context (channelBlock);

        tightnessFilters[static_cast<size_t> (ch)].process (context);
        biteFilters[static_cast<size_t> (ch)].process (context);

        auto* data = block.getChannelPointer (static_cast<size_t> (ch));

        for (size_t i = 0; i < numSamples; ++i)
            data[i] = shapeSample (data[i], mode, driveGain, sat, warm, biteAmt);

        toneFilters[static_cast<size_t> (ch)].process (context);
        warmthFilters[static_cast<size_t> (ch)].process (context);

        juce::FloatVectorOperations::multiply (data, outGain, static_cast<int> (numSamples));
    }
}

void DistortionProcessor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (settings.bypassed)
        return;

    filterTightness = settings.tightness;
    if (settings.mode == static_cast<int> (DistortionMode::AdaptiveAI))
    {
        const float targetTight = juce::jlimit (0.0f, 1.0f, settings.tightness
            + (settings.adaptivePicking - 0.4f) * 0.12f
            + (1.0f - settings.adaptiveDynamics) * 0.08f);
        adaptiveTightSmoother.setTarget (targetTight);
        filterTightness = adaptiveTightSmoother.getNext();
    }

    const bool filtersDirty = paramChanged (filterTightness, lastTightness)
                           || paramChanged (settings.tone, lastTone)
                           || paramChanged (settings.warmth, lastWarmth)
                           || paramChanged (settings.bite, lastBite)
                           || settings.mode != lastMode;

    const int os = juce::jlimit (0, 2, settings.oversampleIndex);

    if (os <= 0)
    {
        if (filtersDirty)
            updateFilters (sampleRate);

        processNonlinear (block);
        return;
    }

    auto& oversampler = (os >= 2) ? oversampler4x : oversampler2x;
    auto osBlock = oversampler.processSamplesUp (block);

    if (filtersDirty)
        updateFilters (sampleRate * static_cast<double> (os == 2 ? 4 : 2));

    processNonlinear (osBlock);
    oversampler.processSamplesDown (block);
}
} // namespace WarlockAI

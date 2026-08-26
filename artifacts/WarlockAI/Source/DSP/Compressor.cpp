#include "DSP/Compressor.h"

namespace WarlockAI
{
void Compressor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;
    scHpf.prepare (monoSpec);
    scLpf.prepare (monoSpec);

    mixSmoother.reset (sampleRate, 0.02f, mix);
    makeupSmoother.reset (sampleRate, 0.02f, juce::Decibels::decibelsToGain (makeupDb));
    lastScHpf = lastScLpf = -1.0f;
    updateCoefficients();
    updateSidechainFilters();
    reset();
}

void Compressor::reset() noexcept
{
    envelope = 0.0f;
    releaseState = 0.0f;
    scHpf.reset();
    scLpf.reset();
    gainReductionDb.store (0.0f, std::memory_order_relaxed);
}

void Compressor::updateCoefficients() noexcept
{
    const auto sr = static_cast<float> (sampleRate);
    attackCoeff  = std::exp (-1.0f / juce::jmax (1.0f, attackMs * 0.001f * sr));
    releaseCoeff = std::exp (-1.0f / juce::jmax (1.0f, releaseMs * 0.001f * sr));
}

void Compressor::updateSidechainFilters() noexcept
{
    scHpfOn = scHpfHz >= 20.0f && scHpfHz < 2000.0f;
    scLpfOn = scLpfHz >= 1000.0f && scLpfHz < 18000.0f;

    if (std::abs (scHpfHz - lastScHpf) < 0.5f && std::abs (scLpfHz - lastScLpf) < 0.5f)
        return;

    lastScHpf = scHpfHz;
    lastScLpf = scLpfHz;

    if (scHpfOn)
        scHpf.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, scHpfHz, 0.707f);
    if (scLpfOn)
        scLpf.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass (sampleRate, scLpfHz, 0.707f);
}

float Compressor::computeGainDb (float levelDb) const noexcept
{
    const float halfKnee = kneeDb * 0.5f;
    const float overshoot = levelDb - thresholdDb;
    const float slope = 1.0f - (1.0f / ratio);

    if (overshoot <= -halfKnee)
        return 0.0f;

    if (overshoot >= halfKnee)
        return -overshoot * slope;

    const float x = overshoot + halfKnee;
    return -(slope * x * x) / (2.0f * juce::jmax (0.001f, kneeDb));
}

void Compressor::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed || block.getNumSamples() == 0)
    {
        gainReductionDb.store (0.0f, std::memory_order_relaxed);
        return;
    }

    updateCoefficients();
    updateSidechainFilters();
    mixSmoother.setTarget (mix);
    makeupSmoother.setTarget (juce::Decibels::decibelsToGain (makeupDb));

    const auto channels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();
    float maxGr = 0.0f;

    for (size_t i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;
        for (size_t ch = 0; ch < channels; ++ch)
            peak = juce::jmax (peak, std::abs (block.getSample (static_cast<int> (ch), static_cast<int> (i))));

        float detect = peak;
        if (scHpfOn)
            detect = std::abs (scHpf.processSample (detect));
        if (scLpfOn)
            detect = std::abs (scLpf.processSample (detect));

        // Log-domain gain computer, then decoupled ballistics on GR (JAES 2012).
        const float levelDb = juce::Decibels::gainToDecibels (juce::jmax (detect, 1.0e-8f), -100.0f);
        const float targetGr = -computeGainDb (levelDb); // magnitude, >= 0

        releaseState = juce::jmax (targetGr, releaseCoeff * releaseState + (1.0f - releaseCoeff) * targetGr);
        envelope = attackCoeff * envelope + (1.0f - attackCoeff) * releaseState;

        const float grDb = -envelope;
        maxGr = juce::jmax (maxGr, envelope);

        const float wetGain = juce::Decibels::decibelsToGain (grDb) * makeupSmoother.getNext();
        const float dryMix = 1.0f - mixSmoother.getNext();
        const float wetMix = 1.0f - dryMix;

        for (size_t ch = 0; ch < channels; ++ch)
        {
            const float in = block.getSample (static_cast<int> (ch), static_cast<int> (i));
            block.setSample (static_cast<int> (ch), static_cast<int> (i), in * dryMix + in * wetGain * wetMix);
        }
    }

    const float previous = gainReductionDb.load (std::memory_order_relaxed);
    gainReductionDb.store (previous * 0.7f + maxGr * 0.3f, std::memory_order_relaxed);
}
} // namespace WarlockAI

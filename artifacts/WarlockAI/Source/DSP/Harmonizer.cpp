#include "DSP/Harmonizer.h"

namespace WarlockAI
{
void Harmonizer::Voice::prepare (int size)
{
    buffer.assign (static_cast<size_t> (size), 0.0f);
    reset();
}

void Harmonizer::Voice::reset() noexcept
{
    std::fill (buffer.begin(), buffer.end(), 0.0f);
    write = 0;
    phase = 0.0f;
}

float Harmonizer::Voice::process (float input, float ratio) noexcept
{
    const int n = static_cast<int> (buffer.size());
    if (n <= 0)
        return input;

    buffer[static_cast<size_t> (write)] = input;

    const float grain = static_cast<float> (n / 2);
    float out = 0.0f;
    float windowSum = 0.0f;

    for (int g = 0; g < 2; ++g)
    {
        float grainPhase = phase + 0.5f * static_cast<float> (g);
        grainPhase -= std::floor (grainPhase);
        const float window = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * grainPhase);
        const float delay = grain - grainPhase * grain;
        float read = static_cast<float> (write) - delay * ratio;
        while (read < 0.0f)
            read += static_cast<float> (n);

        const int i0 = static_cast<int> (read) % n;
        const int i1 = (i0 + 1) % n;
        const float frac = read - std::floor (read);
        const float sample = buffer[static_cast<size_t> (i0)] * (1.0f - frac)
                           + buffer[static_cast<size_t> (i1)] * frac;
        out += sample * window;
        windowSum += window;
    }

    write = (write + 1) % n;
    phase += (1.0f / grain);
    if (phase >= 1.0f)
        phase -= 1.0f;

    return windowSum > 1.0e-6f ? out / windowSum : 0.0f;
}

void Harmonizer::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    mixSmoother.reset (sampleRate, 0.03f, mix);
    const int size = grainSize * 2;
    for (auto& voice : voices)
        voice.prepare (size);
}

void Harmonizer::reset() noexcept
{
    for (auto& voice : voices)
        voice.reset();
}

void Harmonizer::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed || mix <= 0.0001f)
        return;

    mixSmoother.setTarget (mix);
    const float wet = mixSmoother.getNext();
    mixSmoother.skip (juce::jmax (0, static_cast<int> (block.getNumSamples()) - 1));
    const float dryAmt = 1.0f - wet * 0.65f;

    const float ratio1 = std::pow (2.0f, harmonyIntervalSemitones (interval1) / 12.0f);
    const float ratio2 = std::pow (2.0f, (harmonyIntervalSemitones (interval2) + detuneCents / 100.0f) / 12.0f);

    const float pan1L = pan1 <= 0.0f ? 1.0f : 1.0f - pan1;
    const float pan1R = pan1 >= 0.0f ? 1.0f : 1.0f + pan1;
    const float pan2L = pan2 <= 0.0f ? 1.0f : 1.0f - pan2;
    const float pan2R = pan2 >= 0.0f ? 1.0f : 1.0f + pan2;

    const auto channels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();
    auto* left = block.getChannelPointer (0);
    auto* right = channels > 1 ? block.getChannelPointer (1) : left;

    for (size_t i = 0; i < numSamples; ++i)
    {
        const float inL = left[i];
        const float inR = right[i];
        const float mono = 0.5f * (inL + inR);

        const float v1 = voices[0].process (mono, ratio1);
        const float v2 = voices[1].process (mono, ratio2);

        left[i]  = inL * dryAmt + (v1 * pan1L + v2 * pan2L) * wet;
        if (channels > 1)
            right[i] = inR * dryAmt + (v1 * pan1R + v2 * pan2R) * wet;
    }
}
} // namespace WarlockAI

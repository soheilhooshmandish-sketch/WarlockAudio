#include "DSP/Limiter.h"

namespace WarlockAI
{
void Limiter::prepare (const juce::dsp::ProcessSpec& spec)
{
    limiter.prepare (spec);
    reset();
}

void Limiter::reset() noexcept
{
    limiter.reset();
}

void Limiter::process (juce::dsp::AudioBlock<float> block) noexcept
{
    if (bypassed)
        return;

    limiter.setThreshold (ceilingDb);
    limiter.setRelease (releaseMs);

    juce::dsp::ProcessContextReplacing<float> context (block);
    limiter.process (context);
}
} // namespace WarlockAI

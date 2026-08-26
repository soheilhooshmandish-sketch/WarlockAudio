#pragma once

#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
using IirFilter = juce::dsp::IIR::Filter<float>;
using ArrayCoeffs = juce::dsp::IIR::ArrayCoefficients<float>;

inline float clampAudioHz (float hz, float sampleRate) noexcept
{
    const float nyquist = sampleRate * 0.45f;
    return juce::jlimit (20.0f, juce::jmax (20.0f, nyquist), hz);
}

// Assigns RBJ coefficients into an already-prepared IIR filter.
// ArrayCoefficients builds the six floats on the stack; the Filter's
// Coefficients object is mutated in place so this is allocation-free
// after prepare() has run once.
inline void assignIir (IirFilter& filter, const std::array<float, 6>& values) noexcept
{
    if (auto* coeffs = filter.coefficients.get())
        *coeffs = values;
}

inline void assignIirBank (std::array<IirFilter, 2>& bank, const std::array<float, 6>& values) noexcept
{
    for (auto& filter : bank)
        assignIir (filter, values);
}
} // namespace WarlockAI

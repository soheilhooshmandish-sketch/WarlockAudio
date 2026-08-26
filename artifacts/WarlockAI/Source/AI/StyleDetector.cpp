#include "AI/StyleDetector.h"

namespace WarlockAI
{
StyleDetectionResult StyleDetector::detect (const AudioFeatures& features,
                                            const ToneAnalysisResult& analysis) const
{
    StyleDetectionResult result;

    if (! features.valid || features.rmsDb < -50.0f)
    {
        result.style = GuitarStyle::Ambient;
        result.confidence = 0.15f;
        result.label = guitarStyleName (result.style);
        result.rationale = "Signal is too quiet for reliable style detection.";
        return result;
    }

    auto& s = result.scores;
    const float dense = features.density;
    const float bright = analysis.brightness;
    const float dark = 1.0f - bright;
    const float dyn = analysis.dynamicRange;
    const float tight = analysis.tightness;
    const float trans = features.transientStrength;
    const float sustain = features.sustain;
    const float mud = analysis.muddiness;
    const float harsh = analysis.harshness;
    const float zcr = juce::jlimit (0.0f, 1.0f, features.zeroCrossingRate * 8.0f);
    const float sat = analysis.saturationEstimate;

    auto bump = [&] (GuitarStyle st, float v)
    {
        s[static_cast<size_t> (st)] += juce::jmax (0.0f, v);
    };

    bump (GuitarStyle::Clean,            (1.0f - sat) * 0.7f + dyn * 0.4f - dense * 0.5f);
    bump (GuitarStyle::Ambient,          sustain * 0.5f + (1.0f - trans) * 0.4f + (1.0f - zcr) * 0.2f - dense * 0.4f);
    bump (GuitarStyle::HardRock,         (1.0f - std::abs (sat - 0.4f)) * 0.5f + dyn * 0.3f + (1.0f - tight) * 0.15f);
    bump (GuitarStyle::HeavyMetal,       sat * 0.35f + analysis.midEnergy * 0.4f + (1.0f - std::abs (bright - 0.45f)) * 0.25f);
    bump (GuitarStyle::ThrashMetal,      trans * 0.45f + zcr * 0.35f + bright * 0.2f + tight * 0.15f);
    bump (GuitarStyle::DeathMetal,       dark * 0.45f + sat * 0.35f + mud * 0.2f + (1.0f - bright) * 0.15f);
    bump (GuitarStyle::BlackMetal,       bright * 0.4f + harsh * 0.3f + zcr * 0.25f + (1.0f - analysis.lowEnergy) * 0.2f);
    bump (GuitarStyle::ModernMetal,      tight * 0.4f + sat * 0.3f + analysis.highEnergy * 0.2f + (1.0f - mud) * 0.2f);
    bump (GuitarStyle::Djent,            tight * 0.5f + trans * 0.25f + sat * 0.2f + (1.0f - analysis.lowEnergy) * 0.25f - dyn * 0.1f);
    bump (GuitarStyle::Metalcore,        sat * 0.3f + analysis.highEnergy * 0.3f + trans * 0.25f + tight * 0.2f);
    bump (GuitarStyle::ProgressiveMetal, dyn * 0.45f + analysis.midEnergy * 0.3f + (1.0f - std::abs (sat - 0.55f)) * 0.2f);
    bump (GuitarStyle::DoomMetal,        dark * 0.4f + sustain * 0.35f + (1.0f - trans) * 0.3f + analysis.lowEnergy * 0.25f);

    float best = -1.0f;
    float sum = 0.0f;
    int winner = 0;
    for (int i = 0; i < static_cast<int> (GuitarStyle::NumStyles); ++i)
    {
        sum += s[static_cast<size_t> (i)];
        if (s[static_cast<size_t> (i)] > best)
        {
            best = s[static_cast<size_t> (i)];
            winner = i;
        }
    }

    result.style = static_cast<GuitarStyle> (winner);
    result.label = guitarStyleName (result.style);
    const float share = sum > 1.0e-4f ? best / sum : 0.0f;
    result.confidence = juce::jlimit (0.12f, 0.92f, share * 0.85f + 0.15f * analysis.confidence);
    result.rationale
        << "Heuristic match from brightness " << juce::String (bright, 2)
        << ", tightness " << juce::String (tight, 2)
        << ", saturation " << juce::String (sat, 2)
        << ". This is an estimate, not a genre classifier.";
    return result;
}
} // namespace WarlockAI

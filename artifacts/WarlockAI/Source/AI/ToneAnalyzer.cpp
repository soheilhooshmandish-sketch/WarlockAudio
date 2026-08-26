#include "AI/ToneAnalyzer.h"

namespace WarlockAI
{
ToneAnalysisResult ToneAnalyzer::analyseDetailed (const AudioFeatures& features) const
{
    ToneAnalysisResult r;

    if (! features.valid)
        return r;

    r.inputLevel = juce::jlimit (0.0f, 1.0f, (features.rmsDb + 60.0f) / 60.0f);
    r.noiseLevel = juce::jlimit (0.0f, 1.0f, (features.noiseFloorDb + 90.0f) / 50.0f);
    r.brightness = features.brightness;
    r.warmth = juce::jlimit (0.0f, 1.0f, features.lowMidEnergy * 0.55f + (1.0f - features.brightness) * 0.45f);
    r.muddiness = features.muddiness;
    r.harshness = features.harshness;
    r.tightness = juce::jlimit (0.0f, 1.0f, (1.0f - features.lowEnergy) * 0.45f + features.transientStrength * 0.35f
                                           + (1.0f - features.muddiness) * 0.2f);
    r.dynamicRange = juce::jlimit (0.0f, 1.0f, features.dynamicRangeDb / 30.0f);
    r.saturationEstimate = juce::jlimit (0.0f, 1.0f, features.density * 0.7f + (1.0f - juce::jlimit (0.0f, 1.0f, features.crestFactorDb / 16.0f)) * 0.3f);
    r.lowEnergy = juce::jlimit (0.0f, 1.0f, features.lowEnergy + features.lowMidEnergy);
    r.midEnergy = features.midEnergy;
    r.highEnergy = juce::jlimit (0.0f, 1.0f, features.highMidEnergy + features.highEnergy);
    r.inputLevelConfidence = features.rmsDb > -48.0f ? 0.9f : 0.35f;
    r.spectralConfidence = features.spectralCentroidHz > 80.0f ? 0.85f : 0.4f;
    r.confidence = juce::jlimit (0.0f, 1.0f, 0.5f * r.inputLevelConfidence + 0.5f * r.spectralConfidence);
    return r;
}

ToneAnalysis ToneAnalyzer::analyse (const AudioFeatures& features) const
{
    ToneAnalysis result;
    result.features = features;

    if (! features.valid)
    {
        result.summary = "No valid audio captured for analysis.";
        return result;
    }

    const float bright = features.brightness;
    const float dense = features.density;
    const float zcr = features.zeroCrossingRate;
    const float dyn = juce::jlimit (0.0f, 1.0f, features.dynamicRangeDb / 30.0f);
    const float rmsDb = features.rmsDb;

    struct Candidate { ToneClass type; float score; };
    Candidate best { ToneClass::Unknown, -1.0f };

    auto consider = [&] (ToneClass type, float score)
    {
        if (score > best.score)
            best = { type, score };
    };

    consider (ToneClass::Clean,            (rmsDb < -28.0f ? 0.7f : 0.2f) + dyn * 0.3f - dense * 0.3f);
    consider (ToneClass::Crunch,           (rmsDb > -28.0f && rmsDb < -16.0f ? 0.6f : 0.15f) + (1.0f - dense) * 0.2f);
    consider (ToneClass::TightHighGain,    dense * 0.45f + (1.0f - dyn) * 0.25f + (zcr > 0.08f ? 0.2f : 0.0f) + features.transientStrength * 0.15f);
    consider (ToneClass::OpenHighGain,     dense * 0.35f + dyn * 0.25f + (1.0f - bright) * 0.1f);
    consider (ToneClass::DarkSaturated,    dense * 0.4f + (1.0f - bright) * 0.45f + features.muddiness * 0.15f);
    consider (ToneClass::BrightAggressive, bright * 0.5f + dense * 0.25f + zcr * 2.0f + features.harshness * 0.15f);

    result.classification = best.type;
    result.confidence = juce::jlimit (0.0f, 1.0f, best.score);

    result.summary
        << toneClassName (result.classification)
        << "  |  centroid " << juce::String (features.spectralCentroidHz, 0) << " Hz"
        << "  |  RMS " << juce::String (features.rmsDb, 1) << " dB"
        << "  |  crest " << juce::String (features.crestFactorDb, 1) << " dB";

    return result;
}
} // namespace WarlockAI

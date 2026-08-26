#include "AI/ToneDesigner.h"

namespace WarlockAI
{
void ToneDesigner::clampRecommendation (ToneRecommendation& rec) noexcept
{
    rec.drive = juce::jlimit (0.0f, 1.0f, rec.drive);
    rec.tone = juce::jlimit (0.0f, 1.0f, rec.tone);
    rec.tightness = juce::jlimit (0.0f, 1.0f, rec.tightness);
    rec.saturation = juce::jlimit (0.0f, 1.0f, rec.saturation);
    rec.warmth = juce::jlimit (0.0f, 1.0f, rec.warmth);
    rec.bite = juce::jlimit (0.0f, 1.0f, rec.bite);
    rec.gateThreshold = juce::jlimit (-80.0f, 0.0f, rec.gateThreshold);
    rec.bass = juce::jlimit (-12.0f, 12.0f, rec.bass);
    rec.mid = juce::jlimit (-12.0f, 12.0f, rec.mid);
    rec.treble = juce::jlimit (-12.0f, 12.0f, rec.treble);
    rec.presence = juce::jlimit (-12.0f, 12.0f, rec.presence);
    rec.ampGain = juce::jlimit (0.0f, 1.0f, rec.ampGain);
    rec.ampBass = juce::jlimit (-12.0f, 12.0f, rec.ampBass);
    rec.ampMid = juce::jlimit (-12.0f, 12.0f, rec.ampMid);
    rec.ampTreble = juce::jlimit (-12.0f, 12.0f, rec.ampTreble);
    rec.ampPresence = juce::jlimit (-12.0f, 12.0f, rec.ampPresence);
    rec.cabLowCut = juce::jlimit (40.0f, 200.0f, rec.cabLowCut);
    rec.cabHighCut = juce::jlimit (3000.0f, 12000.0f, rec.cabHighCut);
    rec.compressorThreshold = juce::jlimit (-40.0f, 0.0f, rec.compressorThreshold);
    rec.compressorRatio = juce::jlimit (1.0f, 20.0f, rec.compressorRatio);
    rec.confidence = juce::jlimit (0.0f, 1.0f, rec.confidence);
}

std::map<juce::String, float> ToneDesigner::toParameterMap (const ToneRecommendation& rec)
{
    std::map<juce::String, float> m;
    m[ParamID::drive] = rec.drive;
    m[ParamID::tone] = rec.tone;
    m[ParamID::tightness] = rec.tightness;
    m[ParamID::saturation] = rec.saturation;
    m[ParamID::warmth] = rec.warmth;
    m[ParamID::bite] = rec.bite;
    m[ParamID::gateThreshold] = rec.gateThreshold;
    m[ParamID::eqLowGain] = rec.bass;
    m[ParamID::eqLowMidGain] = rec.mid;
    m[ParamID::eqHighMidGain] = rec.presence;
    m[ParamID::eqHighGain] = rec.treble;
    m[ParamID::ampGain] = rec.ampGain;
    m[ParamID::ampBass] = rec.ampBass;
    m[ParamID::ampMid] = rec.ampMid;
    m[ParamID::ampTreble] = rec.ampTreble;
    m[ParamID::ampPresence] = rec.ampPresence;
    m[ParamID::cabLowCut] = rec.cabLowCut;
    m[ParamID::cabHighCut] = rec.cabHighCut;
    m[ParamID::compThreshold] = rec.compressorThreshold;
    m[ParamID::compRatio] = rec.compressorRatio;
    return m;
}

ToneRecommendation ToneDesigner::recommend (const AudioFeatures& features,
                                            const ToneAnalysisResult& analysis,
                                            const StyleDetectionResult& style,
                                            const CurrentToneState& current) const
{
    ToneRecommendation rec;
    rec.drive = current.drive;
    rec.tone = current.tone;
    rec.tightness = current.tightness;
    rec.saturation = current.saturation;
    rec.warmth = current.warmth;
    rec.bite = current.bite;
    rec.gateThreshold = current.gateThreshold;
    rec.bass = current.bass;
    rec.mid = current.mid;
    rec.treble = current.treble;
    rec.presence = current.presence;
    rec.ampGain = current.ampGain;
    rec.ampBass = current.ampBass;
    rec.ampMid = current.ampMid;
    rec.ampTreble = current.ampTreble;
    rec.ampPresence = current.ampPresence;
    rec.cabLowCut = current.cabLowCut;
    rec.cabHighCut = current.cabHighCut;
    rec.compressorThreshold = current.compressorThreshold;
    rec.compressorRatio = current.compressorRatio;

    if (! features.valid)
    {
        rec.reason = "No analysis data available.";
        rec.confidence = 0.0f;
        clampRecommendation (rec);
        rec.parameterValues = toParameterMap (rec);
        return rec;
    }

    auto explainDelta = [&] (const juce::String& name, float before, float after,
                             const juce::String& unit, const juce::String& because)
    {
        const float d = after - before;
        if (std::abs (d) < 0.015f && unit != " Hz")
            return;
        if (unit == " Hz" && std::abs (d) < 4.0f)
            return;

        juce::String line;
        if (unit == " %")
        {
            line << (d < 0.0f ? "Reduced " : "Added ") << name << " by "
                 << juce::String (std::abs (d) * 100.0f, 0) << "% because " << because;
        }
        else
        {
            line << (d < 0.0f ? "Reduced " : "Raised ") << name << " by "
                 << juce::String (std::abs (d), unit == " Hz" ? 0 : 1) << unit
                 << " because " << because;
        }
        rec.explanations.push_back (line);
    };

    if (analysis.saturationEstimate > 0.72f && analysis.dynamicRange < 0.35f)
    {
        rec.drive = current.drive - 0.08f;
        rec.ampGain = current.ampGain - 0.06f;
        explainDelta ("Drive", current.drive, rec.drive, " %",
                      "the input has high saturation and reduced dynamic range.");
    }
    else if (features.rmsDb < -28.0f)
    {
        rec.drive = current.drive + 0.07f;
        explainDelta ("Drive", current.drive, rec.drive, " %",
                      "RMS is low (" + juce::String (features.rmsDb, 1) + " dB) and the capture needs more gain.");
    }

    if (analysis.muddiness > 0.55f)
    {
        rec.tightness = juce::jmax (current.tightness, 0.62f);
        rec.mid = current.mid - 1.8f;
        rec.ampMid = current.ampMid - 1.2f;
        rec.cabLowCut = juce::jmax (current.cabLowCut, 90.0f);
        rec.explanations.push_back ("Reduced 250 Hz energy to improve riff clarity.");
    }

    if (analysis.harshness > 0.55f)
    {
        rec.bite = current.bite - 0.1f;
        rec.tone = current.tone - 0.08f;
        rec.ampPresence = current.ampPresence - 1.5f;
        rec.cabHighCut = juce::jmin (current.cabHighCut, 7500.0f);
        rec.explanations.push_back ("Reduced Bite and Presence because measured high-mid energy is harsh.");
    }
    else if (analysis.brightness < 0.32f)
    {
        rec.tone = current.tone + 0.08f;
        rec.presence = current.presence + 1.4f;
        rec.ampPresence = current.ampPresence + 1.2f;
        rec.explanations.push_back ("Added Presence to improve pick definition.");
    }

    if (features.noiseFloorDb > -55.0f && (features.rmsDb - features.noiseFloorDb) < 18.0f)
    {
        rec.gateThreshold = juce::jmax (current.gateThreshold, -42.0f);
        explainDelta ("Gate threshold", current.gateThreshold, rec.gateThreshold, " dB",
                      "the noise floor is close to the guitar signal.");
    }

    if (features.crestFactorDb < 5.0f)
    {
        rec.compressorRatio = juce::jmin (current.compressorRatio, 2.4f);
        rec.compressorThreshold = juce::jmin (current.compressorThreshold, -22.0f);
        rec.explanations.push_back ("Lowered compressor ratio because crest factor is already low.");
    }

    switch (style.style)
    {
        case GuitarStyle::Djent:
            rec.tightness = juce::jmax (rec.tightness, 0.72f);
            rec.ampMid = juce::jmin (rec.ampMid, -1.5f);
            rec.cabLowCut = juce::jmax (rec.cabLowCut, 100.0f);
            rec.explanations.push_back ("Djent-leaning capture: tightness and low-cut raised for palm-mute definition.");
            break;
        case GuitarStyle::DeathMetal:
            rec.warmth = juce::jmax (rec.warmth, 0.4f);
            rec.tone = juce::jmin (rec.tone, 0.48f);
            rec.ampBass = juce::jmax (rec.ampBass, 1.0f);
            break;
        case GuitarStyle::BlackMetal:
            rec.tone = juce::jmax (rec.tone, 0.62f);
            rec.bite = juce::jmax (rec.bite, 0.3f);
            rec.ampBass = juce::jmin (rec.ampBass, 0.0f);
            break;
        case GuitarStyle::Clean:
            rec.drive = juce::jmin (rec.drive, 0.28f);
            rec.ampGain = juce::jmin (rec.ampGain, 0.35f);
            rec.saturation = juce::jmin (rec.saturation, 0.22f);
            break;
        case GuitarStyle::DoomMetal:
            rec.warmth = juce::jmax (rec.warmth, 0.5f);
            rec.tightness = juce::jmin (rec.tightness, 0.4f);
            rec.cabLowCut = juce::jmin (rec.cabLowCut, 70.0f);
            break;
        case GuitarStyle::ModernMetal:
        case GuitarStyle::ThrashMetal:
        case GuitarStyle::Metalcore:
        case GuitarStyle::ProgressiveMetal:
        case GuitarStyle::HeavyMetal:
        case GuitarStyle::HardRock:
        case GuitarStyle::Ambient:
        case GuitarStyle::NumStyles:
            break;
    }

    rec.drive = current.drive + juce::jlimit (-0.16f, 0.16f, rec.drive - current.drive);
    rec.tone = current.tone + juce::jlimit (-0.16f, 0.16f, rec.tone - current.tone);
    rec.ampGain = current.ampGain + juce::jlimit (-0.14f, 0.14f, rec.ampGain - current.ampGain);

    clampRecommendation (rec);
    rec.parameterValues = toParameterMap (rec);
    rec.confidence = juce::jlimit (0.2f, 0.9f, 0.55f * analysis.confidence + 0.35f * style.confidence);
    rec.reason
        << "Rule-based tone design from "
        << juce::String (features.spectralCentroidHz, 0) << " Hz centroid, RMS "
        << juce::String (features.rmsDb, 1) << " dB, style guess "
        << style.label << " (" << juce::String (style.confidence * 100.0f, 0) << "%).";

    if (rec.explanations.empty())
        rec.explanations.push_back ("Measured tone is already inside a healthy range; recommendations stay conservative.");

    return rec;
}
} // namespace WarlockAI

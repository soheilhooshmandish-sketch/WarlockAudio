#include "AI/MixFit.h"
#include "AI/ToneHealth.h"

namespace WarlockAI
{
MixFitSuggestion MixFit::suggest (const AudioFeatures& features, DistortionMode currentMode) const
{
    CurrentToneState current;
    ToneAnalyzer analyzer;
    const auto analysis = analyzer.analyseDetailed (features);
    ToneHealth health;
    const auto healthResult = health.evaluateDetailed (features, analysis);
    return fit (features, analysis, healthResult.score, current, currentMode).suggestion;
}

MixFitResult MixFit::fit (const AudioFeatures& features,
                          const ToneAnalysisResult& analysis,
                          float healthScore,
                          const CurrentToneState& current,
                          DistortionMode currentMode) const
{
    MixFitResult result;
    result.beforeScore = healthScore;

    MixFitSuggestion suggestion;
    if (! features.valid)
    {
        suggestion.rationale = "No analysis data available.";
        result.suggestion = suggestion;
        result.summary = suggestion.rationale;
        result.afterEstimatedScore = healthScore;
        return result;
    }

    float drive = current.drive;
    float tone = current.tone;
    float tightness = current.tightness;
    float eqLow = current.bass;
    float eqLowMid = current.mid;
    float eqHighMid = current.presence;
    float eqHigh = current.treble;
    float cabLow = current.cabLowCut;
    float cabHigh = current.cabHighCut;
    float width = 1.0f;

    auto push = [&] (const juce::String& id, const juce::String& name,
                     float before, float after, const juce::String& why)
    {
        if (std::abs (after - before) < 0.01f)
            return;
        MixFitChange c;
        c.parameterId = id;
        c.name = name;
        c.before = before;
        c.after = after;
        c.explanation = why;
        result.changes.push_back (std::move (c));
        result.recommendations.push_back (why);
        suggestion.parameterValues[id] = after;
    };

    if (analysis.lowEnergy > 0.38f || features.lowEnergy > 0.22f)
    {
        const float next = juce::jlimit (40.0f, 140.0f, juce::jmax (cabLow, 95.0f));
        push (ParamID::cabLowCut, "Cab low cut", cabLow, next,
              "Raised low cut to clear rumble below the guitar's useful range.");
        cabLow = next;
        const float lowEq = juce::jlimit (-4.0f, 0.0f, eqLow - 1.5f);
        push (ParamID::eqLowGain, "Low shelf", eqLow, lowEq,
              "Trimmed the low shelf so the mix has room for bass.");
        eqLow = lowEq;
    }

    if (analysis.muddiness > 0.5f)
    {
        const float scoop = juce::jlimit (-4.0f, 0.0f, eqLowMid - 2.2f);
        push (ParamID::eqLowMidGain, "Low-mid", eqLowMid, scoop,
              "Reduced 250 Hz energy to improve riff clarity.");
        eqLowMid = scoop;
        tightness = juce::jlimit (0.0f, 1.0f, juce::jmax (tightness, 0.58f));
        push (ParamID::tightness, "Tightness", current.tightness, tightness,
              "Raised tightness to control low-mid smear on palm mutes.");
    }

    if (analysis.midEnergy < 0.16f)
    {
        const float boost = juce::jlimit (0.0f, 3.0f, eqHighMid + 1.8f);
        push (ParamID::eqHighMidGain, "High-mid / presence", eqHighMid, boost,
              "Added Presence to improve pick definition.");
        eqHighMid = boost;
    }

    if (analysis.harshness > 0.55f || features.highEnergy > 0.32f)
    {
        const float nextHigh = juce::jlimit (4500.0f, 10000.0f, juce::jmin (cabHigh, 8000.0f));
        push (ParamID::cabHighCut, "Cab high cut", cabHigh, nextHigh,
              "Lowered high cut because measured treble energy is excessive.");
        const float t = juce::jlimit (0.0f, 1.0f, tone - 0.08f);
        push (ParamID::tone, "Tone", current.tone, t,
              "Darkened Tone to keep the guitar mix-ready without fizz.");
        tone = t;
    }
    else if (analysis.brightness < 0.3f)
    {
        const float t = juce::jlimit (0.0f, 1.0f, tone + 0.07f);
        push (ParamID::tone, "Tone", current.tone, t,
              "Opened Tone slightly because the capture measured dark.");
        tone = t;
    }

    if (analysis.saturationEstimate > 0.75f)
    {
        const float d = juce::jlimit (0.0f, 1.0f, drive - 0.08f);
        push (ParamID::drive, "Drive", current.drive, d,
              "Reduced Drive by 8% because the input has high saturation and reduced dynamic range.");
        drive = d;
    }

    if (features.crestFactorDb > 12.0f && analysis.midEnergy > 0.18f)
        width = 1.05f;
    else if (analysis.harshness > 0.5f)
        width = 0.92f;
    push (ParamID::stereoWidth, "Stereo width", 1.0f, width,
          width < 1.0f ? "Narrowed width slightly so harsh highs do not spray the sides."
                       : "Kept width near unity to preserve the recorded character.");

    switch (currentMode)
    {
        case DistortionMode::Tube:
        case DistortionMode::Modern:
        case DistortionMode::Metal:
        case DistortionMode::AdaptiveAI:
        case DistortionMode::NumModes:
            break;
        case DistortionMode::Djent:
            tightness = juce::jmax (tightness, 0.7f);
            push (ParamID::tightness, "Tightness", current.tightness, tightness,
                  "Djent mode: kept tightness high for percussive mutes.");
            break;
        case DistortionMode::Vintage:
            push (ParamID::warmth, "Warmth", current.warmth, juce::jmax (current.warmth, 0.45f),
                  "Vintage mode: warmth held up so the mix-fit does not bleach the tone.");
            break;
        case DistortionMode::Extreme:
            drive = juce::jmin (drive, 0.7f);
            push (ParamID::drive, "Drive", current.drive, drive,
                  "Extreme mode: drive capped so mix-fit does not pile more saturation.");
            break;
    }

    suggestion.parameterValues[ParamID::drive] = juce::jlimit (0.0f, 1.0f, drive);
    suggestion.parameterValues[ParamID::tone] = juce::jlimit (0.0f, 1.0f, tone);
    suggestion.parameterValues[ParamID::tightness] = juce::jlimit (0.0f, 1.0f, tightness);
    suggestion.rationale
        << "Mix Fit (preview) from centroid "
        << juce::String (features.spectralCentroidHz, 0)
        << " Hz and RMS "
        << juce::String (features.rmsDb, 1)
        << " dB. Changes are conservative so the captured character is preserved.";

    result.suggestion = suggestion;
    result.summary = suggestion.rationale;
    const float lift = juce::jmin (18.0f, 3.5f * static_cast<float> (result.changes.size()));
    result.afterEstimatedScore = juce::jlimit (0.0f, 100.0f, result.beforeScore + lift);
    return result;
}
} // namespace WarlockAI

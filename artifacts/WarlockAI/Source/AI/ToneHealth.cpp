#include "AI/ToneHealth.h"

namespace WarlockAI
{
namespace
{
    void addIssue (ToneHealthResult& result, HealthProblem problem, float severity,
                   const juce::String& description, const juce::String& recommendation)
    {
        if (severity < 0.28f)
            return;

        HealthIssue issue;
        issue.problem = problem;
        issue.severity = juce::jlimit (0.0f, 1.0f, severity);
        issue.description = description;
        issue.recommendation = recommendation;
        result.problems.push_back (std::move (issue));
    }
}

ToneHealthResult ToneHealth::evaluateDetailed (const AudioFeatures& features,
                                               const ToneAnalysisResult& analysis) const
{
    ToneHealthResult result;

    if (! features.valid)
    {
        result.grade = "Idle";
        HealthIssue idle;
        idle.problem = HealthProblem::WeakSignal;
        idle.severity = 0.0f;
        idle.description = "No valid audio captured.";
        idle.recommendation = "Play or record guitar, then press Analyze Tone.";
        result.problems.push_back (std::move (idle));
        return result;
    }

    addIssue (result, HealthProblem::TooMuchGain,
              juce::jlimit (0.0f, 1.0f, (features.rmsDb + 8.0f) / 10.0f)
                  + (features.density > 0.82f ? 0.35f : 0.0f),
              "Gain staging is hot; density is crowding the mix.",
              "Reduce Drive or Amp Gain so RMS sits nearer -18 dB.");

    addIssue (result, HealthProblem::ExcessiveBass,
              juce::jlimit (0.0f, 1.0f, (features.lowEnergy - 0.18f) / 0.25f),
              "Excessive bass energy below ~80 Hz.",
              "Raise cabinet low cut toward 80-110 Hz.");

    addIssue (result, HealthProblem::MuddyLowMids,
              juce::jlimit (0.0f, 1.0f, (features.lowMidEnergy - 0.22f) / 0.28f + analysis.muddiness * 0.35f),
              "Low-mid buildup is masking riff definition.",
              "Reduce low-mid energy around approximately 200-350 Hz.");

    addIssue (result, HealthProblem::WeakMids,
              juce::jlimit (0.0f, 1.0f, (0.14f - features.midEnergy) / 0.14f),
              "Midrange is thin; the guitar may disappear in a mix.",
              "Boost mids around 800-1200 Hz or raise Amp Mid.");

    addIssue (result, HealthProblem::ExcessiveTreble,
              juce::jlimit (0.0f, 1.0f, (features.highEnergy - 0.28f) / 0.3f),
              "Treble energy is dominating the spectrum.",
              "Lower Tone or add a high cut near 8-10 kHz.");

    addIssue (result, HealthProblem::HarshHighs,
              analysis.harshness,
              "Upper harmonics are harsh and fatiguing.",
              "Reduce Bite or Presence; darken Tone slightly.");

    const float noiseGap = features.rmsDb - features.noiseFloorDb;
    addIssue (result, HealthProblem::ExcessiveNoise,
              juce::jlimit (0.0f, 1.0f, (12.0f - noiseGap) / 12.0f) * (features.spectralFlatness > 0.45f ? 1.0f : 0.55f),
              "Noise floor is close to the signal.",
              "Raise the gate threshold or reduce input noise before the plugin.");

    addIssue (result, HealthProblem::ExcessiveCompression,
              juce::jlimit (0.0f, 1.0f, (6.0f - features.crestFactorDb) / 6.0f),
              "Crest factor is low; the tone is over-compressed.",
              "Lower Drive or compressor ratio to restore pick dynamics.");

    addIssue (result, HealthProblem::Clipping,
              juce::jlimit (0.0f, 1.0f, (features.peakDb + 0.4f) / 3.0f),
              "Peaks are clipping or within a fraction of a dB of full scale.",
              "Reduce Output or Dist Out until peaks sit below -0.3 dB.");

    addIssue (result, HealthProblem::WeakSignal,
              juce::jlimit (0.0f, 1.0f, (-features.rmsDb - 36.0f) / 20.0f),
              "Input is too quiet for a confident analysis.",
              "Raise Input until RMS is around -18 to -12 dB.");

    float penalty = 0.0f;
    for (const auto& issue : result.problems)
        penalty += issue.severity * 12.0f;

    result.score = juce::jlimit (0.0f, 100.0f, 100.0f - penalty);

    if (analysis.tightness > 0.55f)
        result.positives.push_back ("Tightness");
    if (features.midEnergy > 0.2f && features.midEnergy < 0.4f)
        result.positives.push_back ("Strong Midrange");
    if (features.crestFactorDb > 8.0f)
        result.positives.push_back ("Open Dynamics");
    if (analysis.brightness > 0.35f && analysis.brightness < 0.7f && analysis.harshness < 0.45f)
        result.positives.push_back ("Balanced Top");

    if (result.score >= 80.0f)      result.grade = "Good";
    else if (result.score >= 60.0f) result.grade = "Fair";
    else                            result.grade = "Poor";

    return result;
}

ToneHealthReport ToneHealth::evaluate (const AudioFeatures& features) const
{
    ToneHealthReport report;
    ToneAnalysisResult analysis;
    analysis.muddiness = features.muddiness;
    analysis.harshness = features.harshness;
    analysis.tightness = juce::jlimit (0.0f, 1.0f, 1.0f - features.lowEnergy);
    analysis.brightness = features.brightness;

    report.detailed = evaluateDetailed (features, analysis);

    if (! features.valid)
    {
        report.notes.emplace_back ("Capture audio before requesting a health report.");
        return report;
    }

    report.mudRisk = juce::jlimit (0.0f, 1.0f, (900.0f - features.spectralCentroidHz) / 900.0f);
    report.harshnessRisk = juce::jlimit (0.0f, 1.0f, (features.spectralRolloffHz - 7000.0f) / 6000.0f);
    report.dynamicsHealth = juce::jlimit (0.0f, 1.0f, features.dynamicRangeDb / 24.0f);
    report.headroomHealth = juce::jlimit (0.0f, 1.0f, (-features.peakDb) / 6.0f);
    report.score = report.detailed.score;

    for (const auto& issue : report.detailed.problems)
        report.notes.push_back (issue.description + " " + issue.recommendation);

    if (report.notes.empty())
        report.notes.emplace_back ("Tone is balanced. Small EQ moves will refine, not rescue.");

    return report;
}
} // namespace WarlockAI

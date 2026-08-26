#pragma once

#include "AI/ToneAnalyzer.h"
#include <JuceHeader.h>
#include <vector>

namespace WarlockAI
{
enum class HealthProblem
{
    TooMuchGain = 0,
    ExcessiveBass,
    MuddyLowMids,
    WeakMids,
    ExcessiveTreble,
    HarshHighs,
    ExcessiveNoise,
    ExcessiveCompression,
    Clipping,
    WeakSignal
};

inline const char* healthProblemName (HealthProblem p) noexcept
{
    switch (p)
    {
        case HealthProblem::TooMuchGain:            return "TooMuchGain";
        case HealthProblem::ExcessiveBass:          return "ExcessiveBass";
        case HealthProblem::MuddyLowMids:           return "MuddyLowMids";
        case HealthProblem::WeakMids:               return "WeakMids";
        case HealthProblem::ExcessiveTreble:        return "ExcessiveTreble";
        case HealthProblem::HarshHighs:             return "HarshHighs";
        case HealthProblem::ExcessiveNoise:         return "ExcessiveNoise";
        case HealthProblem::ExcessiveCompression:   return "ExcessiveCompression";
        case HealthProblem::Clipping:               return "Clipping";
        case HealthProblem::WeakSignal:             return "WeakSignal";
    }
    return "Unknown";
}

struct HealthIssue
{
    HealthProblem problem = HealthProblem::WeakSignal;
    float severity = 0.0f;
    juce::String description;
    juce::String recommendation;
};

struct ToneHealthResult
{
    float score = 0.0f;
    juce::String grade { "Idle" };
    std::vector<HealthIssue> problems;
    std::vector<juce::String> positives;
};

struct ToneHealthReport
{
    float score = 0.0f;          // 0..100
    float mudRisk = 0.0f;        // 0..1
    float harshnessRisk = 0.0f;  // 0..1
    float dynamicsHealth = 0.0f; // 0..1
    float headroomHealth = 0.0f; // 0..1
    std::vector<juce::String> notes;
    ToneHealthResult detailed;
};

class ToneHealth
{
public:
    ToneHealthReport evaluate (const AudioFeatures& features) const;
    ToneHealthResult evaluateDetailed (const AudioFeatures& features,
                                       const ToneAnalysisResult& analysis) const;
};
} // namespace WarlockAI

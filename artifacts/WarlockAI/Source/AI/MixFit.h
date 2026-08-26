#pragma once

#include "AI/ToneAnalyzer.h"
#include "AI/ToneDesigner.h"
#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <map>
#include <vector>

namespace WarlockAI
{
struct MixFitSuggestion
{
    std::map<juce::String, float> parameterValues;
    juce::String rationale;
};

struct MixFitChange
{
    juce::String parameterId;
    juce::String name;
    float before = 0.0f;
    float after = 0.0f;
    juce::String explanation;
};

struct MixFitResult
{
    float beforeScore = 0.0f;
    float afterEstimatedScore = 0.0f;
    std::vector<MixFitChange> changes;
    std::vector<juce::String> recommendations;
    MixFitSuggestion suggestion;
    juce::String summary;
};

class MixFit
{
public:
    MixFitSuggestion suggest (const AudioFeatures& features, DistortionMode currentMode) const;
    MixFitResult fit (const AudioFeatures& features,
                      const ToneAnalysisResult& analysis,
                      float healthScore,
                      const CurrentToneState& current,
                      DistortionMode currentMode) const;
};
} // namespace WarlockAI

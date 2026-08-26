#pragma once

#include "AI/AudioFeatureExtractor.h"
#include <JuceHeader.h>

namespace WarlockAI
{
enum class ToneClass
{
    Clean = 0,
    Crunch,
    TightHighGain,
    OpenHighGain,
    DarkSaturated,
    BrightAggressive,
    Unknown
};

inline const char* toneClassName (ToneClass value) noexcept
{
    switch (value)
    {
        case ToneClass::Clean:            return "Clean / Low Gain";
        case ToneClass::Crunch:           return "Crunch";
        case ToneClass::TightHighGain:    return "Tight High Gain";
        case ToneClass::OpenHighGain:     return "Open High Gain";
        case ToneClass::DarkSaturated:    return "Dark Saturated";
        case ToneClass::BrightAggressive: return "Bright Aggressive";
        case ToneClass::Unknown:          return "Unknown";
    }

    return "Unknown";
}

struct ToneAnalysis
{
    ToneClass classification = ToneClass::Unknown;
    float confidence = 0.0f;
    juce::String summary;
    AudioFeatures features;
};

struct ToneAnalysisResult
{
    float inputLevel = 0.0f;
    float noiseLevel = 0.0f;
    float brightness = 0.0f;
    float warmth = 0.0f;
    float muddiness = 0.0f;
    float harshness = 0.0f;
    float tightness = 0.0f;
    float dynamicRange = 0.0f;
    float saturationEstimate = 0.0f;
    float lowEnergy = 0.0f;
    float midEnergy = 0.0f;
    float highEnergy = 0.0f;
    float confidence = 0.0f;
    float inputLevelConfidence = 0.0f;
    float spectralConfidence = 0.0f;
};

class ToneAnalyzer
{
public:
    ToneAnalysis analyse (const AudioFeatures& features) const;
    ToneAnalysisResult analyseDetailed (const AudioFeatures& features) const;
};
} // namespace WarlockAI

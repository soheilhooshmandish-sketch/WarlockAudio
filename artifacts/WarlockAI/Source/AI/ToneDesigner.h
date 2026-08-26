#pragma once

#include "AI/StyleDetector.h"
#include "AI/ToneAnalyzer.h"
#include "Core/WarlockParameters.h"
#include <JuceHeader.h>
#include <map>
#include <vector>

namespace WarlockAI
{
struct CurrentToneState
{
    float drive = 0.45f;
    float tone = 0.55f;
    float tightness = 0.4f;
    float saturation = 0.35f;
    float warmth = 0.3f;
    float bite = 0.25f;
    float gateThreshold = -48.0f;
    float bass = 0.0f;
    float mid = 0.0f;
    float treble = 0.0f;
    float presence = 0.0f;
    float ampGain = 0.55f;
    float ampBass = 1.5f;
    float ampMid = -1.0f;
    float ampTreble = 2.0f;
    float ampPresence = 1.0f;
    float cabLowCut = 80.0f;
    float cabHighCut = 6500.0f;
    float compressorThreshold = -18.0f;
    float compressorRatio = 3.0f;
    int distortionMode = static_cast<int> (DistortionMode::Metal);
    int ampMode = static_cast<int> (AmpMode::Metal);
};

struct ToneRecommendation
{
    float drive = 0.45f;
    float tone = 0.55f;
    float tightness = 0.4f;
    float saturation = 0.35f;
    float warmth = 0.3f;
    float bite = 0.25f;
    float gateThreshold = -48.0f;
    float bass = 0.0f;
    float mid = 0.0f;
    float treble = 0.0f;
    float presence = 0.0f;
    float ampGain = 0.55f;
    float ampBass = 1.5f;
    float ampMid = -1.0f;
    float ampTreble = 2.0f;
    float ampPresence = 1.0f;
    float cabLowCut = 80.0f;
    float cabHighCut = 6500.0f;
    float compressorThreshold = -18.0f;
    float compressorRatio = 3.0f;
    float confidence = 0.0f;
    juce::String reason;
    std::vector<juce::String> explanations;
    std::map<juce::String, float> parameterValues;
};

class ToneDesigner
{
public:
    ToneRecommendation recommend (const AudioFeatures& features,
                                  const ToneAnalysisResult& analysis,
                                  const StyleDetectionResult& style,
                                  const CurrentToneState& current) const;

    static void clampRecommendation (ToneRecommendation& rec) noexcept;
    static std::map<juce::String, float> toParameterMap (const ToneRecommendation& rec);
};
} // namespace WarlockAI

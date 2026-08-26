#pragma once

#include "AI/AudioFeatureExtractor.h"
#include <JuceHeader.h>

namespace WarlockAI
{
struct ToneAnalysisResult;
struct CurrentToneState;
struct ToneRecommendation;

/** Offline model contract. Never called from processBlock. */
class IAIModel
{
public:
    virtual ~IAIModel() = default;

    virtual void prepare (double sampleRate) = 0;
    virtual AudioFeatures analyze (const juce::AudioBuffer<float>& buffer) = 0;
    virtual ToneRecommendation predict (const AudioFeatures& features,
                                        const ToneAnalysisResult& analysis,
                                        const CurrentToneState& current) const = 0;
    virtual void reset() = 0;
    virtual juce::String getName() const = 0;
};
} // namespace WarlockAI

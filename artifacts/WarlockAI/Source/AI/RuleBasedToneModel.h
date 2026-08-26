#pragma once

#include "AI/IAIModel.h"
#include "AI/ToneDesigner.h"

namespace WarlockAI
{
class RuleBasedToneModel final : public IAIModel
{
public:
    void prepare (double sampleRate) override;
    AudioFeatures analyze (const juce::AudioBuffer<float>& buffer) override;
    ToneRecommendation predict (const AudioFeatures& features,
                                const ToneAnalysisResult& analysis,
                                const CurrentToneState& current) const override;
    void reset() override;
    juce::String getName() const override { return "RuleBasedToneModel"; }

    void setStyle (const StyleDetectionResult& style) { lastStyle = style; }

private:
    AudioFeatureExtractor extractor;
    ToneDesigner designer;
    StyleDetectionResult lastStyle;
};
} // namespace WarlockAI

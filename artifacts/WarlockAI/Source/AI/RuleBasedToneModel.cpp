#include "AI/RuleBasedToneModel.h"

namespace WarlockAI
{
void RuleBasedToneModel::prepare (double sampleRate)
{
    extractor.prepare (sampleRate, 11);
}

void RuleBasedToneModel::reset()
{
    extractor.reset();
}

AudioFeatures RuleBasedToneModel::analyze (const juce::AudioBuffer<float>& buffer)
{
    return extractor.extract (buffer);
}

ToneRecommendation RuleBasedToneModel::predict (const AudioFeatures& features,
                                                const ToneAnalysisResult& analysis,
                                                const CurrentToneState& current) const
{
    return designer.recommend (features, analysis, lastStyle, current);
}
} // namespace WarlockAI

#pragma once

#include "AI/IAIModel.h"
#include "AI/ToneAnalyzer.h"
#include "AI/ToneDesigner.h"

namespace WarlockAI
{
/**
    Future ONNX Runtime hook.

    This class is intentionally not linked against ONNX Runtime. Instantiating it
    does not load a network and does not claim a trained model exists. Enable a
    real implementation later with WARLOCK_ENABLE_ONNX and an actual .onnx file.
*/
class ONNXToneModel final : public IAIModel
{
public:
    void prepare (double) override {}
    void reset() override {}

    AudioFeatures analyze (const juce::AudioBuffer<float>&) override
    {
        return {};
    }

    ToneRecommendation predict (const AudioFeatures&,
                                const ToneAnalysisResult&,
                                const CurrentToneState&) const override
    {
        ToneRecommendation rec;
        rec.confidence = 0.0f;
        rec.reason = "ONNX Runtime is not linked in this build. RuleBasedToneModel is the active engine.";
        return rec;
    }

    juce::String getName() const override { return "ONNX (not linked)"; }
};
} // namespace WarlockAI

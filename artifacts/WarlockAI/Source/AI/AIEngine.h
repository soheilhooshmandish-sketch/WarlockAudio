#pragma once

#include "AI/AudioFeatureExtractor.h"
#include "AI/MixFit.h"
#include "AI/RuleBasedToneModel.h"
#include "AI/StyleDetector.h"
#include "AI/ToneAnalyzer.h"
#include "AI/ToneDesigner.h"
#include "AI/ToneHealth.h"
#include <JuceHeader.h>
#include <atomic>
#include <mutex>

namespace WarlockAI
{
struct AdaptiveHints
{
    float brightness = 0.5f;
    float density = 0.5f;
    float dynamics = 0.5f;
    float rmsDb = -20.0f;
    float peakDb = -6.0f;
    float picking = 0.5f;
};

struct AIResultPack
{
    AudioFeatures features;
    ToneAnalysis analysis;
    ToneAnalysisResult analysisResult;
    ToneHealthReport health;
    ToneHealthResult healthResult;
    StyleDetectionResult style;
    ToneRecommendation recommendation;
    MixFitResult mixFit;
    AdaptiveHints hints;
    bool ready = false;
    juce::int64 analysedAtMs = 0;
};

class AIEngine
{
public:
    void prepare (double sampleRate);
    void reset();

    // Analysis thread only. Never call from processBlock.
    AIResultPack analyse (const juce::AudioBuffer<float>& buffer,
                          const CurrentToneState& current,
                          DistortionMode mode);

    AdaptiveHints lightHints (const juce::AudioBuffer<float>& buffer);

    void publish (const AIResultPack& pack);
    AIResultPack copyPublished() const;
    AdaptiveHints loadHints() const noexcept;

    bool shouldSkip (juce::int64 nowMs, int minIntervalMs = 250) const noexcept;

    RuleBasedToneModel& getModel() noexcept { return model; }

private:
    AudioFeatureExtractor extractor;
    ToneAnalyzer analyzer;
    ToneHealth health;
    StyleDetector styleDetector;
    ToneDesigner designer;
    MixFit mixFit;
    RuleBasedToneModel model;

    mutable std::mutex publishedMutex;
    AIResultPack published;
    std::atomic<float> hintBrightness { 0.5f };
    std::atomic<float> hintDensity { 0.5f };
    std::atomic<float> hintDynamics { 0.5f };
    std::atomic<float> hintRmsDb { -20.0f };
    std::atomic<float> hintPeakDb { -6.0f };
    std::atomic<float> hintPicking { 0.5f };
    std::atomic<juce::int64> lastAnalyseMs { 0 };
};
} // namespace WarlockAI

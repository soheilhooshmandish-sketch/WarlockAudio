#include "AI/AIEngine.h"
#include "AI/ONNXToneModel.h"

namespace WarlockAI
{
void AIEngine::prepare (double sampleRate)
{
    extractor.prepare (sampleRate, 11);
    model.prepare (sampleRate);
}

void AIEngine::reset()
{
    extractor.reset();
    model.reset();
}

bool AIEngine::shouldSkip (juce::int64 nowMs, int minIntervalMs) const noexcept
{
    const auto last = lastAnalyseMs.load (std::memory_order_relaxed);
    return last > 0 && (nowMs - last) < minIntervalMs;
}

AdaptiveHints AIEngine::lightHints (const juce::AudioBuffer<float>& buffer)
{
    AdaptiveHints hints;
    const auto features = extractor.extract (buffer);
    if (! features.valid)
        return hints;

    hints.brightness = features.brightness;
    hints.density = features.density;
    hints.dynamics = juce::jlimit (0.0f, 1.0f, features.dynamicRangeDb / 24.0f);
    hints.rmsDb = features.rmsDb;
    hints.peakDb = features.peakDb;
    hints.picking = juce::jlimit (0.0f, 1.0f, features.transientStrength * 0.7f + features.zeroCrossingRate * 4.0f);
    hintBrightness.store (hints.brightness, std::memory_order_relaxed);
    hintDensity.store (hints.density, std::memory_order_relaxed);
    hintDynamics.store (hints.dynamics, std::memory_order_relaxed);
    hintRmsDb.store (hints.rmsDb, std::memory_order_relaxed);
    hintPeakDb.store (hints.peakDb, std::memory_order_relaxed);
    hintPicking.store (hints.picking, std::memory_order_relaxed);
    return hints;
}

AIResultPack AIEngine::analyse (const juce::AudioBuffer<float>& buffer,
                                const CurrentToneState& current,
                                DistortionMode mode)
{
    AIResultPack pack;
    pack.features = extractor.extract (buffer);
    pack.analysis = analyzer.analyse (pack.features);
    pack.analysisResult = analyzer.analyseDetailed (pack.features);
    pack.health = health.evaluate (pack.features);
    pack.healthResult = health.evaluateDetailed (pack.features, pack.analysisResult);
    pack.health.score = pack.healthResult.score;
    pack.health.detailed = pack.healthResult;
    pack.style = styleDetector.detect (pack.features, pack.analysisResult);
    model.setStyle (pack.style);
    pack.recommendation = model.predict (pack.features, pack.analysisResult, current);
    pack.mixFit = mixFit.fit (pack.features, pack.analysisResult, pack.healthResult.score, current, mode);
    if (pack.features.valid)
    {
        pack.hints.brightness = pack.features.brightness;
        pack.hints.density = pack.features.density;
        pack.hints.dynamics = juce::jlimit (0.0f, 1.0f, pack.features.dynamicRangeDb / 24.0f);
        pack.hints.rmsDb = pack.features.rmsDb;
        pack.hints.peakDb = pack.features.peakDb;
        pack.hints.picking = juce::jlimit (0.0f, 1.0f,
            pack.features.transientStrength * 0.7f + pack.features.zeroCrossingRate * 4.0f);
        hintBrightness.store (pack.hints.brightness, std::memory_order_relaxed);
        hintDensity.store (pack.hints.density, std::memory_order_relaxed);
        hintDynamics.store (pack.hints.dynamics, std::memory_order_relaxed);
        hintRmsDb.store (pack.hints.rmsDb, std::memory_order_relaxed);
        hintPeakDb.store (pack.hints.peakDb, std::memory_order_relaxed);
        hintPicking.store (pack.hints.picking, std::memory_order_relaxed);
    }
    pack.ready = pack.features.valid;
    pack.analysedAtMs = juce::Time::currentTimeMillis();
    lastAnalyseMs.store (pack.analysedAtMs, std::memory_order_relaxed);
    publish (pack);
    return pack;
}

void AIEngine::publish (const AIResultPack& pack)
{
    std::lock_guard lock (publishedMutex);
    published = pack;
}

AIResultPack AIEngine::copyPublished() const
{
    std::lock_guard lock (publishedMutex);
    return published;
}

AdaptiveHints AIEngine::loadHints() const noexcept
{
    AdaptiveHints h;
    h.brightness = hintBrightness.load (std::memory_order_relaxed);
    h.density = hintDensity.load (std::memory_order_relaxed);
    h.dynamics = hintDynamics.load (std::memory_order_relaxed);
    h.rmsDb = hintRmsDb.load (std::memory_order_relaxed);
    h.peakDb = hintPeakDb.load (std::memory_order_relaxed);
    h.picking = hintPicking.load (std::memory_order_relaxed);
    return h;
}
} // namespace WarlockAI

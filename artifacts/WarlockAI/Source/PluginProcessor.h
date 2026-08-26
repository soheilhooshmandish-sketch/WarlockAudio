#pragma once

#include "AI/AIEngine.h"
#include "AI/AudioFeatureExtractor.h"
#include "AI/MixFit.h"
#include "AI/ToneAnalyzer.h"
#include "AI/ToneDesigner.h"
#include "AI/ToneHealth.h"
#include "Core/PresetManager.h"
#include "Core/WarlockParameters.h"
#include "Engine/SignalChain.h"
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <map>
#include <mutex>
#include <vector>

namespace WarlockAI
{
struct AnalysisSnapshot
{
    AudioFeatures features;
    ToneAnalysis analysis;
    ToneAnalysisResult analysisResult;
    ToneHealthReport health;
    ToneHealthResult healthResult;
    MixFitSuggestion mixFit;
    MixFitResult mixFitResult;
    ToneRecommendation recommendation;
    StyleDetectionResult style;
    bool ready = false;
    bool appliedMixFit = false;
    bool analysing = false;
    bool previewActive = false;
};

class WarlockAIAudioProcessor final : public juce::AudioProcessor,
                                      private juce::Thread
{
public:
    WarlockAIAudioProcessor();
    ~WarlockAIAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    using juce::AudioProcessor::processBlock;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.5; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return "Default"; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts; }
    PresetManager& getPresetManager() noexcept { return presets; }
    SignalChain& getSignalChain() noexcept { return chain; }

    void requestToneAnalysis();
    void requestAutoBuildTone();
    void requestMixFitPreview();
    void requestToneHealth();
    void commitMixFit();
    void undoLastAiEdit();
    AnalysisSnapshot getAnalysisSnapshot() const;
    AIResultPack getAiPack() const;
    bool isAnalysisBusy() const noexcept { return analysisBusy.load (std::memory_order_relaxed); }
    bool isMixFitPreviewActive() const noexcept { return previewActive.load (std::memory_order_relaxed); }

    float getCpuLoad() const noexcept { return cpuLoad.load (std::memory_order_relaxed); }
    int getReportedLatencySamples() const noexcept { return reportedLatency.load (std::memory_order_relaxed); }
    double getCurrentSampleRate() const noexcept { return currentSampleRate.load (std::memory_order_relaxed); }

    float getCompressorGainReduction() const noexcept { return chain.getCompressor().getGainReductionDb(); }
    float getDelayWetLevel() const noexcept { return chain.getDelay().getWetLevel(); }
    float getReverbWetLevel() const noexcept { return chain.getReverb().getWetLevel(); }
    float getStereoWidthMeter() const noexcept { return chain.getStereo().getWidthMeter(); }

    static constexpr int spectrumBins = 128;
    void getSpectrumMagnitudes (std::array<float, spectrumBins>& dest);

private:
    void run() override;
    void cacheParameters();
    SignalChainParameters collectParameters() const;
    CurrentToneState collectToneState() const;
    void pushCaptureSample (float sample) noexcept;
    void pushSpectrumSample (float sample) noexcept;
    void applyParameterMapOnMessageThread (const std::map<juce::String, float>& values);
    void storeUndoSnapshot();
    void applyMixFitOnMessageThread (const MixFitSuggestion& suggestion);
    float currentTempoBpm() const;
    bool copyCapture (juce::AudioBuffer<float>& dest, int minSamples);

    juce::AudioProcessorValueTreeState apvts;
    PresetManager presets;
    SignalChain chain;

    std::atomic<float>* pInputGain = nullptr;
    std::atomic<float>* pOutputGain = nullptr;
    std::atomic<float>* pDrive = nullptr;
    std::atomic<float>* pTone = nullptr;
    std::atomic<float>* pTightness = nullptr;
    std::atomic<float>* pSaturation = nullptr;
    std::atomic<float>* pWarmth = nullptr;
    std::atomic<float>* pBite = nullptr;
    std::atomic<float>* pDistOut = nullptr;
    std::atomic<float>* pDistMode = nullptr;
    std::atomic<float>* pOversample = nullptr;
    std::atomic<float>* pDistBypass = nullptr;
    std::atomic<float>* pGateThresh = nullptr;
    std::atomic<float>* pGateAttack = nullptr;
    std::atomic<float>* pGateRelease = nullptr;
    std::atomic<float>* pGateRatio = nullptr;
    std::atomic<float>* pGateBypass = nullptr;
    std::atomic<float>* pCompThresh = nullptr;
    std::atomic<float>* pCompRatio = nullptr;
    std::atomic<float>* pCompAttack = nullptr;
    std::atomic<float>* pCompRelease = nullptr;
    std::atomic<float>* pCompMakeup = nullptr;
    std::atomic<float>* pCompBypass = nullptr;
    std::atomic<float>* pCompEnabled = nullptr;
    std::atomic<float>* pCompMix = nullptr;
    std::atomic<float>* pEqLowGain = nullptr;
    std::atomic<float>* pEqLowFreq = nullptr;
    std::atomic<float>* pEqLowMidGain = nullptr;
    std::atomic<float>* pEqLowMidFreq = nullptr;
    std::atomic<float>* pEqLowMidQ = nullptr;
    std::atomic<float>* pEqHighMidGain = nullptr;
    std::atomic<float>* pEqHighMidFreq = nullptr;
    std::atomic<float>* pEqHighMidQ = nullptr;
    std::atomic<float>* pEqHighGain = nullptr;
    std::atomic<float>* pEqHighFreq = nullptr;
    std::atomic<float>* pEqBypass = nullptr;
    std::atomic<float>* pDelayTime = nullptr;
    std::atomic<float>* pDelayFeedback = nullptr;
    std::atomic<float>* pDelayMix = nullptr;
    std::atomic<float>* pDelayBypass = nullptr;
    std::atomic<float>* pDelayEnabled = nullptr;
    std::atomic<float>* pDelayLowCut = nullptr;
    std::atomic<float>* pDelayHighCut = nullptr;
    std::atomic<float>* pDelayWidth = nullptr;
    std::atomic<float>* pDelaySync = nullptr;
    std::atomic<float>* pDelayMode = nullptr;
    std::atomic<float>* pReverbSize = nullptr;
    std::atomic<float>* pReverbDamp = nullptr;
    std::atomic<float>* pReverbWidth = nullptr;
    std::atomic<float>* pReverbMix = nullptr;
    std::atomic<float>* pReverbBypass = nullptr;
    std::atomic<float>* pReverbEnabled = nullptr;
    std::atomic<float>* pReverbPreDelay = nullptr;
    std::atomic<float>* pStereoWidth = nullptr;
    std::atomic<float>* pStereoBalance = nullptr;
    std::atomic<float>* pStereoHaas = nullptr;
    std::atomic<float>* pStereoBypass = nullptr;
    std::atomic<float>* pStereoEnabled = nullptr;
    std::atomic<float>* pMonoMode = nullptr;
    std::atomic<float>* pMidGain = nullptr;
    std::atomic<float>* pSideGain = nullptr;
    std::atomic<float>* pLimCeiling = nullptr;
    std::atomic<float>* pLimRelease = nullptr;
    std::atomic<float>* pLimBypass = nullptr;
    std::atomic<float>* pAmpEnabled = nullptr;
    std::atomic<float>* pAmpMode = nullptr;
    std::atomic<float>* pAmpGain = nullptr;
    std::atomic<float>* pAmpBass = nullptr;
    std::atomic<float>* pAmpMid = nullptr;
    std::atomic<float>* pAmpTreble = nullptr;
    std::atomic<float>* pAmpPresence = nullptr;
    std::atomic<float>* pAmpResonance = nullptr;
    std::atomic<float>* pAmpMaster = nullptr;
    std::atomic<float>* pCabEnabled = nullptr;
    std::atomic<float>* pCabMix = nullptr;
    std::atomic<float>* pCabLowCut = nullptr;
    std::atomic<float>* pCabHighCut = nullptr;
    std::atomic<float>* pCabMonoStereo = nullptr;
    std::atomic<float>* pHarmEnabled = nullptr;
    std::atomic<float>* pHarmInterval1 = nullptr;
    std::atomic<float>* pHarmInterval2 = nullptr;
    std::atomic<float>* pHarmMix = nullptr;
    std::atomic<float>* pHarmDetune = nullptr;
    std::atomic<float>* pHarmPan1 = nullptr;
    std::atomic<float>* pHarmPan2 = nullptr;

    ParameterSmoother inputSmoother;
    ParameterSmoother outputSmoother;

    static constexpr int captureCapacity = 96000;
    juce::AbstractFifo captureFifo { captureCapacity };
    std::vector<float> captureRing;
    std::atomic<bool> analysisRequested { false };
    std::atomic<bool> autoBuildRequested { false };
    std::atomic<bool> mixFitRequested { false };
    std::atomic<bool> analysisBusy { false };
    std::atomic<bool> previewActive { false };

    AIEngine aiEngine;
    AudioFeatureExtractor extractor;
    ToneAnalyzer analyzer;
    ToneHealth healthEngine;
    MixFit mixFitEngine;

    mutable std::mutex snapshotMutex;
    AnalysisSnapshot snapshot;
    std::map<juce::String, float> undoValues;
    std::atomic<float> adaptiveBrightness { 0.5f };
    std::atomic<float> adaptiveDensity { 0.5f };
    std::atomic<float> adaptiveDynamics { 0.5f };
    std::atomic<float> adaptiveRmsDb { -20.0f };
    std::atomic<float> adaptivePeakDb { -6.0f };
    std::atomic<float> adaptivePicking { 0.5f };

    static constexpr int spectrumCapacity = 4096;
    juce::AbstractFifo spectrumFifo { spectrumCapacity };
    std::vector<float> spectrumRing;
    mutable std::mutex spectrumMutex;
    std::array<float, spectrumBins> spectrumMags {};
    std::unique_ptr<juce::dsp::FFT> spectrumFft;
    std::vector<float> spectrumFftData;
    std::vector<float> spectrumTimeDomain;
    std::vector<float> spectrumWork;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> spectrumWindow;

    std::atomic<float> cpuLoad { 0.0f };
    std::atomic<int> reportedLatency { 0 };
    std::atomic<double> currentSampleRate { 44100.0 };
    int lastLatency = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarlockAIAudioProcessor)
};
} // namespace WarlockAI

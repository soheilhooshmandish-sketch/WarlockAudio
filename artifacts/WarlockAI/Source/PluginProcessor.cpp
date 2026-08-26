#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace WarlockAI
{
namespace
{
    std::atomic<float>* raw (juce::AudioProcessorValueTreeState& state, const char* id)
    {
        return state.getRawParameterValue (id);
    }
}

WarlockAIAudioProcessor::WarlockAIAudioProcessor()
    : juce::AudioProcessor (BusesProperties()
                                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      juce::Thread ("WarlockAI Analysis"),
      apvts (*this, nullptr, "WARLOCKAI", createParameterLayout()),
      presets (apvts)
{
    cacheParameters();
    captureRing.assign (static_cast<size_t> (captureCapacity), 0.0f);
    spectrumRing.assign (static_cast<size_t> (spectrumCapacity), 0.0f);
    presets.storeFactoryState();
    startThread (juce::Thread::Priority::low);
}

WarlockAIAudioProcessor::~WarlockAIAudioProcessor()
{
    analysisRequested.store (false);
    autoBuildRequested.store (false);
    mixFitRequested.store (false);
    signalThreadShouldExit();
    notify();
    stopThread (2000);
}

void WarlockAIAudioProcessor::cacheParameters()
{
    pInputGain      = raw (apvts, ParamID::inputGain);
    pOutputGain     = raw (apvts, ParamID::outputGain);
    pDrive          = raw (apvts, ParamID::drive);
    pTone           = raw (apvts, ParamID::tone);
    pTightness      = raw (apvts, ParamID::tightness);
    pSaturation     = raw (apvts, ParamID::saturation);
    pWarmth         = raw (apvts, ParamID::warmth);
    pBite           = raw (apvts, ParamID::bite);
    pDistOut        = raw (apvts, ParamID::distortionOut);
    pDistMode       = raw (apvts, ParamID::distortionMode);
    pOversample     = raw (apvts, ParamID::oversample);
    pDistBypass     = raw (apvts, ParamID::distortionBypass);
    pGateThresh     = raw (apvts, ParamID::gateThreshold);
    pGateAttack     = raw (apvts, ParamID::gateAttack);
    pGateRelease    = raw (apvts, ParamID::gateRelease);
    pGateRatio      = raw (apvts, ParamID::gateRatio);
    pGateBypass     = raw (apvts, ParamID::gateBypass);
    pCompThresh     = raw (apvts, ParamID::compThreshold);
    pCompRatio      = raw (apvts, ParamID::compRatio);
    pCompAttack     = raw (apvts, ParamID::compAttack);
    pCompRelease    = raw (apvts, ParamID::compRelease);
    pCompMakeup     = raw (apvts, ParamID::compMakeup);
    pCompBypass     = raw (apvts, ParamID::compBypass);
    pCompEnabled    = raw (apvts, ParamID::compressorEnabled);
    pCompMix        = raw (apvts, ParamID::compressorMix);
    pEqLowGain      = raw (apvts, ParamID::eqLowGain);
    pEqLowFreq      = raw (apvts, ParamID::eqLowFreq);
    pEqLowMidGain   = raw (apvts, ParamID::eqLowMidGain);
    pEqLowMidFreq   = raw (apvts, ParamID::eqLowMidFreq);
    pEqLowMidQ      = raw (apvts, ParamID::eqLowMidQ);
    pEqHighMidGain  = raw (apvts, ParamID::eqHighMidGain);
    pEqHighMidFreq  = raw (apvts, ParamID::eqHighMidFreq);
    pEqHighMidQ     = raw (apvts, ParamID::eqHighMidQ);
    pEqHighGain     = raw (apvts, ParamID::eqHighGain);
    pEqHighFreq     = raw (apvts, ParamID::eqHighFreq);
    pEqBypass       = raw (apvts, ParamID::eqBypass);
    pDelayTime      = raw (apvts, ParamID::delayTime);
    pDelayFeedback  = raw (apvts, ParamID::delayFeedback);
    pDelayMix       = raw (apvts, ParamID::delayMix);
    pDelayBypass    = raw (apvts, ParamID::delayBypass);
    pDelayEnabled   = raw (apvts, ParamID::delayEnabled);
    pDelayLowCut    = raw (apvts, ParamID::delayLowCut);
    pDelayHighCut   = raw (apvts, ParamID::delayHighCut);
    pDelayWidth     = raw (apvts, ParamID::delayStereoWidth);
    pDelaySync      = raw (apvts, ParamID::delaySync);
    pDelayMode      = raw (apvts, ParamID::delayMode);
    pReverbSize     = raw (apvts, ParamID::reverbSize);
    pReverbDamp     = raw (apvts, ParamID::reverbDamping);
    pReverbWidth    = raw (apvts, ParamID::reverbWidth);
    pReverbMix      = raw (apvts, ParamID::reverbMix);
    pReverbBypass   = raw (apvts, ParamID::reverbBypass);
    pReverbEnabled  = raw (apvts, ParamID::reverbEnabled);
    pReverbPreDelay = raw (apvts, ParamID::reverbPreDelay);
    pStereoWidth    = raw (apvts, ParamID::stereoWidth);
    pStereoBalance  = raw (apvts, ParamID::stereoBalance);
    pStereoHaas     = raw (apvts, ParamID::stereoHaas);
    pStereoBypass   = raw (apvts, ParamID::stereoBypass);
    pStereoEnabled  = raw (apvts, ParamID::stereoEnabled);
    pMonoMode       = raw (apvts, ParamID::monoMode);
    pMidGain        = raw (apvts, ParamID::midGain);
    pSideGain       = raw (apvts, ParamID::sideGain);
    pLimCeiling     = raw (apvts, ParamID::limiterCeiling);
    pLimRelease     = raw (apvts, ParamID::limiterRelease);
    pLimBypass      = raw (apvts, ParamID::limiterBypass);
    pAmpEnabled     = raw (apvts, ParamID::ampEnabled);
    pAmpMode        = raw (apvts, ParamID::ampMode);
    pAmpGain        = raw (apvts, ParamID::ampGain);
    pAmpBass        = raw (apvts, ParamID::ampBass);
    pAmpMid         = raw (apvts, ParamID::ampMid);
    pAmpTreble      = raw (apvts, ParamID::ampTreble);
    pAmpPresence    = raw (apvts, ParamID::ampPresence);
    pAmpResonance   = raw (apvts, ParamID::ampResonance);
    pAmpMaster      = raw (apvts, ParamID::ampMaster);
    pCabEnabled     = raw (apvts, ParamID::cabEnabled);
    pCabMix         = raw (apvts, ParamID::cabMix);
    pCabLowCut      = raw (apvts, ParamID::cabLowCut);
    pCabHighCut     = raw (apvts, ParamID::cabHighCut);
    pCabMonoStereo  = raw (apvts, ParamID::cabMonoStereo);
    pHarmEnabled    = raw (apvts, ParamID::harmEnabled);
    pHarmInterval1  = raw (apvts, ParamID::harmInterval1);
    pHarmInterval2  = raw (apvts, ParamID::harmInterval2);
    pHarmMix        = raw (apvts, ParamID::harmMix);
    pHarmDetune     = raw (apvts, ParamID::harmDetune);
    pHarmPan1       = raw (apvts, ParamID::harmPan1);
    pHarmPan2       = raw (apvts, ParamID::harmPan2);
}

void WarlockAIAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate.store (sampleRate, std::memory_order_relaxed);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (juce::jmax (1, samplesPerBlock));
    spec.numChannels = static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()));

    chain.prepare (spec);
    inputSmoother.reset (sampleRate, 0.02f, loadParam (pInputGain));
    outputSmoother.reset (sampleRate, 0.02f, loadParam (pOutputGain));

    extractor.prepare (sampleRate, 11);
    aiEngine.prepare (sampleRate);

    constexpr int fftOrder = 11;
    spectrumFft = std::make_unique<juce::dsp::FFT> (fftOrder);
    spectrumWindow = std::make_unique<juce::dsp::WindowingFunction<float>> (
        static_cast<size_t> (1 << fftOrder), juce::dsp::WindowingFunction<float>::hann, true);
    spectrumFftData.assign (static_cast<size_t> ((1 << fftOrder) * 2), 0.0f);
    spectrumTimeDomain.assign (static_cast<size_t> (1 << fftOrder), 0.0f);
    spectrumWork.assign (static_cast<size_t> ((1 << fftOrder) * 2), 0.0f);

    lastLatency = -1;
}

void WarlockAIAudioProcessor::releaseResources()
{
    chain.reset();
}

bool WarlockAIAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (in.isDisabled() || out.isDisabled())
        return false;

    if (in.size() > 2 || out.size() > 2)
        return false;

    if (out.size() < in.size())
        return false;

    return (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo())
        && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

SignalChainParameters WarlockAIAudioProcessor::collectParameters() const
{
    SignalChainParameters p;

    p.inputGainDb  = loadParam (pInputGain);
    p.outputGainDb = loadParam (pOutputGain);

    p.gateBypass       = loadBoolParam (pGateBypass);
    p.gateThresholdDb  = loadParam (pGateThresh, -48.0f);
    p.gateAttackMs     = loadParam (pGateAttack, 1.5f);
    p.gateReleaseMs    = loadParam (pGateRelease, 80.0f);
    p.gateRatio        = loadParam (pGateRatio, 8.0f);

    p.compBypass       = ! loadBoolParam (pCompEnabled) && loadBoolParam (pCompBypass);
    p.compThresholdDb  = loadParam (pCompThresh, -18.0f);
    p.compRatio        = loadParam (pCompRatio, 3.0f);
    p.compAttackMs     = loadParam (pCompAttack, 8.0f);
    p.compReleaseMs    = loadParam (pCompRelease, 120.0f);
    p.compMakeupDb     = loadParam (pCompMakeup);
    p.compMix          = loadParam (pCompMix, 1.0f);

    p.distortion.drive           = loadParam (pDrive, 0.45f);
    p.distortion.tone            = loadParam (pTone, 0.55f);
    p.distortion.tightness       = loadParam (pTightness, 0.4f);
    p.distortion.saturation      = loadParam (pSaturation, 0.35f);
    p.distortion.warmth          = loadParam (pWarmth, 0.3f);
    p.distortion.bite            = loadParam (pBite, 0.25f);
    p.distortion.outputDb        = loadParam (pDistOut);
    p.distortion.mode            = juce::roundToInt (loadParam (pDistMode, 3.0f));
    p.distortion.oversampleIndex = juce::roundToInt (loadParam (pOversample, 2.0f));
    p.distortion.bypassed        = loadBoolParam (pDistBypass);

    p.distortion.adaptiveBrightness = adaptiveBrightness.load (std::memory_order_relaxed);
    p.distortion.adaptiveDensity    = adaptiveDensity.load (std::memory_order_relaxed);
    p.distortion.adaptiveDynamics   = adaptiveDynamics.load (std::memory_order_relaxed);
    p.distortion.adaptiveRmsDb      = adaptiveRmsDb.load (std::memory_order_relaxed);
    p.distortion.adaptivePeakDb     = adaptivePeakDb.load (std::memory_order_relaxed);
    p.distortion.adaptivePicking    = adaptivePicking.load (std::memory_order_relaxed);

    p.eqBypass      = loadBoolParam (pEqBypass);
    p.eqLowGain     = loadParam (pEqLowGain);
    p.eqLowFreq     = loadParam (pEqLowFreq, 110.0f);
    p.eqLowMidGain  = loadParam (pEqLowMidGain);
    p.eqLowMidFreq  = loadParam (pEqLowMidFreq, 450.0f);
    p.eqLowMidQ     = loadParam (pEqLowMidQ, 1.0f);
    p.eqHighMidGain = loadParam (pEqHighMidGain);
    p.eqHighMidFreq = loadParam (pEqHighMidFreq, 1800.0f);
    p.eqHighMidQ    = loadParam (pEqHighMidQ, 1.1f);
    p.eqHighGain    = loadParam (pEqHighGain);
    p.eqHighFreq    = loadParam (pEqHighFreq, 6500.0f);

    p.ampEnabled    = loadBoolParam (pAmpEnabled);
    p.ampMode       = juce::roundToInt (loadParam (pAmpMode, 4.0f));
    p.ampGain       = loadParam (pAmpGain, 0.55f);
    p.ampBass       = loadParam (pAmpBass, 1.5f);
    p.ampMid        = loadParam (pAmpMid, -1.0f);
    p.ampTreble     = loadParam (pAmpTreble, 2.0f);
    p.ampPresence   = loadParam (pAmpPresence, 1.0f);
    p.ampResonance  = loadParam (pAmpResonance, 0.35f);
    p.ampMasterDb   = loadParam (pAmpMaster);

    p.cabEnabled    = loadBoolParam (pCabEnabled);
    p.cabMix        = loadParam (pCabMix, 1.0f);
    p.cabLowCut     = loadParam (pCabLowCut, 80.0f);
    p.cabHighCut    = loadParam (pCabHighCut, 6500.0f);
    p.cabStereo     = juce::roundToInt (loadParam (pCabMonoStereo, 1.0f)) >= 1;

    p.harmEnabled   = loadBoolParam (pHarmEnabled);
    p.harmInterval1 = juce::roundToInt (loadParam (pHarmInterval1, 5.0f));
    p.harmInterval2 = juce::roundToInt (loadParam (pHarmInterval2, 6.0f));
    p.harmMix       = loadParam (pHarmMix, 0.25f);
    p.harmDetune    = loadParam (pHarmDetune, 4.0f);
    p.harmPan1      = loadParam (pHarmPan1, -0.4f);
    p.harmPan2      = loadParam (pHarmPan2, 0.4f);

    p.delayBypass   = ! loadBoolParam (pDelayEnabled) && loadBoolParam (pDelayBypass);
    p.delayTimeMs   = loadParam (pDelayTime, 320.0f);
    p.delayFeedback = loadParam (pDelayFeedback, 0.25f);
    p.delayMix      = loadParam (pDelayMix);
    p.delayLowCut   = loadParam (pDelayLowCut, 80.0f);
    p.delayHighCut  = loadParam (pDelayHighCut, 8000.0f);
    p.delayWidth    = loadParam (pDelayWidth, 1.0f);
    p.delayMode     = juce::roundToInt (loadParam (pDelayMode, 1.0f));
    p.delaySync     = juce::roundToInt (loadParam (pDelaySync));
    p.tempoBpm      = currentTempoBpm();

    p.reverbBypass  = ! loadBoolParam (pReverbEnabled) && loadBoolParam (pReverbBypass);
    p.reverbSize    = loadParam (pReverbSize, 0.35f);
    p.reverbDamping = loadParam (pReverbDamp, 0.5f);
    p.reverbWidth   = loadParam (pReverbWidth, 1.0f);
    p.reverbMix     = loadParam (pReverbMix);
    p.reverbPreDelayMs = loadParam (pReverbPreDelay, 18.0f);

    p.stereoBypass  = ! loadBoolParam (pStereoEnabled) && loadBoolParam (pStereoBypass);
    p.stereoWidth   = loadParam (pStereoWidth, 1.0f);
    p.stereoBalance = loadParam (pStereoBalance);
    p.stereoHaasMs  = loadParam (pStereoHaas);
    p.monoMode      = loadBoolParam (pMonoMode);
    p.midGainDb     = loadParam (pMidGain);
    p.sideGainDb    = loadParam (pSideGain);

    p.limiterBypass    = loadBoolParam (pLimBypass);
    p.limiterCeilingDb = loadParam (pLimCeiling, -0.3f);
    p.limiterReleaseMs = loadParam (pLimRelease, 50.0f);

    return p;
}

float WarlockAIAudioProcessor::currentTempoBpm() const
{
    if (auto* head = getPlayHead())
        if (auto position = head->getPosition())
            if (auto bpm = position->getBpm())
                return static_cast<float> (*bpm);

    return 120.0f;
}

void WarlockAIAudioProcessor::pushCaptureSample (float sample) noexcept
{
    int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
    captureFifo.prepareToWrite (1, start1, size1, start2, size2);

    if (size1 > 0)
        captureRing[static_cast<size_t> (start1)] = sample;
    else if (size2 > 0)
        captureRing[static_cast<size_t> (start2)] = sample;
    else
        return;

    captureFifo.finishedWrite (1);
}

void WarlockAIAudioProcessor::pushSpectrumSample (float sample) noexcept
{
    int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
    spectrumFifo.prepareToWrite (1, start1, size1, start2, size2);

    if (size1 > 0)
        spectrumRing[static_cast<size_t> (start1)] = sample;
    else if (size2 > 0)
        spectrumRing[static_cast<size_t> (start2)] = sample;
    else
        return;

    spectrumFifo.finishedWrite (1);
}

void WarlockAIAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused (midi);
    juce::ScopedNoDenormals noDenormals;

    const auto totalIn  = getTotalNumInputChannels();
    const auto totalOut = getTotalNumOutputChannels();

    for (int ch = totalIn; ch < totalOut; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    if (buffer.getNumSamples() <= 0)
        return;

    const auto startTicks = juce::Time::getHighResolutionTicks();

    auto params = collectParameters();
    inputSmoother.setTarget (params.inputGainDb);
    outputSmoother.setTarget (params.outputGainDb);
    params.inputGainDb  = inputSmoother.getNext();
    params.outputGainDb = outputSmoother.getNext();
    inputSmoother.skip (juce::jmax (0, buffer.getNumSamples() - 1));
    outputSmoother.skip (juce::jmax (0, buffer.getNumSamples() - 1));

    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();
    const float invCh = channels > 0 ? 1.0f / static_cast<float> (channels) : 1.0f;

    // Lock-free dry capture for the analysis thread. Never allocate, never lock.
    for (int i = 0; i < samples; ++i)
    {
        float mix = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            mix += buffer.getSample (ch, i);

        mix *= invCh;
        pushCaptureSample (mix);
    }

    chain.setParameters (params);
    chain.process (buffer);

    const int latency = chain.getLatencySamples();
    if (latency != lastLatency)
    {
        lastLatency = latency;
        setLatencySamples (latency);
        reportedLatency.store (latency, std::memory_order_relaxed);
    }

    for (int i = 0; i < samples; ++i)
    {
        float mix = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            mix += buffer.getSample (ch, i);

        mix *= invCh;
        pushSpectrumSample (mix);
    }

    const auto elapsed = juce::Time::highResolutionTicksToSeconds (
        juce::Time::getHighResolutionTicks() - startTicks);
    const double blockSeconds = static_cast<double> (samples)
                              / juce::jmax (1.0, currentSampleRate.load (std::memory_order_relaxed));
    const float instant = blockSeconds > 0.0 ? static_cast<float> (elapsed / blockSeconds) : 0.0f;
    const float previous = cpuLoad.load (std::memory_order_relaxed);
    cpuLoad.store (previous * 0.92f + instant * 0.08f, std::memory_order_relaxed);
}

void WarlockAIAudioProcessor::requestToneAnalysis()
{
    analysisRequested.store (true, std::memory_order_relaxed);
    notify();
}

void WarlockAIAudioProcessor::requestAutoBuildTone()
{
    autoBuildRequested.store (true, std::memory_order_relaxed);
    analysisRequested.store (true, std::memory_order_relaxed);
    notify();
}

void WarlockAIAudioProcessor::requestMixFitPreview()
{
    mixFitRequested.store (true, std::memory_order_relaxed);
    analysisRequested.store (true, std::memory_order_relaxed);
    notify();
}

void WarlockAIAudioProcessor::requestToneHealth()
{
    requestToneAnalysis();
}

CurrentToneState WarlockAIAudioProcessor::collectToneState() const
{
    CurrentToneState s;
    s.drive = loadParam (pDrive, 0.45f);
    s.tone = loadParam (pTone, 0.55f);
    s.tightness = loadParam (pTightness, 0.4f);
    s.saturation = loadParam (pSaturation, 0.35f);
    s.warmth = loadParam (pWarmth, 0.3f);
    s.bite = loadParam (pBite, 0.25f);
    s.gateThreshold = loadParam (pGateThresh, -48.0f);
    s.bass = loadParam (pEqLowGain);
    s.mid = loadParam (pEqLowMidGain);
    s.treble = loadParam (pEqHighGain);
    s.presence = loadParam (pEqHighMidGain);
    s.ampGain = loadParam (pAmpGain, 0.55f);
    s.ampBass = loadParam (pAmpBass, 1.5f);
    s.ampMid = loadParam (pAmpMid, -1.0f);
    s.ampTreble = loadParam (pAmpTreble, 2.0f);
    s.ampPresence = loadParam (pAmpPresence, 1.0f);
    s.cabLowCut = loadParam (pCabLowCut, 80.0f);
    s.cabHighCut = loadParam (pCabHighCut, 6500.0f);
    s.compressorThreshold = loadParam (pCompThresh, -18.0f);
    s.compressorRatio = loadParam (pCompRatio, 3.0f);
    s.distortionMode = juce::roundToInt (loadParam (pDistMode, 3.0f));
    s.ampMode = juce::roundToInt (loadParam (pAmpMode, 4.0f));
    return s;
}

void WarlockAIAudioProcessor::storeUndoSnapshot()
{
    undoValues.clear();
    for (auto* param : getParameters())
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (param))
            undoValues[ranged->paramID] = ranged->convertFrom0to1 (ranged->getValue());
    }
}

void WarlockAIAudioProcessor::applyParameterMapOnMessageThread (const std::map<juce::String, float>& values, bool captureUndo)
{
    juce::MessageManager::callAsync ([this, values, captureUndo]
    {
        if (captureUndo && undoValues.empty())
            storeUndoSnapshot();

        for (const auto& entry : values)
        {
            if (auto* parameter = dynamic_cast<juce::RangedAudioParameter*> (apvts.getParameter (entry.first)))
            {
                const float converted = parameter->convertTo0to1 (entry.second);
                parameter->beginChangeGesture();
                parameter->setValueNotifyingHost (converted);
                parameter->endChangeGesture();
            }
        }
    });
}

void WarlockAIAudioProcessor::commitMixFit()
{
    previewActive.store (false, std::memory_order_relaxed);
    std::lock_guard lock (snapshotMutex);
    snapshot.previewActive = false;
    snapshot.appliedMixFit = true;
}

void WarlockAIAudioProcessor::undoLastAiEdit()
{
    const auto restore = undoValues;
    if (restore.empty())
        return;

    undoValues.clear();
    previewActive.store (false, std::memory_order_relaxed);
    applyParameterMapOnMessageThread (restore, false);
    std::lock_guard lock (snapshotMutex);
    snapshot.previewActive = false;
    snapshot.appliedMixFit = false;
}

bool WarlockAIAudioProcessor::copyCapture (juce::AudioBuffer<float>& dest, int minSamples)
{
    const int available = captureFifo.getNumReady();
    if (available < minSamples)
        return false;

    const int toRead = juce::jmin (available, captureCapacity);
    dest.setSize (1, toRead, false, false, true);
    dest.clear();

    int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
    captureFifo.prepareToRead (toRead, start1, size1, start2, size2);

    if (size1 > 0)
        dest.copyFrom (0, 0, captureRing.data() + start1, size1);
    if (size2 > 0)
        dest.copyFrom (0, size1, captureRing.data() + start2, size2);

    captureFifo.finishedRead (toRead);
    return true;
}

AnalysisSnapshot WarlockAIAudioProcessor::getAnalysisSnapshot() const
{
    std::lock_guard lock (snapshotMutex);
    return snapshot;
}

AIResultPack WarlockAIAudioProcessor::getAiPack() const
{
    return aiEngine.copyPublished();
}

void WarlockAIAudioProcessor::applyMixFitOnMessageThread (const MixFitSuggestion& suggestion)
{
    applyParameterMapOnMessageThread (suggestion.parameterValues);
}

void WarlockAIAudioProcessor::run()
{
    while (! threadShouldExit())
    {
        const bool analyse = analysisRequested.exchange (false);
        const bool autoBuild = autoBuildRequested.exchange (false);
        const bool mixFit = mixFitRequested.exchange (false);
        const auto now = juce::Time::currentTimeMillis();
        const bool adaptiveMode = juce::roundToInt (loadParam (pDistMode, 3.0f))
            == static_cast<int> (DistortionMode::AdaptiveAI);

        if (! analyse && ! autoBuild && ! mixFit)
        {
            if (adaptiveMode && captureFifo.getNumReady() >= 4096 && ! aiEngine.shouldSkip (now, 400))
            {
                juce::AudioBuffer<float> slice;
                if (copyCapture (slice, 4096))
                {
                    const auto hints = aiEngine.lightHints (slice);
                    adaptiveBrightness.store (hints.brightness, std::memory_order_relaxed);
                    adaptiveDensity.store (hints.density, std::memory_order_relaxed);
                    adaptiveDynamics.store (hints.dynamics, std::memory_order_relaxed);
                    adaptiveRmsDb.store (hints.rmsDb, std::memory_order_relaxed);
                    adaptivePeakDb.store (hints.peakDb, std::memory_order_relaxed);
                    adaptivePicking.store (hints.picking, std::memory_order_relaxed);
                }
            }
            wait (200);
            continue;
        }

        analysisBusy.store (true, std::memory_order_relaxed);
        {
            std::lock_guard lock (snapshotMutex);
            snapshot.analysing = true;
        }

        juce::AudioBuffer<float> offline;
        if (! copyCapture (offline, 2048))
        {
            analysisRequested.store (true, std::memory_order_relaxed);
            if (autoBuild)
                autoBuildRequested.store (true, std::memory_order_relaxed);
            if (mixFit)
                mixFitRequested.store (true, std::memory_order_relaxed);
            analysisBusy.store (false, std::memory_order_relaxed);
            wait (80);
            continue;
        }

        const auto current = collectToneState();
        const auto mode = static_cast<DistortionMode> (
            juce::jlimit (0, static_cast<int> (DistortionMode::NumModes) - 1, current.distortionMode));
        const auto pack = aiEngine.analyse (offline, current, mode);

        adaptiveBrightness.store (pack.hints.brightness, std::memory_order_relaxed);
        adaptiveDensity.store (pack.hints.density, std::memory_order_relaxed);
        adaptiveDynamics.store (pack.hints.dynamics, std::memory_order_relaxed);
        adaptiveRmsDb.store (pack.hints.rmsDb, std::memory_order_relaxed);
        adaptivePeakDb.store (pack.hints.peakDb, std::memory_order_relaxed);
        adaptivePicking.store (pack.hints.picking, std::memory_order_relaxed);

        {
            std::lock_guard lock (snapshotMutex);
            snapshot.features = pack.features;
            snapshot.analysis = pack.analysis;
            snapshot.analysisResult = pack.analysisResult;
            snapshot.health = pack.health;
            snapshot.healthResult = pack.healthResult;
            snapshot.mixFit = pack.mixFit.suggestion;
            snapshot.mixFitResult = pack.mixFit;
            snapshot.recommendation = pack.recommendation;
            snapshot.style = pack.style;
            snapshot.ready = pack.ready;
            snapshot.analysing = false;
            snapshot.appliedMixFit = autoBuild || mixFit;
            snapshot.previewActive = mixFit;
        }

        if (autoBuild)
        {
            applyParameterMapOnMessageThread (pack.recommendation.parameterValues);
            previewActive.store (true, std::memory_order_relaxed);
        }
        else if (mixFit)
        {
            applyMixFitOnMessageThread (pack.mixFit.suggestion);
            previewActive.store (true, std::memory_order_relaxed);
        }

        analysisBusy.store (false, std::memory_order_relaxed);
    }
}

void WarlockAIAudioProcessor::getSpectrumMagnitudes (std::array<float, spectrumBins>& dest)
{
    if (spectrumFft == nullptr)
    {
        dest.fill (0.0f);
        return;
    }

    const int fftSize = spectrumFft->getSize();
    if (static_cast<int> (spectrumTimeDomain.size()) < fftSize
        || static_cast<int> (spectrumWork.size()) < fftSize * 2)
    {
        dest.fill (0.0f);
        return;
    }

    std::fill (spectrumTimeDomain.begin(), spectrumTimeDomain.begin() + fftSize, 0.0f);

    int available = spectrumFifo.getNumReady();
    if (available <= 0)
    {
        std::lock_guard lock (spectrumMutex);
        dest = spectrumMags;
        return;
    }

    const int toRead = juce::jmin (available, fftSize);
    int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
    spectrumFifo.prepareToRead (toRead, start1, size1, start2, size2);

    int outIndex = fftSize - toRead;
    for (int i = 0; i < size1; ++i)
        spectrumTimeDomain[static_cast<size_t> (outIndex++)] = spectrumRing[static_cast<size_t> (start1 + i)];
    for (int i = 0; i < size2; ++i)
        spectrumTimeDomain[static_cast<size_t> (outIndex++)] = spectrumRing[static_cast<size_t> (start2 + i)];

    spectrumFifo.finishedRead (toRead);

    std::fill (spectrumWork.begin(), spectrumWork.begin() + fftSize * 2, 0.0f);
    std::copy (spectrumTimeDomain.begin(), spectrumTimeDomain.begin() + fftSize, spectrumWork.begin());
    spectrumWindow->multiplyWithWindowingTable (spectrumWork.data(), static_cast<size_t> (fftSize));
    spectrumFft->performFrequencyOnlyForwardTransform (spectrumWork.data());

    std::array<float, spectrumBins> fresh {};
    const int bins = fftSize / 2;
    const float scale = 2.0f / static_cast<float> (fftSize);

    for (int i = 0; i < spectrumBins; ++i)
    {
        const int src = 1 + static_cast<int> ((static_cast<float> (i) / static_cast<float> (spectrumBins)) * (bins - 2));
        fresh[static_cast<size_t> (i)] = juce::jlimit (0.0f, 1.0f,
            std::sqrt (spectrumWork[static_cast<size_t> (src)] * scale));
    }

    {
        std::lock_guard lock (spectrumMutex);
        for (int i = 0; i < spectrumBins; ++i)
            spectrumMags[static_cast<size_t> (i)] = spectrumMags[static_cast<size_t> (i)] * 0.65f
                                                 + fresh[static_cast<size_t> (i)] * 0.35f;
        dest = spectrumMags;
    }
}

void WarlockAIAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void WarlockAIAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* WarlockAIAudioProcessor::createEditor()
{
    return new WarlockAIAudioProcessorEditor (*this);
}
} // namespace WarlockAI

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WarlockAI::WarlockAIAudioProcessor();
}

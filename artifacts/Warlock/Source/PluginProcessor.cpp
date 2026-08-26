#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    juce::File resolveResourceFile (const juce::String& fileName)
    {
        const juce::File exe = juce::File::getSpecialLocation (juce::File::currentExecutableFile);
        const juce::File parent = exe.getParentDirectory();
        const juce::File contents = parent.getParentDirectory();

        const juce::File candidates[] = {
            parent.getChildFile ("Resources").getChildFile (fileName),
            contents.getChildFile ("Resources").getChildFile (fileName),
            juce::File::getSpecialLocation (juce::File::currentApplicationFile)
                .getParentDirectory().getChildFile ("Resources").getChildFile (fileName)
        };

        for (const auto& candidate : candidates)
            if (candidate.existsAsFile())
                return candidate;

        return {};
    }
}

WarlockAudioProcessor::WarlockAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "WarlockSatanicParameters", createParameterLayout())
{
    oversampler = std::make_unique<juce::dsp::Oversampling<float>> (
        2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseAllpass, true, true
    );
}

WarlockAudioProcessor::~WarlockAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout WarlockAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_BOOST_ID, 1), PARAM_BOOST_NAME, juce::NormalisableRange<float> (0.0f, 15.0f, 0.1f, 1.0f), 6.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_GAIN_ID, 1), PARAM_GAIN_NAME, juce::NormalisableRange<float> (0.0f, 50.0f, 0.1f, 0.5f), 45.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_COLD_ID, 1), PARAM_COLD_NAME, juce::NormalisableRange<float> (-12.0f, 15.0f, 0.1f, 1.0f), 12.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_GRIM_ID, 1), PARAM_GRIM_NAME, juce::NormalisableRange<float> (-15.0f, 15.0f, 0.1f, 1.0f), 10.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_GATE_ID, 1), PARAM_GATE_NAME, juce::NormalisableRange<float> (-100.0f, -20.0f, 0.5f, 1.0f), -65.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_GATERELEASE_ID, 1), PARAM_GATERELEASE_NAME, juce::NormalisableRange<float> (5.0f, 200.0f, 1.0f, 0.5f), 20.0f));

    // NEW AUDIO CONTROL INITIALIZERS
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_SUB_ID, 1), PARAM_SUB_NAME, juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.0f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_PSYCHOSIS_ID, 1), PARAM_PSYCHOSIS_NAME, juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.0f), 0.0f));

    juce::StringArray ritualChoices = { "Symmetrical Leviathan", "Asymmetric Abomination", "Luciferian LED Headroom" };
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID (PARAM_DIODE_ID, 1), PARAM_DIODE_NAME, ritualChoices, 0));

    juce::StringArray altarChoices = { "Bypass Temple", "Crypt Void Center", "Catacomb Edge Blast" };
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID (PARAM_CAB_MODE_ID, 1), PARAM_CAB_MODE_NAME, altarChoices, 1));

    juce::StringArray soulChoices = { "Forced Desecration (Mono)", "True Stereo Pandemonium", "Ego Dissolution Phase (12ms)" };
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID (PARAM_ROUTING_ID, 1), PARAM_ROUTING_NAME, soulChoices, 1));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_EQ_LOW_ID, 1), PARAM_EQ_LOW_NAME, juce::NormalisableRange<float> (-15.0f, 15.0f, 0.1f, 1.0f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_EQ_MID_ID, 1), PARAM_EQ_MID_NAME, juce::NormalisableRange<float> (-15.0f, 15.0f, 0.1f, 1.0f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_EQ_HIGH_ID, 1), PARAM_EQ_HIGH_NAME, juce::NormalisableRange<float> (-15.0f, 15.0f, 0.1f, 1.0f), 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (PARAM_LEVEL_ID, 1), PARAM_LEVEL_NAME, juce::NormalisableRange<float> (-24.0f, 6.0f, 0.1f, 1.0f), 0.0f));

    return layout;
}

void WarlockAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;
    const int numChannels = juce::jmax (2, getTotalNumOutputChannels());

    oversampler->initProcessing (static_cast<size_t> (samplesPerBlock));
    double internalSampleRate = sampleRate * oversampler->getOversamplingFactor();
    preFilters.resize (static_cast<size_t> (numChannels));
    boostFilters.resize (static_cast<size_t> (numChannels));
    postFiltersHigh.resize (static_cast<size_t> (numChannels));
    postFiltersMid.resize (static_cast<size_t> (numChannels));
    postFiltersFixed.resize (static_cast<size_t> (numChannels));

    parametricLowFilters.resize (static_cast<size_t> (numChannels));
    parametricMidFilters.resize (static_cast<size_t> (numChannels));
    parametricHighFilters.resize (static_cast<size_t> (numChannels));

    envelopeStates.assign (static_cast<size_t> (numChannels), 0.0f);
    lastInputSamples.assign (static_cast<size_t> (numChannels), 0.0f);
    subPhaseStates.assign (static_cast<size_t> (numChannels), 1);
    subFilterStates.assign (static_cast<size_t> (numChannels), 0.0f);
    currentVoltageSag.store (1.0f);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (numChannels);
    cabinetConvolution.prepare (spec);
    lastCabinetSelection = -1;
    updateCabinetFile();
    for (int i = 0; i < numChannels; ++i)
    {
        preFilters[static_cast<size_t>(i)].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass (internalSampleRate, 340.0f, 0.707f);
        preFilters[static_cast<size_t>(i)].reset();

        postFiltersFixed[static_cast<size_t>(i)].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter (internalSampleRate, 1250.0f, 3.2f, juce::Decibels::decibelsToGain(14.5f));
        postFiltersFixed[static_cast<size_t>(i)].reset();

        boostFilters[static_cast<size_t>(i)].reset();
        postFiltersHigh[static_cast<size_t>(i)].reset();
        postFiltersMid[static_cast<size_t>(i)].reset();

        parametricLowFilters[static_cast<size_t>(i)].reset();
        parametricMidFilters[static_cast<size_t>(i)].reset();
        parametricHighFilters[static_cast<size_t>(i)].reset();
    }
}

void WarlockAudioProcessor::updateCabinetFile()
{
    int currentSelection = static_cast<int>(apvts.getRawParameterValue(PARAM_CAB_MODE_ID)->load());
    if (currentSelection == lastCabinetSelection) return;

    lastCabinetSelection = currentSelection;
    if (currentSelection == 0)
    {
        cabinetConvolution.reset();
        return;
    }

    juce::File targetIR = resolveResourceFile (currentSelection == 1 ? "Cab_Center.wav" : "Cab_Edge.wav");

    if (targetIR.existsAsFile())
    {
        cabinetConvolution.loadImpulseResponse(targetIR, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
    }
}

void WarlockAudioProcessor::releaseResources()
{
    oversampler->reset();
    cabinetConvolution.reset();
}

bool WarlockAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return (layouts.getMainOutput() == juce::AudioChannelSet::mono() || layouts.getMainOutput() == juce::AudioChannelSet::stereo()) && (layouts.getMainInput() == layouts.getMainOutput());
}

void WarlockAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);
    updateCabinetFile();
    const int routingMode = static_cast<int>(apvts.getRawParameterValue(PARAM_ROUTING_ID)->load());
    const float boostGain = juce::Decibels::decibelsToGain (apvts.getRawParameterValue(PARAM_BOOST_ID)->load());
    const float inputGain = juce::Decibels::decibelsToGain (apvts.getRawParameterValue(PARAM_GAIN_ID)->load());
    const float gateThresh = juce::Decibels::decibelsToGain (apvts.getRawParameterValue(PARAM_GATE_ID)->load());
    const float gateRelMS = apvts.getRawParameterValue(PARAM_GATERELEASE_ID)->load();
    const int diodeMode = static_cast<int>(apvts.getRawParameterValue(PARAM_DIODE_ID)->load());
    const int cabMode = static_cast<int>(apvts.getRawParameterValue(PARAM_CAB_MODE_ID)->load());

    const float eqLowGain = apvts.getRawParameterValue(PARAM_EQ_LOW_ID)->load();
    const float eqMidGain = apvts.getRawParameterValue(PARAM_EQ_MID_ID)->load();
    const float eqHighGain = apvts.getRawParameterValue(PARAM_EQ_HIGH_ID)->load();
    const float finalLevel = juce::Decibels::decibelsToGain (apvts.getRawParameterValue(PARAM_LEVEL_ID)->load());
    const float demonicSubMix = apvts.getRawParameterValue(PARAM_SUB_ID)->load();
    const float psychosisFactor = apvts.getRawParameterValue(PARAM_PSYCHOSIS_ID)->load();
    // SOUL CAPTURE SPATIAL INTERCEPT MATRIX
    if (routingMode == 0 && totalNumInputChannels > 1)
    {
        buffer.addFrom(0, 0, buffer.getReadPointer(1), numSamples, 0.5f);
        buffer.copyFrom(1, 0, buffer.getReadPointer(0), numSamples);
    }
    else if (routingMode == 2 && totalNumInputChannels > 1)
    {
        buffer.addFrom(0, 0, buffer.getReadPointer(1), numSamples, 0.5f);
        buffer.copyFrom(1, 0, buffer.getReadPointer(0), numSamples);

        float* rightChannelData = buffer.getWritePointer(1);
        int delayOffsetSamples = static_cast<int>(currentSampleRate * 0.012);
        if (numSamples > delayOffsetSamples)
        {
            for (int s = numSamples - 1; s >= delayOffsetSamples; --s)
                rightChannelData[s] = rightChannelData[s - delayOffsetSamples];
            for (int s = 0; s < delayOffsetSamples; ++s)
                rightChannelData[s] *= 0.5f;
        }
    }
    const float attackCoef = static_cast<float> (1.0 - std::exp(-1.0 / (currentSampleRate * 0.001)));
    const float releaseCoef = static_cast<float> (1.0 - std::exp(-1.0 / (currentSampleRate * (gateRelMS / 1000.0))));
    double internalSampleRate = currentSampleRate * oversampler->getOversamplingFactor();

    auto boostCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter (internalSampleRate, 750.0f, 1.1f, boostGain);
    auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter (internalSampleRate, 4200.0f, 1.9f, juce::Decibels::decibelsToGain(apvts.getRawParameterValue(PARAM_COLD_ID)->load()));
    auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter (internalSampleRate, 980.0f, 2.3f, juce::Decibels::decibelsToGain(apvts.getRawParameterValue(PARAM_GRIM_ID)->load()));
    auto eqLowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf (currentSampleRate, 130.0f, 0.75f, juce::Decibels::decibelsToGain(eqLowGain));
    auto eqMidCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 666.0f, 1.3f, juce::Decibels::decibelsToGain(eqMidGain)); // Satanic Mid Center Frequency Tuning
    auto eqHighCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 6200.0f, 0.75f, juce::Decibels::decibelsToGain(eqHighGain));

    // SACRIFICE GATE INPUT STAGE
    const int gateChannels = juce::jmin (totalNumInputChannels, (int) envelopeStates.size());
    for (int channel = 0; channel < gateChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        float env = envelopeStates[static_cast<size_t> (channel)];
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float inputMag = std::abs (channelData[sample]);
            if (inputMag > env) env += attackCoef * (inputMag - env);
            else env += releaseCoef * (inputMag - env);
            const float gateGain = (env > gateThresh) ? 1.0f : 0.0f;
            channelData[sample] *= gateGain;
        }
        envelopeStates[static_cast<size_t> (channel)] = env;
    }

    // DEMONIC SUB HARMONIC OCTAVE GENERATOR TRACKER
    if (demonicSubMix > 0.001f)
    {
        const int subChannels = juce::jmin (totalNumInputChannels, (int) subPhaseStates.size());
        for (int channel = 0; channel < subChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer (channel);
            float lastInput = lastInputSamples[static_cast<size_t> (channel)];
            int phase = subPhaseStates[static_cast<size_t> (channel)];
            float filtState = subFilterStates[static_cast<size_t> (channel)];
            for (int sample = 0; sample < numSamples; ++sample)
            {
                const float currentInput = channelData[sample];
                if (currentInput > 0.0f && lastInput <= 0.0f)
                    phase = -phase;
                lastInput = currentInput;
                const float subSquare = (phase > 0) ? 1.0f : -1.0f;
                const float subSignal = subSquare * std::abs (currentInput);
                filtState += 0.08f * (subSignal - filtState);
                channelData[sample] = (currentInput * (1.0f - demonicSubMix)) + (filtState * demonicSubMix * 1.35f);
            }
            lastInputSamples[static_cast<size_t> (channel)] = lastInput;
            subPhaseStates[static_cast<size_t> (channel)] = phase;
            subFilterStates[static_cast<size_t> (channel)] = filtState;
        }
    }

    // POSSESSION NONLINEAR OVERLOAD MATRIX WITH PSYCHOSIS VOLTAGE SAG
    juce::dsp::AudioBlock<float> inputBlock (buffer);
    juce::dsp::AudioBlock<float> oversampledBlock = oversampler->processSamplesUp (inputBlock);
    const int osChannels = (int) oversampledBlock.getNumChannels();
    const int numOversampledSamples = (int) oversampledBlock.getNumSamples();
    const int filterCount = (int) preFilters.size();

    for (int channel = 0; channel < osChannels; ++channel)
    {
        const size_t fi = static_cast<size_t> (juce::jmin (channel, juce::jmax (0, filterCount - 1)));
        float* channelData = oversampledBlock.getChannelPointer (static_cast<size_t> (channel));
        auto& preFilt = preFilters[fi];
        auto& bstFilt = boostFilters[fi];
        auto& posHigh = postFiltersHigh[fi];
        auto& posMid = postFiltersMid[fi];
        auto& posTarget = postFiltersFixed[fi];
        bstFilt.coefficients = boostCoeffs;
        posHigh.coefficients = highCoeffs;
        posMid.coefficients = midCoeffs;

        for (int sample = 0; sample < numOversampledSamples; ++sample)
        {
            float sag = currentVoltageSag.load (std::memory_order_relaxed);
            if (psychosisFactor > 0.001f && occultRandom.nextFloat() > 0.985f)
            {
                const float randomDrop = occultRandom.nextFloat() * psychosisFactor * 0.45f;
                sag = 1.0f - randomDrop;
            }
            else
            {
                sag += 0.005f * (1.0f - sag);
            }
            currentVoltageSag.store (sag, std::memory_order_relaxed);

            float x = preFilt.processSample (channelData[sample]) * inputGain * sag;
            x = bstFilt.processSample (x);

            const float posCeil = 0.30f * sag;
            const float negCeil = -0.80f * sag;
            const float symCeil = 0.60f * sag;
            if (diodeMode == 1)
            {
                if (x > posCeil) x = posCeil;
                else if (x < negCeil) x = negCeil;
            }
            else if (diodeMode == 2)
            {
                const float ledBound = 1.2f * sag;
                if (x > ledBound) x = ledBound;
                else if (x < -ledBound) x = -ledBound;
                x = std::tanh (x) * 0.80f;
            }
            else
            {
                if (x > symCeil) x = symCeil;
                else if (x < -symCeil) x = -symCeil;
            }

            x = posTarget.processSample (x);
            x = posMid.processSample (x);
            channelData[sample] = posHigh.processSample (x);
        }
    }

    oversampler->processSamplesDown (inputBlock);

    if (cabMode > 0)
    {
        juce::dsp::ProcessContextReplacing<float> context (inputBlock);
        cabinetConvolution.process (context);
    }

    // PARAMETRIC MIND-CONTROL RITUAL EQ EXECUTIONS
    const int eqChannels = juce::jmin (totalNumInputChannels, filterCount);
    for (int channel = 0; channel < eqChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        auto& lowEQ = parametricLowFilters[static_cast<size_t> (channel)];
        auto& midEQ = parametricMidFilters[static_cast<size_t> (channel)];
        auto& highEQ = parametricHighFilters[static_cast<size_t> (channel)];
        lowEQ.coefficients = eqLowCoeffs;
        midEQ.coefficients = eqMidCoeffs;
        highEQ.coefficients = eqHighCoeffs;
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float s = channelData[sample];
            s = lowEQ.processSample (s);
            s = midEQ.processSample (s);
            channelData[sample] = highEQ.processSample (s);
        }
    }

    buffer.applyGain (finalLevel);
}

juce::AudioProcessorEditor* WarlockAudioProcessor::createEditor()
{
    return new WarlockAudioProcessorEditor (*this);
}

void WarlockAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void WarlockAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary (data, sizeInBytes))
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WarlockAudioProcessor();
}


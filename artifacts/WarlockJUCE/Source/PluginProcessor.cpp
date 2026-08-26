#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include <cmath>

WarlockAudioProcessor::WarlockAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

WarlockAudioProcessor::~WarlockAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout WarlockAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_TRIM_ID, 1), PARAM_TRIM_NAME,
        juce::NormalisableRange<float> (-18.0f, 18.0f, 0.1f, 1.0f), 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_GAIN_ID, 1), PARAM_GAIN_NAME,
        juce::NormalisableRange<float> (0.0f, 36.0f, 0.1f, 0.5f), 28.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_COLD_ID, 1), PARAM_COLD_NAME,
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f, 1.0f), 8.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_GRIM_ID, 1), PARAM_GRIM_NAME,
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f, 1.0f), 6.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_GATE_ID, 1), PARAM_GATE_NAME,
        juce::NormalisableRange<float> (-80.0f, -20.0f, 0.5f, 1.0f), -50.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_GATERELEASE_ID, 1), PARAM_GATERELEASE_NAME,
        juce::NormalisableRange<float> (5.0f, 200.0f, 1.0f, 0.5f), 12.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_GATESLEW_ID, 1), PARAM_GATESLEW_NAME,
        juce::NormalisableRange<float> (0.5f, 20.0f, 0.1f, 0.5f), 4.0f));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (PARAM_DIODE_ID, 1), PARAM_DIODE_NAME, true));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (PARAM_CAB_ID, 1), PARAM_CAB_NAME, true));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID (PARAM_CABMODE_ID, 1), PARAM_CABMODE_NAME,
        juce::StringArray { "Crypt Center 4x12", "Catacomb Edge 4x12", "Bedroom Lo-Fi" }, 0));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (PARAM_LOFI_ID, 1), PARAM_LOFI_NAME, false));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_LEVEL_ID, 1), PARAM_LEVEL_NAME,
        juce::NormalisableRange<float> (-24.0f, 6.0f, 0.1f, 1.0f), -6.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_DELAY_ID, 1), PARAM_DELAY_NAME,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 0.5f), 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (PARAM_REVERB_ID, 1), PARAM_REVERB_NAME,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 0.5f), 0.12f));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (PARAM_BYPASS_ID, 1), PARAM_BYPASS_NAME, false));

    return layout;
}

void WarlockAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    const int channels = juce::jmax (1, getTotalNumOutputChannels());

    oversampler = std::make_unique<juce::dsp::Oversampling<float>> (
        (size_t) channels, 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) channels };
    oversampler->initProcessing ((size_t) samplesPerBlock);
    reportedLatency = (int) std::lround (oversampler->getLatencyInSamples());
    setLatencySamples (reportedLatency);

    bypassDelay.setMaximumDelayInSamples (juce::jmax (1, reportedLatency + 8));
    bypassDelay.prepare (spec);
    bypassDelay.reset();
    bypassDelay.setDelay ((float) juce::jmax (0, reportedLatency));

    const double osRate = sampleRate * (double) oversampler->getOversamplingFactor();
    juce::dsp::ProcessSpec osSpec {
        osRate,
        (juce::uint32) (samplesPerBlock * (int) oversampler->getOversamplingFactor()),
        (juce::uint32) channels
    };

    auto resize = [&] (auto& v, const juce::dsp::ProcessSpec& s)
    {
        v.clear();
        v.resize ((size_t) channels);
        for (auto& f : v)
        {
            f.reset();
            f.prepare (s);
        }
    };

    resize (preFilters, osSpec);
    resize (postFiltersFixed, osSpec);
    resize (postFiltersMid, osSpec);
    resize (postFiltersHigh, osSpec);
    resize (lofiFilters, spec);
    envelopeStates.assign ((size_t) channels, 0.0f);
    gateGainStates.assign ((size_t) channels, 0.0f);
    dcStates.assign ((size_t) channels, 0.0f);

    lastCold = lastGrim = 1.0e9f;
    lastLofi = -1.0f;
    updateToneCoeffs();

    auto loadCab = [&] (juce::dsp::Convolution& cab, const void* data, size_t size)
    {
        cab.reset();
        cab.prepare (spec);
        cab.loadImpulseResponse (
            data, size,
            juce::dsp::Convolution::Stereo::no,
            juce::dsp::Convolution::Trim::yes,
            0,
            juce::dsp::Convolution::Normalise::yes);
    };
    loadCab (cabCenter,  BinaryData::Cab_Center_wav, (size_t) BinaryData::Cab_Center_wavSize);
    loadCab (cabEdge,    BinaryData::Cab_Edge_wav,   (size_t) BinaryData::Cab_Edge_wavSize);
    loadCab (cabBedroom, BinaryData::CabinetIR_wav,  (size_t) BinaryData::CabinetIR_wavSize);
    cabReady = true;

    echoDelay.setMaximumDelayInSamples (juce::jmax (1, (int) std::lround (sampleRate * 1.0)));
    echoDelay.prepare (spec);
    echoDelay.reset();
    echoTimeSamples = (float) (sampleRate * 0.320); // 320 ms slap
    echoDelay.setDelay (echoTimeSamples);

    chamber.prepare (spec);
    chamber.reset();
    juce::dsp::Reverb::Parameters rv;
    rv.roomSize   = 0.42f;
    rv.damping    = 0.55f;
    rv.wetLevel   = 0.0f;
    rv.dryLevel   = 1.0f;
    rv.width      = 0.85f;
    rv.freezeMode = 0.0f;
    chamber.setParameters (rv);
}

void WarlockAudioProcessor::releaseResources()
{
    if (oversampler != nullptr)
        oversampler->reset();
    cabCenter.reset();
    cabEdge.reset();
    cabBedroom.reset();
    bypassDelay.reset();
    echoDelay.reset();
    chamber.reset();
    cabReady = false;
}

bool WarlockAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void WarlockAudioProcessor::updateToneCoeffs()
{
    const float coldDb = apvts.getRawParameterValue (PARAM_COLD_ID)->load();
    const float grimDb = apvts.getRawParameterValue (PARAM_GRIM_ID)->load();
    const bool lofi = apvts.getRawParameterValue (PARAM_LOFI_ID)->load() > 0.5f;
    const double osRate = currentSampleRate * (double) oversampler->getOversamplingFactor();

    if (std::abs (coldDb - lastCold) < 0.05f
        && std::abs (grimDb - lastGrim) < 0.05f
        && (lofi ? 1.0f : 0.0f) == lastLofi)
        return;

    lastCold = coldDb;
    lastGrim = grimDb;
    lastLofi = lofi ? 1.0f : 0.0f;

    auto hp = juce::dsp::IIR::Coefficients<float>::makeHighPass (osRate, 320.0f, 0.707f);
    auto fix = juce::dsp::IIR::Coefficients<float>::makePeakFilter (osRate, 1200.0f, 3.0f, juce::Decibels::decibelsToGain (14.0f));
    auto mid = juce::dsp::IIR::Coefficients<float>::makePeakFilter (osRate, 950.0f, 2.2f, juce::Decibels::decibelsToGain (grimDb));
    auto hi  = juce::dsp::IIR::Coefficients<float>::makePeakFilter (osRate, 4100.0f, 1.8f, juce::Decibels::decibelsToGain (coldDb));
    auto lp  = juce::dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, lofi ? 4200.0f : 18000.0f, 0.7f);

    for (auto& f : preFilters)        f.coefficients = hp;
    for (auto& f : postFiltersFixed)  f.coefficients = fix;
    for (auto& f : postFiltersMid)    f.coefficients = mid;
    for (auto& f : postFiltersHigh)   f.coefficients = hi;
    for (auto& f : lofiFilters)       f.coefficients = lp;
}

void WarlockAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamples);

    if (oversampler == nullptr
        || apvts.getRawParameterValue (PARAM_BYPASS_ID)->load() >= 0.5f)
    {
        applyIntegerLatency (buffer);
        meterPeak.store (buffer.getMagnitude (0, numSamples));
        return;
    }

    const float trimDb   = apvts.getRawParameterValue (PARAM_TRIM_ID)->load();
    const float gainDb   = apvts.getRawParameterValue (PARAM_GAIN_ID)->load();
    const float gateDb   = apvts.getRawParameterValue (PARAM_GATE_ID)->load();
    const float relMs    = apvts.getRawParameterValue (PARAM_GATERELEASE_ID)->load();
    const float slewMs   = apvts.getRawParameterValue (PARAM_GATESLEW_ID)->load();
    const bool  useAsym  = apvts.getRawParameterValue (PARAM_DIODE_ID)->load() > 0.5f;
    const bool  runCab   = apvts.getRawParameterValue (PARAM_CAB_ID)->load() > 0.5f;
    const int   cabMode  = (int) apvts.getRawParameterValue (PARAM_CABMODE_ID)->load();
    const float levelDb  = apvts.getRawParameterValue (PARAM_LEVEL_ID)->load();
    const float delayMix = apvts.getRawParameterValue (PARAM_DELAY_ID)->load();
    const float reverbMix = apvts.getRawParameterValue (PARAM_REVERB_ID)->load();

    updateToneCoeffs();

    const float trimLin  = juce::Decibels::decibelsToGain (trimDb);
    const float inputGain = juce::Decibels::decibelsToGain (gainDb);
    const float thresh    = juce::Decibels::decibelsToGain (gateDb);
    const float finalLevel = juce::Decibels::decibelsToGain (levelDb);
    const float attackCoef = 1.0f - std::exp (-1.0f / (float) (currentSampleRate * 0.001));
    const float releaseCoef = 1.0f - std::exp (-1.0f / (float) (currentSampleRate * (relMs * 0.001)));
    const float gateSlew = 1.0f - std::exp (-1.0f / (float) (currentSampleRate * juce::jmax (0.0005f, slewMs * 0.001f)));

    while ((int) envelopeStates.size() < numCh)
        envelopeStates.push_back (0.0f);
    while ((int) gateGainStates.size() < numCh)
        gateGainStates.push_back (0.0f);

    // 1. Input trim + slewed gate (no hard open/close clicks)
    for (int ch = 0; ch < numCh; ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        float env = envelopeStates[(size_t) ch];
        float gg  = gateGainStates[(size_t) ch];
        for (int i = 0; i < numSamples; ++i)
        {
            float x = data[i] * trimLin;
            const float mag = std::abs (x);
            if (mag > env) env += attackCoef * (mag - env);
            else           env += releaseCoef * (mag - env);
            const float over = env / juce::jmax (thresh, 1.0e-8f);
            const float target = over >= 1.0f ? 1.0f : over * over * over * over;
            gg += gateSlew * (target - gg);
            data[i] = x * gg;
        }
        envelopeStates[(size_t) ch] = env;
        gateGainStates[(size_t) ch] = gg;
    }

    // 2. 4× oversampled diode + tone stack
    juce::dsp::AudioBlock<float> block (buffer);
    auto osBlock = oversampler->processSamplesUp (block);
    const int osN = (int) osBlock.getNumSamples();
    const int osCh = (int) osBlock.getNumChannels();

    while ((int) preFilters.size() < osCh)
    {
        preFilters.emplace_back();
        postFiltersFixed.emplace_back();
        postFiltersMid.emplace_back();
        postFiltersHigh.emplace_back();
        lastCold = 1.0e9f;
        updateToneCoeffs();
    }
    while ((int) dcStates.size() < osCh)
        dcStates.push_back (0.0f);

    const float dcR = 0.995f;

    for (int ch = 0; ch < osCh; ++ch)
    {
        auto* channelData = osBlock.getChannelPointer ((size_t) ch);
        auto& preFilt  = preFilters[(size_t) ch];
        auto& posFixed = postFiltersFixed[(size_t) ch];
        auto& posMid   = postFiltersMid[(size_t) ch];
        auto& posHigh  = postFiltersHigh[(size_t) ch];
        float dc = dcStates[(size_t) ch];

        for (int sample = 0; sample < osN; ++sample)
        {
            float x = preFilt.processSample (channelData[sample]) * inputGain;

            if (useAsym)
            {
                if (x > 0.35f) x = 0.35f + 0.22f * std::tanh ((x - 0.35f) * 1.8f);
                else if (x < -0.75f) x = -0.75f + 0.22f * std::tanh ((x + 0.75f) * 1.8f);
            }
            else
            {
                if (x > 0.65f) x = 0.65f + 0.22f * std::tanh ((x - 0.65f) * 1.8f);
                else if (x < -0.65f) x = -0.65f + 0.22f * std::tanh ((x + 0.65f) * 1.8f);
            }

            const float y = x - dc;
            dc = x + dcR * (dc - x);
            x = y;

            x = posFixed.processSample (x);
            x = posMid.processSample (x);
            channelData[sample] = posHigh.processSample (x);
        }
        dcStates[(size_t) ch] = dc;
    }

    oversampler->processSamplesDown (block);

    // 3. Lo-Fi LP
    while ((int) lofiFilters.size() < numCh)
    {
        lofiFilters.emplace_back();
        lastLofi = -1.0f;
        updateToneCoeffs();
    }

    for (int ch = 0; ch < numCh; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        auto& lp = lofiFilters[(size_t) ch];
        for (int i = 0; i < numSamples; ++i)
            data[i] = lp.processSample (data[i]);
    }

    // 4. Cabinet IR — Center 4x12 / Edge 4x12 / Bedroom
    if (runCab && cabReady)
    {
        juce::dsp::Convolution* cab = &cabCenter;
        if (cabMode == 1) cab = &cabEdge;
        else if (cabMode == 2) cab = &cabBedroom;

        if (cab->getCurrentIRSize() > 0)
        {
            juce::dsp::AudioBlock<float> cabBlock (buffer);
            juce::dsp::ProcessContextReplacing<float> context (cabBlock);
            cab->process (context);
        }
    }

    // 5. Delay (320 ms slap, feedback 0.28) then plate reverb
    {
        const float fb = 0.28f;
        echoDelay.setDelay (echoTimeSamples);
        for (int i = 0; i < numSamples; ++i)
        {
            for (int ch = 0; ch < numCh; ++ch)
            {
                float* data = buffer.getWritePointer (ch);
                const float dry = data[i];
                const float delayed = echoDelay.popSample (ch);
                echoDelay.pushSample (ch, dry + delayed * fb);
                data[i] = dry + delayed * delayMix;
            }
        }
    }

    if (reverbMix > 0.001f)
    {
        juce::dsp::Reverb::Parameters rv;
        rv.roomSize   = 0.42f;
        rv.damping    = 0.55f;
        rv.wetLevel   = reverbMix * 0.55f;
        rv.dryLevel   = 1.0f;
        rv.width      = 0.85f;
        rv.freezeMode = 0.0f;
        chamber.setParameters (rv);
        juce::dsp::AudioBlock<float> rvBlock (buffer);
        juce::dsp::ProcessContextReplacing<float> rvCtx (rvBlock);
        chamber.process (rvCtx);
    }

    buffer.applyGain (finalLevel);
    meterPeak.store (buffer.getMagnitude (0, numSamples));
}

void WarlockAudioProcessor::applyIntegerLatency (juce::AudioBuffer<float>& buffer)
{
    if (reportedLatency <= 0)
        return;

    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < numCh; ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
        {
            bypassDelay.pushSample (ch, data[i]);
            data[i] = bypassDelay.popSample (ch);
        }
    }
}

void WarlockAudioProcessor::processBlockBypassed (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    applyIntegerLatency (buffer);
    meterPeak.store (buffer.getMagnitude (0, buffer.getNumSamples()));
}

juce::AudioProcessorParameter* WarlockAudioProcessor::getBypassParameter() const
{
    return apvts.getParameter (PARAM_BYPASS_ID);
}

double WarlockAudioProcessor::getTailLengthSeconds() const
{
    double tail = 0.0;
    if (apvts.getRawParameterValue (PARAM_CAB_ID)->load() > 0.5f)
        tail = 0.006;
    if (apvts.getRawParameterValue (PARAM_DELAY_ID)->load() > 0.001f)
        tail = juce::jmax (tail, 1.2);
    if (apvts.getRawParameterValue (PARAM_REVERB_ID)->load() > 0.001f)
        tail = juce::jmax (tail, 2.5);
    return tail;
}

juce::AudioProcessorEditor* WarlockAudioProcessor::createEditor()
{
    return new WarlockAudioProcessorEditor (*this);
}

void WarlockAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void WarlockAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WarlockAudioProcessor();
}

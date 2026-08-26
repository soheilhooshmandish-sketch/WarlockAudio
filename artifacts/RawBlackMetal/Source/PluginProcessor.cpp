#include "PluginProcessor.h"

namespace
{
    float asymSoftClip (float x) noexcept
    {
        if (x > 0.0f)
            return std::tanh (x);

        const float ax = std::abs (x);
        return x / (1.0f + ax * 0.5f);
    }
}

RawBlackMetalAudioProcessor::RawBlackMetalAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    driveParam     = apvts.getRawParameterValue ("drive");
    reverbMixParam = apvts.getRawParameterValue ("reverb_mix");
    outputParam    = apvts.getRawParameterValue ("output");
}

RawBlackMetalAudioProcessor::~RawBlackMetalAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
RawBlackMetalAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID ("drive", 1), "Drive",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f, 0.5f), 72.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID ("reverb_mix", 1), "Reverb Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 15.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID ("output", 1), "Output",
        juce::NormalisableRange<float> (-24.0f, 6.0f, 0.1f), 0.0f));

    return layout;
}

void RawBlackMetalAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    const int channels = juce::jmax (1, getTotalNumOutputChannels());

    juce::dsp::ProcessSpec spec {
        currentSampleRate,
        (juce::uint32) juce::jmax (1, samplesPerBlock),
        (juce::uint32) channels
    };

    highPass.clear();
    lowPass.clear();
    highPass.resize ((size_t) channels);
    lowPass.resize ((size_t) channels);

    for (int ch = 0; ch < channels; ++ch)
    {
        highPass[(size_t) ch].prepare (spec);
        lowPass[(size_t) ch].prepare (spec);
        highPass[(size_t) ch].reset();
        lowPass[(size_t) ch].reset();
    }

    updateFilters();

    reverb.prepare (spec);
    reverb.reset();
    juce::dsp::Reverb::Parameters rv;
    rv.roomSize   = 0.85f;
    rv.damping    = 0.40f;
    rv.width      = 1.00f;
    rv.wetLevel   = 0.15f;
    rv.dryLevel   = 0.85f;
    rv.freezeMode = 0.0f;
    reverb.setParameters (rv);

    limiterEnv = 1.0f;
    limiterCeiling = juce::Decibels::decibelsToGain (kCeilingDb);
    limiterReleaseCoeff = std::exp (-1.0f / (float) (currentSampleRate * 0.020));
    setLatencySamples (0);
}

void RawBlackMetalAudioProcessor::releaseResources()
{
    reverb.reset();
    limiterDelay.reset();
    limiterEnv = 1.0f;
}

void RawBlackMetalAudioProcessor::updateFilters()
{
    auto hp = juce::dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, kHpfHz, 0.70710678f);
    auto lp = juce::dsp::IIR::Coefficients<float>::makeLowPass  (currentSampleRate, kLpfHz, 0.70710678f);

    for (auto& f : highPass)
        f.coefficients = hp;
    for (auto& f : lowPass)
        f.coefficients = lp;
}

bool RawBlackMetalAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return in == out;
}

void RawBlackMetalAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    const float drivePct   = driveParam != nullptr ? driveParam->load() : 72.0f;
    const float reverbPct  = reverbMixParam != nullptr ? reverbMixParam->load() : 15.0f;
    const float outputDb   = outputParam != nullptr ? outputParam->load() : 0.0f;

    const float driveDb    = juce::jlimit (0.0f, kMaxDriveDb, drivePct * 0.01f * kMaxDriveDb);
    const float driveLin   = juce::Decibels::decibelsToGain (driveDb);
    const float gainComp   = 1.0f / (1.0f + 0.3f * driveLin);
    const float wet        = juce::jlimit (0.0f, 1.0f, reverbPct * 0.01f);
    const float outLin     = juce::Decibels::decibelsToGain (outputDb);

    while ((int) highPass.size() < numCh)
    {
        highPass.emplace_back();
        lowPass.emplace_back();
        highPass.back().reset();
        lowPass.back().reset();
        updateFilters();
    }

    // 1–3. HPF → drive / asym clip / auto-gain → LPF
    for (int ch = 0; ch < numCh; ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        auto& hp = highPass[(size_t) ch];
        auto& lp = lowPass[(size_t) ch];

        for (int i = 0; i < numSamples; ++i)
        {
            float x = hp.processSample (data[i]);
            x = asymSoftClip (x * driveLin) * gainComp;
            data[i] = lp.processSample (x);
        }
    }

    // 4. Cavern reverb (wet/dry). Dry stays in the buffer; JUCE Reverb mixes in place.
    {
        juce::dsp::Reverb::Parameters rv;
        rv.roomSize   = 0.85f;
        rv.damping    = 0.40f;
        rv.width      = 1.00f;
        rv.wetLevel   = wet;
        rv.dryLevel   = 1.0f - wet;
        rv.freezeMode = 0.0f;
        reverb.setParameters (rv);

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        reverb.process (ctx);
    }

    buffer.applyGain (outLin);

    // 5. Lookahead brickwall at −0.3 dBFS
    processLimiter (buffer);
}

void RawBlackMetalAudioProcessor::processLimiter (juce::AudioBuffer<float>& buffer)
{
    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float attackCoeff = std::exp (-1.0f / (float) (currentSampleRate * 0.0008));

    for (int i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;
        for (int ch = 0; ch < numCh; ++ch)
            peak = juce::jmax (peak, std::abs (buffer.getSample (ch, i)));

        const float needed = peak > limiterCeiling ? limiterCeiling / juce::jmax (peak, 1.0e-8f) : 1.0f;
        const float coeff = needed < limiterEnv ? attackCoeff : limiterReleaseCoeff;
        limiterEnv = needed + coeff * (limiterEnv - needed);

        for (int ch = 0; ch < numCh; ++ch)
            buffer.setSample (ch, i, buffer.getSample (ch, i) * limiterEnv);
    }
}

double RawBlackMetalAudioProcessor::getTailLengthSeconds() const
{
    const float wet = reverbMixParam != nullptr ? reverbMixParam->load() * 0.01f : 0.15f;
    return wet > 0.001f ? 3.0 : 0.0;
}

juce::AudioProcessorEditor* RawBlackMetalAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void RawBlackMetalAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void RawBlackMetalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RawBlackMetalAudioProcessor();
}

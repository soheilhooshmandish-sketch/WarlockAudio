#pragma once
#include <JuceHeader.h>

#define PARAM_TRIM_ID "trim"
#define PARAM_TRIM_NAME "Trim"
#define PARAM_GAIN_ID "gain"
#define PARAM_GAIN_NAME "Gain"
#define PARAM_COLD_ID "cold"
#define PARAM_COLD_NAME "Cold (Highs)"
#define PARAM_GRIM_ID "grim"
#define PARAM_GRIM_NAME "Grim (Mids)"
#define PARAM_GATE_ID "gate"
#define PARAM_GATE_NAME "Gate Threshold"
#define PARAM_GATERELEASE_ID "gaterelease"
#define PARAM_GATERELEASE_NAME "Gate Release Time"
#define PARAM_GATESLEW_ID "gateslew"
#define PARAM_GATESLEW_NAME "Gate Slew"
#define PARAM_DIODE_ID "diode"
#define PARAM_DIODE_NAME "Asymmetric Clipping"
#define PARAM_CAB_ID "cab"
#define PARAM_CAB_NAME "Cabinet Emulation"
#define PARAM_CABMODE_ID "cabmode"
#define PARAM_CABMODE_NAME "Cabinet"
#define PARAM_LOFI_ID "lofi"
#define PARAM_LOFI_NAME "Lo-Fi"
#define PARAM_LEVEL_ID "level"
#define PARAM_LEVEL_NAME "Level"
#define PARAM_DELAY_ID "delay"
#define PARAM_DELAY_NAME "Delay"
#define PARAM_REVERB_ID "reverb"
#define PARAM_REVERB_NAME "Reverb"
#define PARAM_BYPASS_ID "bypass"
#define PARAM_BYPASS_NAME "Bypass"

class WarlockAudioProcessor : public juce::AudioProcessor
{
public:
    WarlockAudioProcessor();
    ~WarlockAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorParameter* getBypassParameter() const override;
    double getTailLengthSeconds() const override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Warlock"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    float getMeterPeak() const noexcept { return meterPeak.load(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateToneCoeffs();
    void applyIntegerLatency (juce::AudioBuffer<float>& buffer);

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    std::vector<juce::dsp::IIR::Filter<float>> preFilters;
    std::vector<juce::dsp::IIR::Filter<float>> postFiltersHigh;
    std::vector<juce::dsp::IIR::Filter<float>> postFiltersMid;
    std::vector<juce::dsp::IIR::Filter<float>> postFiltersFixed;
    std::vector<juce::dsp::IIR::Filter<float>> lofiFilters;

    std::vector<float> envelopeStates;
    std::vector<float> gateGainStates;
    std::vector<float> dcStates;
    juce::dsp::Convolution cabCenter;
    juce::dsp::Convolution cabEdge;
    juce::dsp::Convolution cabBedroom;
    bool cabReady = false;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> echoDelay;
    juce::dsp::Reverb chamber;
    float echoTimeSamples = 0.0f;

    double currentSampleRate = 44100.0;
    float lastCold = 1.0e9f;
    float lastGrim = 1.0e9f;
    float lastLofi = -1.0f;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> bypassDelay;
    int reportedLatency = 0;
    std::atomic<float> meterPeak { 0.0f };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarlockAudioProcessor)
};

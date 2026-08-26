#pragma once
#include <JuceHeader.h>
#define PARAM_BOOST_ID "invocation"
#define PARAM_BOOST_NAME "Invocation Boost"
#define PARAM_GAIN_ID "possession"
#define PARAM_GAIN_NAME "Possession Gain"
#define PARAM_COLD_ID "cold"
#define PARAM_COLD_NAME "Cold High Distortion"
#define PARAM_GRIM_ID "grim"
#define PARAM_GRIM_NAME "Grim Mid Distortion"
#define PARAM_GATE_ID "sacrifice"
#define PARAM_GATE_NAME "Sacrifice Noise Floor"
#define PARAM_GATERELEASE_ID "agonyrelease"
#define PARAM_GATERELEASE_NAME "Agony Release Time"
#define PARAM_DIODE_ID "diode"
#define PARAM_DIODE_NAME "Clipping Ritual Mode"
#define PARAM_CAB_MODE_ID "altar"
#define PARAM_CAB_MODE_NAME "Altar Cabinet Selection"
#define PARAM_ROUTING_ID "soulcapture"
#define PARAM_ROUTING_NAME "Soul Capture Matrix"
#define PARAM_SUB_ID "demonicsub"
#define PARAM_SUB_NAME "Demonic Sub-Octave Blend"
#define PARAM_PSYCHOSIS_ID "psychosis"
#define PARAM_PSYCHOSIS_NAME "Psychosis Voltage Instability"
#define PARAM_EQ_LOW_ID "despair"
#define PARAM_EQ_LOW_NAME "Despair Low Shelf Gain"
#define PARAM_EQ_MID_ID "agony"
#define PARAM_EQ_MID_NAME "Agony Mid Notch Gain"
#define PARAM_EQ_HIGH_ID "wrath"
#define PARAM_EQ_HIGH_NAME "Wrath High Shelf Gain"
#define PARAM_LEVEL_ID "curse"
#define PARAM_LEVEL_NAME "Curse Output Master Level"
class WarlockAudioProcessor : public juce::AudioProcessor
{
public:
    WarlockAudioProcessor();
    ~WarlockAudioProcessor() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Warlock"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void updateCabinetFile();
    float getActiveVoltageSagValue() const noexcept { return currentVoltageSag.load(); }
    juce::AudioProcessorValueTreeState apvts;
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
   
    std::vector<juce::dsp::IIR::Filter<float>> preFilters;
    std::vector<juce::dsp::IIR::Filter<float>> boostFilters;
    std::vector<juce::dsp::IIR::Filter<float>> postFiltersHigh;
    std::vector<juce::dsp::IIR::Filter<float>> postFiltersMid;
    std::vector<juce::dsp::IIR::Filter<float>> postFiltersFixed;
   
    std::vector<juce::dsp::IIR::Filter<float>> parametricLowFilters;
    std::vector<juce::dsp::IIR::Filter<float>> parametricMidFilters;
    std::vector<juce::dsp::IIR::Filter<float>> parametricHighFilters;
   
    std::vector<float> lastInputSamples;
    std::vector<int> subPhaseStates;
    std::vector<float> subFilterStates;
    juce::Random occultRandom;
    std::atomic<float> currentVoltageSag { 1.0f };
   
    std::vector<float> envelopeStates;
    juce::dsp::Convolution cabinetConvolution;
    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    int lastCabinetSelection = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarlockAudioProcessor)
};

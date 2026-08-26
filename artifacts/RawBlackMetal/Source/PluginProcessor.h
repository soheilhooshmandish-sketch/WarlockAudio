#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <vector>

class RawBlackMetalAudioProcessor : public juce::AudioProcessor
{
public:
    RawBlackMetalAudioProcessor();
    ~RawBlackMetalAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Raw Black Metal Distortion"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override;

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateFilters();
    void processLimiter (juce::AudioBuffer<float>& buffer);

    static constexpr float kHpfHz = 130.0f;
    static constexpr float kLpfHz = 7000.0f;
    static constexpr float kMaxDriveDb = 36.0f;
    static constexpr float kCeilingDb = -0.3f;
    static constexpr int   kLookaheadSamples = 64;

    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* reverbMixParam = nullptr;
    std::atomic<float>* outputParam = nullptr;

    double currentSampleRate = 44100.0;

    std::vector<juce::dsp::IIR::Filter<float>> highPass;
    std::vector<juce::dsp::IIR::Filter<float>> lowPass;

    juce::dsp::Reverb reverb;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> limiterDelay;
    float limiterEnv = 1.0f;
    float limiterCeiling = 0.96605088f; // 10^(-0.3/20)
    float limiterReleaseCoeff = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RawBlackMetalAudioProcessor)
};

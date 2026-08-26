#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
class SpectrumComponent final : public juce::Component,
                                private juce::Timer
{
public:
    explicit SpectrumComponent (WarlockAIAudioProcessor& processorToUse);
    ~SpectrumComponent() override;

    void paint (juce::Graphics& g) override;

private:
    void timerCallback() override;

    WarlockAIAudioProcessor& processor;
    std::array<float, WarlockAIAudioProcessor::spectrumBins> magnitudes {};
};
} // namespace WarlockAI

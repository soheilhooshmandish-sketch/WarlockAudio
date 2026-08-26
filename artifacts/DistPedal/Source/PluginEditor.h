#pragma once
#if __has_include(<JuceHeader.h>)
 #include <JuceHeader.h>
#else
 #include <juce_audio_processors/juce_audio_processors.h>
#endif
#include "PluginProcessor.h"

class DistPedalAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DistPedalAudioProcessorEditor (DistPedalAudioProcessor&);
    ~DistPedalAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DistPedalAudioProcessor& proc;
    juce::ComboBox modeBox, cabBox;
    juce::Slider drive, tone, level, boost, amp, mix;
    juce::Label modeL, cabL, driveL, toneL, levelL, boostL, ampL, mixL, title;

    using BoxA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SldA = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<BoxA> modeAt, cabAt;
    std::unique_ptr<SldA> driveAt, toneAt, levelAt, boostAt, ampAt, mixAt;

    void styleSlider (juce::Slider& s, juce::Label& l, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistPedalAudioProcessorEditor)
};

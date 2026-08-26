#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PedalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PedalLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Image chassisImage, knobImage, stompImage, logoImage;
};

class WarlockAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    WarlockAudioProcessorEditor (WarlockAudioProcessor&);
    ~WarlockAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    WarlockAudioProcessor& audioProcessor;
    PedalLookAndFeel lnf;

    juce::Slider gainSlider, coldSlider, grimSlider, gateSlider, releaseSlider, levelSlider,
                 trimSlider, delaySlider, reverbSlider;
    juce::ToggleButton diodeButton, cabButton, lofiButton, stompButton;
    juce::ComboBox cabModeBox;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> gainAttachment, coldAttachment, grimAttachment,
        gateAttachment, releaseAttachment, levelAttachment, trimAttachment,
        delayAttachment, reverbAttachment;
    std::unique_ptr<ButtonAttachment> diodeAttachment, cabAttachment, lofiAttachment;
    std::unique_ptr<ComboAttachment> cabModeAttachment;

    float ledPulse = 0.0f;
    float meterSmooth = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarlockAudioProcessorEditor)
};

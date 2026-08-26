#include "UI/WarlockKnob.h"

namespace WarlockAI
{
WarlockKnob::WarlockKnob (const juce::String& title, juce::Colour accent)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 16);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    addAndMakeVisible (slider);

    label.setText (title, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (label);
}

void WarlockKnob::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds (bounds.removeFromTop (16));
    slider.setBounds (bounds);
}
} // namespace WarlockAI

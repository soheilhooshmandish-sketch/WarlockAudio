#pragma once

#include <JuceHeader.h>

namespace WarlockAI
{
class WarlockKnob final : public juce::Component
{
public:
    WarlockKnob (const juce::String& title, juce::Colour accent);

    void resized() override;

    juce::Slider slider;
    juce::Label label;
};
} // namespace WarlockAI

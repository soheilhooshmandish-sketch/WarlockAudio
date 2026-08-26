#pragma once

#include <JuceHeader.h>

namespace WarlockAI
{
class MeterBar final : public juce::Component
{
public:
    explicit MeterBar (juce::Colour fillColour);

    void paint (juce::Graphics& g) override;
    void setLevel (float normalised) noexcept;
    void setCaption (const juce::String& text);

private:
    juce::Colour colour;
    float level = 0.0f;
    juce::String caption;
};
} // namespace WarlockAI

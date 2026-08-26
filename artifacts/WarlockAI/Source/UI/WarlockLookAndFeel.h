#pragma once

#include <JuceHeader.h>

namespace WarlockAI
{
struct ForgeColours
{
    juce::Colour background    { 0xff0b0c0e };
    juce::Colour panel         { 0xff14161a };
    juce::Colour panelAlt      { 0xff1a1d22 };
    juce::Colour steel         { 0xff6e737a };
    juce::Colour steelDark     { 0xff2b2f35 };
    juce::Colour bronze        { 0xffb08d57 };
    juce::Colour bronzeDim     { 0xff7a6240 };
    juce::Colour distortion    { 0xffc23b3b };
    juce::Colour ai            { 0xff7a6cf0 };
    juce::Colour aiCyan        { 0xff4f8fd4 };
    juce::Colour text          { 0xffe6e1d6 };
    juce::Colour muted         { 0xff8b8680 };
    juce::Colour outline       { 0xff2f343c };
};

class WarlockLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    WarlockLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getLabelFont (juce::Label&) override;

    const ForgeColours colours;
};
} // namespace WarlockAI

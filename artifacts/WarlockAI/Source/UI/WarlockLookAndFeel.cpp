#include "UI/WarlockLookAndFeel.h"

namespace WarlockAI
{
WarlockLookAndFeel::WarlockLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, colours.background);
    setColour (juce::Slider::rotarySliderFillColourId, colours.bronze);
    setColour (juce::Slider::rotarySliderOutlineColourId, colours.steelDark);
    setColour (juce::Slider::thumbColourId, colours.text);
    setColour (juce::Slider::textBoxTextColourId, colours.text);
    setColour (juce::Slider::textBoxOutlineColourId, colours.outline);
    setColour (juce::Slider::textBoxBackgroundColourId, colours.panelAlt);
    setColour (juce::Label::textColourId, colours.text);
    setColour (juce::TextButton::buttonColourId, colours.panelAlt);
    setColour (juce::TextButton::buttonOnColourId, colours.steelDark);
    setColour (juce::TextButton::textColourOffId, colours.text);
    setColour (juce::TextButton::textColourOnId, colours.bronze);
    setColour (juce::ComboBox::backgroundColourId, colours.panelAlt);
    setColour (juce::ComboBox::outlineColourId, colours.outline);
    setColour (juce::ComboBox::textColourId, colours.text);
    setColour (juce::ComboBox::arrowColourId, colours.bronze);
    setColour (juce::PopupMenu::backgroundColourId, colours.panel);
    setColour (juce::PopupMenu::textColourId, colours.text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, colours.steelDark);
    setColour (juce::PopupMenu::highlightedTextColourId, colours.bronze);
    setColour (juce::ToggleButton::textColourId, colours.text);
    setColour (juce::ToggleButton::tickColourId, colours.bronze);
    setColour (juce::ToggleButton::tickDisabledColourId, colours.steel);
}

void WarlockLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle,
                                           float rotaryEndAngle, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                                static_cast<float> (width), static_cast<float> (height))
                            .reduced (6.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    g.setColour (colours.steelDark);
    g.fillEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    juce::ColourGradient metal (colours.panel.brighter (0.18f), centre.x, centre.y - radius,
                                colours.background, centre.x, centre.y + radius, false);
    g.setGradientFill (metal);
    g.fillEllipse (centre.x - radius + 2.0f, centre.y - radius + 2.0f,
                   radius * 2.0f - 4.0f, radius * 2.0f - 4.0f);

    juce::Path arc;
    arc.addCentredArc (centre.x, centre.y, radius - 3.0f, radius - 3.0f,
                       0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent);
    g.strokePath (arc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));

    g.setColour (colours.bronze.withAlpha (0.35f));
    g.drawEllipse (centre.x - radius + 5.0f, centre.y - radius + 5.0f,
                   radius * 2.0f - 10.0f, radius * 2.0f - 10.0f, 1.0f);

    juce::Path pointer;
    const float pointerLength = radius * 0.62f;
    pointer.addRoundedRectangle (-1.4f, -pointerLength, 2.8f, pointerLength * 0.78f, 1.0f);
    g.setColour (colours.text);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));

    g.setColour (accent);
    g.fillEllipse (centre.x - 3.0f, centre.y - 3.0f, 6.0f, 6.0f);
}

void WarlockLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                               const juce::Colour&,
                                               bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    auto fill = colours.panelAlt;

    if (button.getToggleState())
        fill = colours.steelDark;
    if (highlighted)
        fill = fill.brighter (0.08f);
    if (down)
        fill = fill.darker (0.12f);

    g.setColour (fill);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (button.getToggleState() ? colours.bronze : colours.outline);
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
}

void WarlockLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                       int, int, int, int, juce::ComboBox&)
{
    auto bounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height));
    g.setColour (colours.panelAlt);
    g.fillRoundedRectangle (bounds, 3.0f);
    g.setColour (colours.outline);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 3.0f, 1.0f);

    juce::Path arrow;
    const float cx = static_cast<float> (width) - 14.0f;
    const float cy = static_cast<float> (height) * 0.5f;
    arrow.addTriangle (cx - 5.0f, cy - 3.0f, cx + 5.0f, cy - 3.0f, cx, cy + 4.0f);
    g.setColour (colours.bronze);
    g.fillPath (arrow);
}

void WarlockLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                           bool highlighted, bool)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto box = bounds.removeFromLeft (18.0f).withSizeKeepingCentre (14.0f, 14.0f);

    g.setColour (highlighted ? colours.panelAlt.brighter (0.1f) : colours.panelAlt);
    g.fillRoundedRectangle (box, 2.0f);
    g.setColour (colours.outline);
    g.drawRoundedRectangle (box, 2.0f, 1.0f);

    if (button.getToggleState())
    {
        g.setColour (colours.bronze);
        g.fillRoundedRectangle (box.reduced (3.0f), 1.5f);
    }

    g.setColour (colours.text);
    g.setFont (juce::FontOptions (12.5f));
    g.drawFittedText (button.getButtonText(), bounds.toNearestInt().reduced (4, 0),
                      juce::Justification::centredLeft, 1);
}

juce::Font WarlockLookAndFeel::getTextButtonFont (juce::TextButton&, int)
{
    return juce::Font (juce::FontOptions (13.0f));
}

juce::Font WarlockLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return juce::Font (juce::FontOptions (13.0f));
}

juce::Font WarlockLookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (juce::FontOptions (12.0f));
}
} // namespace WarlockAI

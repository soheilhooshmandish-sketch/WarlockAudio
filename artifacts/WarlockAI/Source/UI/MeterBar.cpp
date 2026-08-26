#include "UI/MeterBar.h"
#include "UI/WarlockLookAndFeel.h"

namespace WarlockAI
{
MeterBar::MeterBar (juce::Colour fillColour)
    : colour (fillColour)
{
}

void MeterBar::setLevel (float normalised) noexcept
{
    level = juce::jlimit (0.0f, 1.0f, normalised);
    repaint();
}

void MeterBar::setCaption (const juce::String& text)
{
    caption = text;
    repaint();
}

void MeterBar::paint (juce::Graphics& g)
{
    ForgeColours colours;
    auto bounds = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (colours.steelDark);
    g.fillRoundedRectangle (bounds, 3.0f);

    auto fill = bounds.reduced (2.0f);
    fill.setWidth (fill.getWidth() * level);
    g.setColour (colour);
    g.fillRoundedRectangle (fill, 2.0f);

    g.setColour (colours.text);
    g.setFont (juce::FontOptions (10.5f));
    g.drawText (caption, getLocalBounds().reduced (6, 0), juce::Justification::centredLeft);
}
} // namespace WarlockAI

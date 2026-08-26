#include "UI/SpectrumComponent.h"
#include "UI/WarlockLookAndFeel.h"

namespace WarlockAI
{
SpectrumComponent::SpectrumComponent (WarlockAIAudioProcessor& processorToUse)
    : processor (processorToUse)
{
    startTimerHz (24);
}

SpectrumComponent::~SpectrumComponent()
{
    stopTimer();
}

void SpectrumComponent::timerCallback()
{
    processor.getSpectrumMagnitudes (magnitudes);
    repaint();
}

void SpectrumComponent::paint (juce::Graphics& g)
{
    ForgeColours colours;
    auto bounds = getLocalBounds().toFloat();

    g.setColour (colours.panelAlt);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (colours.outline);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 4.0f, 1.0f);

    auto plot = bounds.reduced (8.0f, 10.0f);
    const float barWidth = plot.getWidth() / static_cast<float> (magnitudes.size());

    for (size_t i = 0; i < magnitudes.size(); ++i)
    {
        const float mag = juce::jlimit (0.0f, 1.0f, magnitudes[i]);
        const float h = mag * plot.getHeight();
        const float x = plot.getX() + static_cast<float> (i) * barWidth;
        const auto bar = juce::Rectangle<float> (x + 0.5f, plot.getBottom() - h, barWidth - 1.0f, h);

        const float t = static_cast<float> (i) / static_cast<float> (magnitudes.size() - 1);
        g.setColour (colours.distortion.interpolatedWith (colours.ai, t).withAlpha (0.85f));
        g.fillRect (bar);
    }

    g.setColour (colours.muted);
    g.setFont (juce::FontOptions (11.0f));
    g.drawText ("SPECTRUM", plot.removeFromTop (14.0f).toNearestInt(), juce::Justification::topLeft);
}
} // namespace WarlockAI

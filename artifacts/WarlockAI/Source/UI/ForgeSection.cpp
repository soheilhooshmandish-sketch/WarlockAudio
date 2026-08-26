#include "UI/ForgeSection.h"

namespace WarlockAI
{
ForgeSection::ForgeSection (const juce::String& sectionTitle, juce::Colour sectionAccent)
    : title (sectionTitle),
      accent (sectionAccent)
{
    disclosure.setClickingTogglesState (true);
    disclosure.setToggleState (true, juce::dontSendNotification);
    disclosure.setButtonText ("-");
    disclosure.onClick = [this]
    {
        setExpanded (disclosure.getToggleState());
        if (onExpandChanged)
            onExpandChanged();
    };
    addAndMakeVisible (disclosure);
    addAndMakeVisible (enable);
}

void ForgeSection::setExpanded (bool shouldExpand)
{
    expanded = shouldExpand;
    disclosure.setToggleState (expanded, juce::dontSendNotification);
    disclosure.setButtonText (expanded ? "-" : "+");
}

int ForgeSection::getPreferredHeight() const noexcept
{
    return expanded ? 168 : 32;
}

void ForgeSection::paint (juce::Graphics& g)
{
    ForgeColours colours;
    auto bounds = getLocalBounds().toFloat();
    g.setColour (colours.panel);
    g.fillRoundedRectangle (bounds, 5.0f);
    g.setColour (accent.withAlpha (0.65f));
    g.fillRect (0.0f, 0.0f, 3.0f, bounds.getHeight());
    g.setColour (colours.outline);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 5.0f, 1.0f);

    g.setColour (colours.text);
    g.setFont (juce::FontOptions().withHeight (12.5f).withStyle ("Bold"));
    g.drawText (title, 28, 0, 160, 32, juce::Justification::centredLeft);
}

void ForgeSection::resized()
{
    auto header = getLocalBounds().removeFromTop (32);
    disclosure.setBounds (header.removeFromLeft (26).reduced (4));
    enable.setBounds (header.removeFromRight (54).reduced (4, 6));
}
} // namespace WarlockAI

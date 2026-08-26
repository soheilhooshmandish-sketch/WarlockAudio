#pragma once

#include "UI/WarlockLookAndFeel.h"
#include <JuceHeader.h>
#include <functional>
#include <vector>

namespace WarlockAI
{
class ForgeSection final : public juce::Component
{
public:
    ForgeSection (const juce::String& title, juce::Colour accent);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setExpanded (bool shouldExpand);
    bool isExpanded() const noexcept { return expanded; }
    int getPreferredHeight() const noexcept;

    juce::ToggleButton enable { "ON" };

    std::function<void()> onExpandChanged;

private:
    juce::String title;
    juce::Colour accent;
    bool expanded = true;
    juce::TextButton disclosure { "-" };
};
} // namespace WarlockAI

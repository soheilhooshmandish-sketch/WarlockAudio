#pragma once

#include <JuceHeader.h>

namespace WarlockAI
{
class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState& state);

    juce::File getPresetDirectory() const;
    juce::StringArray getPresetNames() const;

    bool savePreset (const juce::String& name) const;
    bool loadPreset (const juce::String& name);
    bool deletePreset (const juce::String& name) const;

    void storeFactoryState();
    void loadFactoryState();

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree factoryState;
    juce::File presetDir;
};
} // namespace WarlockAI

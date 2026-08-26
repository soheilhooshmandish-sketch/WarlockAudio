#include "Core/PresetManager.h"

namespace WarlockAI
{
PresetManager::PresetManager (juce::AudioProcessorValueTreeState& state)
    : apvts (state)
{
    presetDir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                    .getChildFile ("BlackForgeAudio")
                    .getChildFile ("WarlockAI")
                    .getChildFile ("Presets");

    presetDir.createDirectory();
}

juce::File PresetManager::getPresetDirectory() const
{
    return presetDir;
}

juce::StringArray PresetManager::getPresetNames() const
{
    juce::StringArray names;
    auto files = presetDir.findChildFiles (juce::File::findFiles, false, "*.xml");
    files.sort();

    for (const auto& file : files)
        names.add (file.getFileNameWithoutExtension());

    return names;
}

bool PresetManager::savePreset (const juce::String& name) const
{
    if (name.isEmpty())
        return false;

    const auto file = presetDir.getChildFile (name + ".xml");
    if (auto xml = apvts.copyState().createXml())
        return xml->writeTo (file);

    return false;
}

bool PresetManager::loadPreset (const juce::String& name)
{
    const auto file = presetDir.getChildFile (name + ".xml");
    if (! file.existsAsFile())
        return false;

    if (auto xml = juce::XmlDocument::parse (file))
    {
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            return true;
        }
    }

    return false;
}

bool PresetManager::deletePreset (const juce::String& name) const
{
    return presetDir.getChildFile (name + ".xml").deleteFile();
}

void PresetManager::storeFactoryState()
{
    factoryState = apvts.copyState();
}

void PresetManager::loadFactoryState()
{
    if (factoryState.isValid())
        apvts.replaceState (factoryState.createCopy());
}
} // namespace WarlockAI

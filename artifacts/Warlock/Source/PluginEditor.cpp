#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FactoryPresets.h"
WarlockAudioProcessorEditor::WarlockAudioProcessorEditor (WarlockAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (1120, 540);
    juce::File exeFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    juce::File parent = exeFile.getParentDirectory();
    juce::File imgFile = parent.getChildFile("Resources").getChildFile("PedalFaceplate.png");
    if (!imgFile.existsAsFile())
        imgFile = parent.getParentDirectory().getChildFile("Resources").getChildFile("PedalFaceplate.png");
    if (imgFile.existsAsFile()) {
        logoImage = juce::ImageFileFormat::loadFrom(imgFile);
    }
    auto setupRotary = [this](juce::Slider& s, juce::Colour thumbColor) {
        s.setLookAndFeel(&satanicLookAndFeel); // Injecting our Inverted Star Engine layout pointer
        s.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 16);
        s.setColour(juce::Slider::thumbColourId, thumbColor);
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colour::fromRGB(190, 190, 190));
        s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(s);
    };
    // INVERTED CUSTOM PENTAGRAM KNOB ARRAY INITIALIZATION
    setupRotary(boostSlider, juce::Colour::fromRGB(110, 0, 0));
    setupRotary(gainSlider, juce::Colour::fromRGB(220, 15, 15));
    setupRotary(coldSlider, juce::Colour::fromRGB(230, 230, 255));
    setupRotary(grimSlider, juce::Colour::fromRGB(130, 130, 140));
    setupRotary(gateSlider, juce::Colour::fromRGB(80, 80, 85));
    setupRotary(releaseSlider, juce::Colour::fromRGB(60, 60, 65));
   
    setupRotary(subSlider, juce::Colour::fromRGB(140, 5, 175));
    setupRotary(psychosisSlider, juce::Colour::fromRGB(225, 95, 0));
   
    setupRotary(eqLowSlider, juce::Colour::fromRGB(70, 8, 8));
    setupRotary(eqMidSlider, juce::Colour::fromRGB(115, 12, 12));
    setupRotary(eqHighSlider, juce::Colour::fromRGB(165, 18, 18));
    setupRotary(levelSlider, juce::Colour::fromRGB(240, 20, 20));
    diodeSelector.addItemList(audioProcessor.apvts.getParameter(PARAM_DIODE_ID)->getAllValueStrings(), 1);
    addAndMakeVisible(diodeSelector);
   
    cabSelector.addItemList(audioProcessor.apvts.getParameter(PARAM_CAB_MODE_ID)->getAllValueStrings(), 1);
    addAndMakeVisible(cabSelector);
    routingSelector.addItemList(audioProcessor.apvts.getParameter(PARAM_ROUTING_ID)->getAllValueStrings(), 1);
    addAndMakeVisible(routingSelector);
    boostAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_BOOST_ID, boostSlider);
    gainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_GAIN_ID, gainSlider);
    coldAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_COLD_ID, coldSlider);
    grimAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_GRIM_ID, grimSlider);
    gateAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_GATE_ID, gateSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_GATERELEASE_ID, releaseSlider);
   
    subAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_SUB_ID, subSlider);
    psychosisAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_PSYCHOSIS_ID, psychosisSlider);
   
    eqLowAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_EQ_LOW_ID, eqLowSlider);
    eqMidAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_EQ_MID_ID, eqMidSlider);
    eqHighAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_EQ_HIGH_ID, eqHighSlider);
    levelAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, PARAM_LEVEL_ID, levelSlider);
   
    diodeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, PARAM_DIODE_ID, diodeSelector);
    cabAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, PARAM_CAB_MODE_ID, cabSelector);
    routingAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, PARAM_ROUTING_ID, routingSelector);

    ritualSelector.addItem ("— Ritual Grimoire —", 1);
    const auto rituals = WarlockPresetManager::getRitualPresets();
    for (int i = 0; i < (int) rituals.size(); ++i)
        ritualSelector.addItem (rituals[static_cast<size_t> (i)].name, i + 2);
    ritualSelector.setSelectedId (1, juce::dontSendNotification);
    ritualSelector.onChange = [this]()
    {
        const int id = ritualSelector.getSelectedId();
        if (id <= 1) return;
        auto rituals = WarlockPresetManager::getRitualPresets();
        const int index = id - 2;
        if (index >= 0 && index < (int) rituals.size())
            WarlockPresetManager::applyPreset (rituals[static_cast<size_t> (index)], audioProcessor.apvts);
    };
    addAndMakeVisible (ritualSelector);
}
WarlockAudioProcessorEditor::~WarlockAudioProcessorEditor()
{
    // Break connections and release LookAndFeel styling locks
    boostSlider.setLookAndFeel(nullptr);
    gainSlider.setLookAndFeel(nullptr);
    coldSlider.setLookAndFeel(nullptr);
    grimSlider.setLookAndFeel(nullptr);
    gateSlider.setLookAndFeel(nullptr);
    releaseSlider.setLookAndFeel(nullptr);
    subSlider.setLookAndFeel(nullptr);
    psychosisSlider.setLookAndFeel(nullptr);
    eqLowSlider.setLookAndFeel(nullptr);
    eqMidSlider.setLookAndFeel(nullptr);
    eqHighSlider.setLookAndFeel(nullptr);
    levelSlider.setLookAndFeel(nullptr);
}
void WarlockAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (logoImage.isValid()) {
        g.drawImageWithin(logoImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::fillDestination);
    } else {
        g.fillAll (juce::Colour::fromRGB (2, 2, 3));
    }
   
    g.setColour (juce::Colour::fromRGBA(0, 0, 0, 230));
    g.fillRect(25, 30, 850, 220);
    g.fillRect(25, 280, 850, 230);
    g.fillRect(900, 30, 195, 480);
    g.setColour (juce::Colour::fromRGB(150, 10, 10));
   
    // Geometric Top Spikes Rendering Loop
    for (int x = 25; x < 875; x += 30) {
        juce::Path spike;
        spike.startNewSubPath(static_cast<float>(x), 30.0f);
        spike.lineTo(static_cast<float>(x + 15), 55.0f);
        spike.lineTo(static_cast<float>(x + 30), 30.0f);
        spike.closeSubPath();
        g.fillPath(spike);
    }
   
    // Geometric Bottom Spikes Rendering Loop
    for (int x = 25; x < 875; x += 30) {
        juce::Path spike;
        spike.startNewSubPath(static_cast<float>(x), 510.0f);
        spike.lineTo(static_cast<float>(x + 15), 485.0f);
        spike.lineTo(static_cast<float>(x + 30), 510.0f);
        spike.closeSubPath();
        g.fillPath(spike);
    }
    g.setColour (juce::Colour::fromRGB(95, 5, 5));
    g.drawRect (getLocalBounds(), 4);
   
    g.setFont (juce::FontOptions ("Lucida Console", 11.0f, juce::Font::bold));
    g.setColour (juce::Colour::fromRGB(155, 155, 155));
   
    g.drawText ("MIND INVOCATION", 40, 215, 95, 20, juce::Justification::centred);
    g.drawText ("TOTAL POSSESSION",165, 215, 95, 20, juce::Justification::centred);
    g.drawText ("COLD EMBERS", 290, 215, 95, 20, juce::Justification::centred);
    g.drawText ("GRIM VOID", 415, 215, 95, 20, juce::Justification::centred);
    g.drawText ("DEMONIC SUB", 540, 215, 95, 20, juce::Justification::centred);
    g.drawText ("PSYCHOSIS SAG", 665, 215, 95, 20, juce::Justification::centred);
   
    g.drawText ("SACRIFICE THRSH", 40, 465, 95, 20, juce::Justification::centred);
    g.drawText ("AGONY RELEASE", 165, 465, 95, 20, juce::Justification::centred);
    g.drawText ("DESPAIR LOW", 290, 465, 95, 20, juce::Justification::centred);
    g.drawText ("AGONY MID", 415, 465, 95, 20, juce::Justification::centred);
    g.drawText ("WRATH HIGH", 540, 465, 95, 20, juce::Justification::centred);
    g.drawText ("FINAL CURSE", 665, 465, 95, 20, juce::Justification::centred);
    g.setFont (juce::FontOptions ("Lucida Console", 13.0f, juce::Font::bold));
    g.setColour (juce::Colour::fromRGB(190, 15, 15));
    g.drawText ("CLIPPING RITUAL", 900, 50, 195, 25, juce::Justification::centred);
    g.drawText ("ALTAR SIMULATION",900, 190, 195, 25, juce::Justification::centred);
    g.drawText ("SOUL CAPTURE", 900, 330, 195, 25, juce::Justification::centred);
    g.drawText ("RITUAL GRIMOIRE", 900, 430, 195, 25, juce::Justification::centred);
}
void WarlockAudioProcessorEditor::resized()
{
    boostSlider.setBounds (40, 55, 95, 155);
    gainSlider.setBounds (165, 55, 95, 155);
    coldSlider.setBounds (290, 55, 95, 155);
    grimSlider.setBounds (415, 55, 95, 155);
    subSlider.setBounds (540, 55, 95, 155);
    psychosisSlider.setBounds (665, 55, 95, 155);
   
    gateSlider.setBounds (40, 305, 95, 155);
    releaseSlider.setBounds (165, 305, 95, 155);
    eqLowSlider.setBounds (290, 305, 95, 155);
    eqMidSlider.setBounds (415, 305, 95, 155);
    eqHighSlider.setBounds (540, 305, 95, 155);
    levelSlider.setBounds (665, 305, 95, 155);
    diodeSelector.setBounds (920, 85, 155, 24);
    cabSelector.setBounds (920, 225, 155, 24);
    routingSelector.setBounds (920, 365, 155, 24);
    ritualSelector.setBounds (920, 465, 155, 24);
}

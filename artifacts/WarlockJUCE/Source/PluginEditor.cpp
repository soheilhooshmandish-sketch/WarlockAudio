#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include <cmath>

namespace
{
juce::File pluginResourcesDir()
{
    return juce::File::getSpecialLocation (juce::File::currentExecutableFile)
               .getParentDirectory()
               .getParentDirectory()
               .getChildFile ("Resources");
}

juce::Image loadSkin (const void* data, int size, const char* fileName)
{
    auto img = juce::ImageCache::getFromMemory (data, size);
    if (img.isValid())
        return img;

    const auto file = pluginResourcesDir().getChildFile (fileName);
    if (file.existsAsFile())
        return juce::ImageFileFormat::loadFrom (file);

    return {};
}
}

PedalLookAndFeel::PedalLookAndFeel()
{
    chassisImage = loadSkin (BinaryData::chassis_jpg, BinaryData::chassis_jpgSize, "chassis.jpg");
    knobImage    = loadSkin (BinaryData::knob_jpg,    BinaryData::knob_jpgSize,    "knob.jpg");
    stompImage   = loadSkin (BinaryData::stomp_jpg,   BinaryData::stomp_jpgSize,   "stomp.jpg");
    logoImage    = loadSkin (BinaryData::logo_png,    BinaryData::logo_pngSize,    "logo.png");

    setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8e0d4));
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xcc0a0806));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff3a342c));
    setColour (juce::ToggleButton::textColourId, juce::Colour (0xffc4b8a4));
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff14110e));
    setColour (juce::ComboBox::textColourId, juce::Colour (0xffd8cfc2));
    setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff4a4438));
}

void PedalLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (6.0f);
    const float size = juce::jmin (bounds.getWidth(), bounds.getHeight() - 18.0f);
    auto knob = juce::Rectangle<float> (0, 0, size, size)
                    .withCentre ({ bounds.getCentreX(), bounds.getY() + size * 0.48f });

    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (knob.expanded (4.0f).translated (0.0f, 3.0f));

    g.setColour (juce::Colour (0xff1a1612));
    g.fillEllipse (knob.expanded (3.0f));
    g.setColour (juce::Colour (0xff6a6256).withAlpha (0.7f));
    g.drawEllipse (knob.expanded (3.0f), 1.2f);

    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    {
        juce::Graphics::ScopedSaveState state (g);
        g.addTransform (juce::AffineTransform::rotation (angle, knob.getCentreX(), knob.getCentreY()));
        if (knobImage.isValid())
            g.drawImage (knobImage, knob, juce::RectanglePlacement::centred);
        else
        {
            g.setColour (juce::Colour (0xff2c2620));
            g.fillEllipse (knob);
        }
        g.setColour (juce::Colour (0xfff2ece0));
        auto needle = juce::Rectangle<float> (0, 0, 3.0f, size * 0.38f);
        needle.setCentre (knob.getCentreX(), knob.getY() + size * 0.28f);
        g.fillRoundedRectangle (needle, 1.0f);
    }
}

void PedalLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b,
                                         bool highlighted, bool down)
{
    auto r = b.getLocalBounds().toFloat();
    const bool on = b.getToggleState();
    const bool stomp = b.getName() == "stomp";

    if (stomp)
    {
        auto well = r.reduced (4.0f);
        g.setColour (juce::Colours::black.withAlpha (0.85f));
        g.fillEllipse (well.translated (0.0f, 3.0f));
        g.setColour (juce::Colour (0xff1c1814));
        g.fillEllipse (well);
        g.setColour (juce::Colour (0xff5a5248));
        g.drawEllipse (well, 1.4f);
        auto cap = well.reduced (down ? 10.0f : 7.0f);
        if (stompImage.isValid())
            g.drawImage (stompImage, cap, juce::RectanglePlacement::centred);
        else
        {
            g.setColour (juce::Colour (0xff3a322c));
            g.fillEllipse (cap);
        }
        return;
    }

    auto well = r.removeFromTop (22.0f).reduced (8.0f, 2.0f);
    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.fillRoundedRectangle (well.translated (0, 2), 3.0f);
    g.setColour (on ? juce::Colour (0xffc9b48a) : juce::Colour (0xff2a2420));
    g.fillRoundedRectangle (well, 3.0f);
    g.setColour (highlighted ? juce::Colour (0xffece6dc) : juce::Colour (0xff6a6258));
    g.drawRoundedRectangle (well, 3.0f, 1.0f);
    if (on)
    {
        g.setColour (juce::Colour (0xff8e1018).withAlpha (0.9f));
        g.fillEllipse (well.withSizeKeepingCentre (6.0f, 6.0f).translated (0.0f, -10.0f));
    }

    g.setColour (juce::Colour (0xffc4b8a4));
    g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
    g.drawText (b.getButtonText(), r, juce::Justification::centredTop);
}

WarlockAudioProcessorEditor::WarlockAudioProcessorEditor (WarlockAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&lnf);
    setSize (400, 740);
    setResizable (false, false);

    auto setupRotary = [this] (juce::Slider& s)
    {
        s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 78, 16);
        s.setRotaryParameters (juce::degreesToRadians (225.0f), juce::degreesToRadians (495.0f), true);
        addAndMakeVisible (s);
    };

    setupRotary (gainSlider);
    setupRotary (coldSlider);
    setupRotary (grimSlider);
    setupRotary (gateSlider);
    setupRotary (releaseSlider);
    setupRotary (levelSlider);
    setupRotary (trimSlider);
    setupRotary (delaySlider);
    setupRotary (reverbSlider);

    diodeButton.setButtonText ("ASYM");
    cabButton.setButtonText ("CAB IR");
    lofiButton.setButtonText ("LO-FI");
    cabModeBox.addItem ("Crypt Center 4x12", 1);
    cabModeBox.addItem ("Catacomb Edge 4x12", 2);
    cabModeBox.addItem ("Bedroom Lo-Fi", 3);
    cabModeBox.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (cabModeBox);
    stompButton.setName ("stomp");
    stompButton.setClickingTogglesState (true);
    addAndMakeVisible (diodeButton);
    addAndMakeVisible (cabButton);
    addAndMakeVisible (lofiButton);
    addAndMakeVisible (stompButton);

    auto& vts = audioProcessor.apvts;
    gainAttachment    = std::make_unique<SliderAttachment> (vts, PARAM_GAIN_ID, gainSlider);
    coldAttachment    = std::make_unique<SliderAttachment> (vts, PARAM_COLD_ID, coldSlider);
    grimAttachment    = std::make_unique<SliderAttachment> (vts, PARAM_GRIM_ID, grimSlider);
    gateAttachment    = std::make_unique<SliderAttachment> (vts, PARAM_GATE_ID, gateSlider);
    releaseAttachment = std::make_unique<SliderAttachment> (vts, PARAM_GATERELEASE_ID, releaseSlider);
    levelAttachment   = std::make_unique<SliderAttachment> (vts, PARAM_LEVEL_ID, levelSlider);
    trimAttachment    = std::make_unique<SliderAttachment> (vts, PARAM_TRIM_ID, trimSlider);
    delayAttachment   = std::make_unique<SliderAttachment> (vts, PARAM_DELAY_ID, delaySlider);
    reverbAttachment  = std::make_unique<SliderAttachment> (vts, PARAM_REVERB_ID, reverbSlider);
    diodeAttachment   = std::make_unique<ButtonAttachment> (vts, PARAM_DIODE_ID, diodeButton);
    cabAttachment     = std::make_unique<ButtonAttachment> (vts, PARAM_CAB_ID, cabButton);
    cabModeAttachment = std::make_unique<ComboAttachment>  (vts, PARAM_CABMODE_ID, cabModeBox);
    lofiAttachment    = std::make_unique<ButtonAttachment> (vts, PARAM_LOFI_ID, lofiButton);

    const bool engaged = vts.getRawParameterValue (PARAM_BYPASS_ID)->load() < 0.5f;
    stompButton.setToggleState (engaged, juce::dontSendNotification);
    stompButton.onClick = [this]
    {
        auto* bp = audioProcessor.apvts.getParameter (PARAM_BYPASS_ID);
        if (bp == nullptr) return;
        const bool wantEngage = stompButton.getToggleState();
        bp->beginChangeGesture();
        bp->setValueNotifyingHost (wantEngage ? 0.0f : 1.0f);
        bp->endChangeGesture();
    };

    startTimerHz (30);
}

WarlockAudioProcessorEditor::~WarlockAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void WarlockAudioProcessorEditor::timerCallback()
{
    const bool engaged = audioProcessor.apvts.getRawParameterValue (PARAM_BYPASS_ID)->load() < 0.5f;
    if (stompButton.getToggleState() != engaged)
        stompButton.setToggleState (engaged, juce::dontSendNotification);
    ledPulse += 0.12f;
    const float peak = audioProcessor.getMeterPeak();
    meterSmooth = meterSmooth * 0.72f + peak * 0.28f;
    repaint();
}

void WarlockAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll (juce::Colour (0xff070604));

    auto enclosure = bounds.reduced (10.0f, 8.0f);
    g.setColour (juce::Colour (0xff1a1612));
    g.fillRoundedRectangle (enclosure.expanded (4.0f), 18.0f);
    g.setColour (juce::Colour (0xff8a8276));
    g.drawRoundedRectangle (enclosure.expanded (4.0f), 18.0f, 2.0f);

    if (lnf.chassisImage.isValid())
    {
        juce::Path clip;
        clip.addRoundedRectangle (enclosure, 14.0f);
        g.saveState();
        g.reduceClipRegion (clip);
        g.setOpacity (1.0f);
        g.drawImage (lnf.chassisImage, enclosure, juce::RectanglePlacement::fillDestination, false);
        g.restoreState();
    }
    else
    {
        g.setColour (juce::Colour (0xff2a2620));
        g.fillRoundedRectangle (enclosure, 14.0f);
    }

    g.setColour (juce::Colour (0xffd8d0c4).withAlpha (0.35f));
    g.drawRoundedRectangle (enclosure, 14.0f, 1.2f);

    g.setFont (juce::FontOptions (8.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffcfc6b8));
    g.drawText ("IN",  22, 10, 40, 12, juce::Justification::centred);
    g.drawText ("OUT", getWidth() - 62, 10, 40, 12, juce::Justification::centred);
    g.setColour (juce::Colour (0xff8a8276));
    g.drawText ("9V DC -", 0, 10, getWidth(), 12, juce::Justification::centred);

    const bool engaged = audioProcessor.apvts.getRawParameterValue (PARAM_BYPASS_ID)->load() < 0.5f;
    const float glow = engaged ? (0.40f + 0.30f * std::sin (ledPulse)) : 0.06f;

    auto logo = juce::Rectangle<float> ((float) getWidth() * 0.5f - 52.0f, 28.0f, 104.0f, 104.0f);
    g.setColour (juce::Colours::black.withAlpha (0.65f));
    g.fillEllipse (logo.expanded (5.0f));
    if (lnf.logoImage.isValid())
        g.drawImage (lnf.logoImage, logo, juce::RectanglePlacement::centred);
    else
    {
        g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
        g.setColour (juce::Colour (0xffece6dc));
        g.drawText ("WARLOCK", 0, 48, getWidth(), 32, juce::Justification::centred);
    }

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffb8c8cc));
    g.drawText ("CHAINSAW DISTORTION", 0, 132, getWidth(), 14, juce::Justification::centred);
    g.setFont (juce::FontOptions (8.0f));
    g.setColour (juce::Colour (0xff8a8276));
    g.drawText ("WARLOCK AUDIO  ·  MODEL 1510", 0, 146, getWidth(), 12, juce::Justification::centred);

    auto led = juce::Rectangle<float> ((float) getWidth() * 0.5f - 6.0f, 162.0f, 12.0f, 12.0f);
    g.setColour (juce::Colour (0xff8e1018).withAlpha (glow));
    g.fillEllipse (led.expanded (5.0f));
    g.setColour (engaged ? juce::Colour (0xffe11d2e) : juce::Colour (0xff2a0c0c));
    g.fillEllipse (led);

    g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffe8e0d4).withAlpha (0.85f));
    g.drawText ("GAIN",    28,  278, 100, 12, juce::Justification::centred);
    g.drawText ("COLD",    150, 278, 100, 12, juce::Justification::centred);
    g.drawText ("GRIM",    272, 278, 100, 12, juce::Justification::centred);
    g.drawText ("GATE",    28,  398, 100, 12, juce::Justification::centred);
    g.drawText ("RELEASE", 150, 398, 100, 12, juce::Justification::centred);
    g.drawText ("LEVEL",   272, 398, 100, 12, juce::Justification::centred);
    g.drawText ("TRIM",    28,  598, 90, 12, juce::Justification::centred);
    g.drawText ("DELAY",   28,  698, 90, 12, juce::Justification::centred);
    g.drawText ("REVERB",  282, 698, 90, 12, juce::Justification::centred);

    auto meter = juce::Rectangle<float> (118.0f, 620.0f, 12.0f, 72.0f);
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (meter, 3.0f);
    const int segs = 8;
    const int lit = (int) std::round (meterSmooth * (float) segs);
    for (int i = 0; i < segs; ++i)
    {
        auto seg = meter.removeFromBottom (8.5f).reduced (2.0f, 0.8f);
        const bool on = engaged && i < lit;
        juce::Colour c = i >= 6 ? juce::Colour (0xffe11d2e)
                      : i >= 4 ? juce::Colour (0xffd4a017)
                               : juce::Colour (0xff3d9b54);
        g.setColour (on ? c : c.withAlpha (0.16f));
        g.fillRoundedRectangle (seg, 1.0f);
    }

    if (! engaged)
    {
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.fillRoundedRectangle (enclosure, 14.0f);
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.setColour (juce::Colour (0xffd8cfc2));
        g.drawText ("STANDBY", 0, 548, getWidth(), 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (10.0f));
        g.setColour (juce::Colour (0xff9a9286));
        g.drawText ("Press the stomp", 0, 566, getWidth(), 14, juce::Justification::centred);
    }

    g.setFont (juce::FontOptions (7.5f));
    g.setColour (juce::Colour (0xff7a7368));
    g.drawText ("HAMMOND 1590XX  ·  POWDER COAT  ·  UV PRINT", 0, getHeight() - 22, getWidth(), 10, juce::Justification::centred);
    g.drawText ("HIGH GAIN  ·  9V DC NEGATIVE TIP", 0, getHeight() - 12, getWidth(), 10, juce::Justification::centred);
}

void WarlockAudioProcessorEditor::resized()
{
    gainSlider.setBounds    (28,  176, 100, 104);
    coldSlider.setBounds    (150, 176, 100, 104);
    grimSlider.setBounds    (272, 176, 100, 104);

    gateSlider.setBounds    (28,  296, 100, 104);
    releaseSlider.setBounds (150, 296, 100, 104);
    levelSlider.setBounds   (272, 296, 100, 104);

    diodeButton.setBounds (40,  416, 100, 44);
    cabButton.setBounds    (150, 416, 100, 44);
    lofiButton.setBounds   (260, 416, 100, 44);
    cabModeBox.setBounds   (90,  462, 220, 22);

    trimSlider.setBounds   (28,  496, 90, 104);
    stompButton.setBounds  (148, 500, 104, 104);
    delaySlider.setBounds  (28,  610, 90, 88);
    reverbSlider.setBounds (282, 610, 90, 88);
}

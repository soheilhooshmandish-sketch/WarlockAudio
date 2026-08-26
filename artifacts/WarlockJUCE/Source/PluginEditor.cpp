#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include <cmath>

namespace
{
juce::File pluginResourcesDir()
{
    // Warlock.vst3/Contents/x86_64-win/Warlock.vst3  →  ../Resources
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

    setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffece6dc));
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ToggleButton::textColourId, juce::Colour (0xff7a7268));
}

void PedalLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (10.0f);
    const float size = juce::jmin (bounds.getWidth(), bounds.getHeight() - 16.0f);
    auto knob = juce::Rectangle<float> (0, 0, size, size)
                    .withCentre ({ bounds.getCentreX(), bounds.getY() + size * 0.5f });

    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.fillEllipse (knob.expanded (5.0f));

    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    {
        juce::Graphics::ScopedSaveState state (g);
        g.addTransform (juce::AffineTransform::rotation (angle, knob.getCentreX(), knob.getCentreY()));
        if (knobImage.isValid())
            g.drawImage (knobImage, knob, juce::RectanglePlacement::centred);
        else
        {
            g.setColour (juce::Colour (0xff2a2420));
            g.fillEllipse (knob);
        }
        g.setColour (juce::Colour (0xffece6dc));
        auto needle = knob.withSizeKeepingCentre (3.0f, size * 0.42f);
        needle.setY (knob.getY() + 6.0f);
        g.fillRoundedRectangle (needle, 1.0f);
    }

    g.setColour (juce::Colour (0xff8e1018).withAlpha (0.25f + sliderPos * 0.55f));
    g.drawEllipse (knob.expanded (3.0f), 2.0f);
}

void PedalLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b,
                                         bool highlighted, bool down)
{
    auto r = b.getLocalBounds().toFloat();
    const bool on = b.getToggleState();
    const bool stomp = b.getName() == "stomp";

    if (stomp)
    {
        auto well = r.reduced (6.0f);
        g.setColour (juce::Colours::black);
        g.fillEllipse (well);
        auto cap = well.reduced (down || on ? 8.0f : 5.0f);
        if (stompImage.isValid())
            g.drawImage (stompImage, cap, juce::RectanglePlacement::centred);
        else
        {
            g.setColour (juce::Colour (0xff3a322c));
            g.fillEllipse (cap);
        }
        return;
    }

    auto box = r.removeFromTop (18.0f).reduced (1.0f);
    g.setColour (juce::Colour (0xff12100c));
    g.fillRoundedRectangle (box, 3.0f);
    g.setColour (juce::Colour (0xff1a1612));
    g.drawRoundedRectangle (box, 3.0f, 1.0f);

    auto knob = box.reduced (2.0f);
    knob = on ? knob.removeFromRight (knob.getWidth() * 0.55f)
              : knob.removeFromLeft  (knob.getWidth() * 0.55f);
    g.setColour (on || highlighted ? juce::Colour (0xff8e1018) : juce::Colour (0xff3a322c));
    g.fillRoundedRectangle (knob, 2.0f);

    g.setColour (juce::Colour (0xff7a7268));
    g.setFont (juce::FontOptions (10.0f));
    g.drawText (b.getButtonText(), r, juce::Justification::centredTop);
}

WarlockAudioProcessorEditor::WarlockAudioProcessorEditor (WarlockAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&lnf);
    setSize (520, 700);
    setResizable (false, false);

    auto setupRotary = [this] (juce::Slider& s)
    {
        s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 16);
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
    repaint (juce::Rectangle<int> (getWidth() / 2 - 20, 128, 40, 28));
    repaint (juce::Rectangle<int> (190, 490, 40, 110));
}

void WarlockAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll (juce::Colour (0xff030201));

    if (lnf.chassisImage.isValid())
    {
        g.setOpacity (0.55f);
        g.drawImage (lnf.chassisImage, bounds, juce::RectanglePlacement::fillDestination, false);
        g.setOpacity (1.0f);
        g.setColour (juce::Colour (0xff030201).withAlpha (0.42f));
        g.fillRect (bounds);
    }

    g.setColour (juce::Colour (0xff2a2824));
    g.drawRect (bounds, 3.0f);

    const bool engaged = audioProcessor.apvts.getRawParameterValue (PARAM_BYPASS_ID)->load() < 0.5f;
    const float glow = engaged ? (0.45f + 0.35f * std::sin (ledPulse)) : 0.08f;

    auto logo = juce::Rectangle<float> ((float) getWidth() * 0.5f - 56.0f, 8.0f, 112.0f, 112.0f);
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillEllipse (logo.expanded (6.0f));
    g.setColour (juce::Colour (0xff3a322c).withAlpha (0.8f));
    g.drawEllipse (logo.expanded (6.0f), 1.5f);
    if (lnf.logoImage.isValid())
        g.drawImage (lnf.logoImage, logo, juce::RectanglePlacement::centred);
    else
    {
        g.setFont (juce::FontOptions (32.0f, juce::Font::bold));
        g.setColour (juce::Colour (0xffece6dc));
        g.drawText ("WARLOCK", 0, 16, getWidth(), 36, juce::Justification::centred);
    }

    g.setFont (juce::FontOptions (11.0f));
    g.setColour (juce::Colour (0xff9eb8bc));
    g.drawText ("CHAINSAW DISTORTION", 0, 118, getWidth(), 16, juce::Justification::centred);

    auto led = juce::Rectangle<float> ((float) getWidth() * 0.5f - 8.0f, 136.0f, 16.0f, 16.0f);
    g.setColour (juce::Colour (0xff8e1018).withAlpha (glow));
    g.fillEllipse (led.expanded (6.0f));
    g.setColour (engaged ? juce::Colour (0xffe11d2e) : juce::Colour (0xff3a1010));
    g.fillEllipse (led);

    g.setFont (juce::FontOptions (10.0f));
    g.setColour (juce::Colour (0xffece6dc).withAlpha (0.7f));
    const int labelsY1 = 258;
    const int labelsY2 = 388;
    g.drawText ("GAIN",     28,  labelsY1, 90, 14, juce::Justification::centred);
    g.drawText ("COLD",     128, labelsY1, 90, 14, juce::Justification::centred);
    g.drawText ("GRIM",     228, labelsY1, 90, 14, juce::Justification::centred);
    g.drawText ("GATE",     28,  labelsY2, 90, 14, juce::Justification::centred);
    g.drawText ("RELEASE",  128, labelsY2, 90, 14, juce::Justification::centred);
    g.drawText ("LEVEL",    228, labelsY2, 90, 14, juce::Justification::centred);
    g.drawText ("TRIM",     28,  518, 90, 14, juce::Justification::centred);
    g.drawText ("DELAY",   128,  518, 90, 14, juce::Justification::centred);
    g.drawText ("REVERB",  340,  438, 150, 14, juce::Justification::centred);

    auto meter = juce::Rectangle<float> (196.0f, 500.0f, 14.0f, 88.0f);
    g.setColour (juce::Colours::black.withAlpha (0.8f));
    g.fillRoundedRectangle (meter, 3.0f);
    const int segs = 8;
    const int lit = (int) std::round (meterSmooth * (float) segs);
    for (int i = 0; i < segs; ++i)
    {
        auto seg = meter.removeFromBottom (10.0f).reduced (2.0f, 1.0f);
        const bool on = engaged && i < lit;
        juce::Colour c = i >= 6 ? juce::Colour (0xffe11d2e)
                      : i >= 4 ? juce::Colour (0xffd4a017)
                               : juce::Colour (0xff3d9b54);
        g.setColour (on ? c : c.withAlpha (0.18f));
        g.fillRoundedRectangle (seg, 1.0f);
    }

    g.setFont (juce::FontOptions (9.0f));
    g.setColour (juce::Colour (0xff7a7268));
    g.drawText ("STOMP  BYPASS", 0, getHeight() - 28, getWidth(), 16, juce::Justification::centred);
}

void WarlockAudioProcessorEditor::resized()
{
    gainSlider.setBounds    (28,  156, 90, 104);
    coldSlider.setBounds    (128, 156, 90, 104);
    grimSlider.setBounds    (228, 156, 90, 104);

    gateSlider.setBounds    (28,  286, 90, 104);
    releaseSlider.setBounds (128, 286, 90, 104);
    levelSlider.setBounds   (228, 286, 90, 104);

    diodeButton.setBounds (340, 170, 150, 36);
    cabButton.setBounds    (340, 218, 150, 36);
    cabModeBox.setBounds   (340, 258, 150, 28);
    lofiButton.setBounds   (340, 294, 150, 36);

    trimSlider.setBounds   (28,  420, 90, 100);
    delaySlider.setBounds  (128, 420, 90, 100);
    reverbSlider.setBounds (340, 340, 150, 104);
    stompButton.setBounds  (230, 530, 120, 120);
}

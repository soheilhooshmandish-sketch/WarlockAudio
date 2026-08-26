#include "PluginEditor.h"

namespace WarlockAI
{
namespace
{
    void attachSlider (juce::AudioProcessorValueTreeState& state,
                       const juce::String& id,
                       juce::Slider& slider,
                       std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>& attachments)
    {
        attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, id, slider));
    }
}

WarlockAIAudioProcessorEditor::WarlockAIAudioProcessorEditor (WarlockAIAudioProcessor& p)
    : juce::AudioProcessorEditor (&p),
      processor (p),
      spectrum (p)
{
    setLookAndFeel (&lookAndFeel);

    titleLabel.setText ("WARLOCK AI", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::FontOptions().withHeight (26.0f).withStyle ("Bold"));
    titleLabel.setColour (juce::Label::textColourId, lookAndFeel.colours.bronze);
    addAndMakeVisible (titleLabel);

    statusLabel.setJustificationType (juce::Justification::centredRight);
    statusLabel.setColour (juce::Label::textColourId, lookAndFeel.colours.muted);
    addAndMakeVisible (statusLabel);

    for (auto* label : { &analysisLabel, &healthLabel, &mixFitLabel, &meterLabel })
    {
        label->setJustificationType (juce::Justification::topLeft);
        label->setColour (juce::Label::textColourId, lookAndFeel.colours.text);
        addAndMakeVisible (*label);
    }
    mixFitLabel.setColour (juce::Label::textColourId, lookAndFeel.colours.aiCyan);
    meterLabel.setJustificationType (juce::Justification::centredLeft);
    meterLabel.setColour (juce::Label::textColourId, lookAndFeel.colours.muted);

    auto& state = processor.getAPVTS();
    auto addKnob = [this] (WarlockKnob& knob) { addAndMakeVisible (knob); };

    addKnob (inputKnob); addKnob (driveKnob); addKnob (toneKnob); addKnob (tightKnob);
    addKnob (satKnob); addKnob (warmthKnob); addKnob (biteKnob); addKnob (distOutKnob);
    addKnob (gateKnob); addKnob (gateAtkKnob); addKnob (gateRelKnob); addKnob (outputKnob);
    addKnob (compThreshKnob); addKnob (compRatioKnob); addKnob (compAtkKnob);
    addKnob (compRelKnob); addKnob (compMakeKnob); addKnob (compMixKnob);
    addKnob (ampGainKnob); addKnob (ampBassKnob); addKnob (ampMidKnob); addKnob (ampTrebleKnob);
    addKnob (ampPresenceKnob); addKnob (ampResKnob); addKnob (ampMasterKnob);
    addKnob (cabMixKnob); addKnob (cabLowKnob); addKnob (cabHighKnob);
    addKnob (eqLowKnob); addKnob (eqLowMidKnob); addKnob (eqHighMidKnob); addKnob (eqHighKnob);
    addKnob (harmMixKnob); addKnob (harmDetuneKnob); addKnob (harmPan1Knob); addKnob (harmPan2Knob);
    addKnob (delayTimeKnob); addKnob (delayFbKnob); addKnob (delayMixKnob);
    addKnob (delayLoKnob); addKnob (delayHiKnob); addKnob (delayWidthKnob);
    addKnob (verbSizeKnob); addKnob (verbDampKnob); addKnob (verbMixKnob);
    addKnob (verbWidthKnob); addKnob (verbPreKnob);
    addKnob (widthKnob); addKnob (balanceKnob); addKnob (haasKnob);
    addKnob (midKnob); addKnob (sideKnob); addKnob (limitKnob); addKnob (limitRelKnob);

    attachSlider (state, ParamID::inputGain, inputKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::drive, driveKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::tone, toneKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::tightness, tightKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::saturation, satKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::warmth, warmthKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::bite, biteKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::distortionOut, distOutKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::gateThreshold, gateKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::gateAttack, gateAtkKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::gateRelease, gateRelKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::outputGain, outputKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::compThreshold, compThreshKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::compRatio, compRatioKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::compAttack, compAtkKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::compRelease, compRelKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::compMakeup, compMakeKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::compressorMix, compMixKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampGain, ampGainKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampBass, ampBassKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampMid, ampMidKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampTreble, ampTrebleKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampPresence, ampPresenceKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampResonance, ampResKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::ampMaster, ampMasterKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::cabMix, cabMixKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::cabLowCut, cabLowKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::cabHighCut, cabHighKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::eqLowGain, eqLowKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::eqLowMidGain, eqLowMidKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::eqHighMidGain, eqHighMidKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::eqHighGain, eqHighKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::harmMix, harmMixKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::harmDetune, harmDetuneKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::harmPan1, harmPan1Knob.slider, sliderAttachments);
    attachSlider (state, ParamID::harmPan2, harmPan2Knob.slider, sliderAttachments);
    attachSlider (state, ParamID::delayTime, delayTimeKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::delayFeedback, delayFbKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::delayMix, delayMixKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::delayLowCut, delayLoKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::delayHighCut, delayHiKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::delayStereoWidth, delayWidthKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::reverbSize, verbSizeKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::reverbDamping, verbDampKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::reverbMix, verbMixKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::reverbWidth, verbWidthKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::reverbPreDelay, verbPreKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::stereoWidth, widthKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::stereoBalance, balanceKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::stereoHaas, haasKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::midGain, midKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::sideGain, sideKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::limiterCeiling, limitKnob.slider, sliderAttachments);
    attachSlider (state, ParamID::limiterRelease, limitRelKnob.slider, sliderAttachments);

    modeBox.addItemList (distortionModeChoices(), 1);
    ampModeBox.addItemList (ampModeChoices(), 1);
    cabRouteBox.addItemList ({ "Mono", "Stereo" }, 1);
    harmInt1Box.addItemList (harmonyIntervalChoices(), 1);
    harmInt2Box.addItemList (harmonyIntervalChoices(), 1);
    delayModeBox.addItemList (delayModeChoices(), 1);
    delaySyncBox.addItemList (delaySyncChoices(), 1);
    oversampleBox.addItemList ({ "OS Off", "OS 2x", "OS 4x" }, 1);

    addAndMakeVisible (modeBox);
    addAndMakeVisible (oversampleBox);
    addAndMakeVisible (ampModeBox);
    addAndMakeVisible (cabRouteBox);
    addAndMakeVisible (harmInt1Box);
    addAndMakeVisible (harmInt2Box);
    addAndMakeVisible (delayModeBox);
    addAndMakeVisible (delaySyncBox);

    modeAttachment = std::make_unique<ComboAttachment> (state, ParamID::distortionMode, modeBox);
    oversampleAttachment = std::make_unique<ComboAttachment> (state, ParamID::oversample, oversampleBox);
    ampModeAttachment = std::make_unique<ComboAttachment> (state, ParamID::ampMode, ampModeBox);
    cabRouteAttachment = std::make_unique<ComboAttachment> (state, ParamID::cabMonoStereo, cabRouteBox);
    harmInt1Attachment = std::make_unique<ComboAttachment> (state, ParamID::harmInterval1, harmInt1Box);
    harmInt2Attachment = std::make_unique<ComboAttachment> (state, ParamID::harmInterval2, harmInt2Box);
    delayModeAttachment = std::make_unique<ComboAttachment> (state, ParamID::delayMode, delayModeBox);
    delaySyncAttachment = std::make_unique<ComboAttachment> (state, ParamID::delaySync, delaySyncBox);

    simpleButton.setClickingTogglesState (true);
    advancedButton.setClickingTogglesState (true);
    simpleButton.setRadioGroupId (1001);
    advancedButton.setRadioGroupId (1001);
    addAndMakeVisible (simpleButton);
    addAndMakeVisible (advancedButton);
    simpleButton.onClick = [this]
    {
        if (auto* param = processor.getAPVTS().getParameter (ParamID::uiMode))
            param->setValueNotifyingHost (0.0f);
        rebuildLayout();
    };
    advancedButton.onClick = [this]
    {
        if (auto* param = processor.getAPVTS().getParameter (ParamID::uiMode))
            param->setValueNotifyingHost (1.0f);
        rebuildLayout();
    };

    analyzeButton.onClick = [this] { processor.requestToneAnalysis(); };
    autoBuildButton.onClick = [this] { processor.requestAutoBuildTone(); };
    healthButton.onClick = [this] { processor.requestToneHealth(); };
    mixFitButton.onClick = [this] { processor.requestMixFitPreview(); };
    applyButton.onClick = [this] { processor.commitMixFit(); };
    undoButton.onClick = [this] { processor.undoLastAiEdit(); };
    addAndMakeVisible (analyzeButton);
    addAndMakeVisible (autoBuildButton);
    addAndMakeVisible (healthButton);
    addAndMakeVisible (mixFitButton);
    addAndMakeVisible (applyButton);
    addAndMakeVisible (undoButton);

    addAndMakeVisible (gateBypass);
    addAndMakeVisible (distBypass);
    addAndMakeVisible (monoButton);
    gateBypassAttachment = std::make_unique<ButtonAttachment> (state, ParamID::gateBypass, gateBypass);
    distBypassAttachment = std::make_unique<ButtonAttachment> (state, ParamID::distortionBypass, distBypass);
    monoAttachment = std::make_unique<ButtonAttachment> (state, ParamID::monoMode, monoButton);

    for (auto* section : { &coreSection, &ampSection, &cabSection, &eqSection, &modSection,
                           &harmSection, &delaySection, &verbSection, &stereoSection, &limitSection })
    {
        addAndMakeVisible (*section);
        section->onExpandChanged = [this] { resized(); };
    }

    compEnableAttachment   = std::make_unique<ButtonAttachment> (state, ParamID::compressorEnabled, coreSection.enable);
    ampEnableAttachment    = std::make_unique<ButtonAttachment> (state, ParamID::ampEnabled, ampSection.enable);
    cabEnableAttachment    = std::make_unique<ButtonAttachment> (state, ParamID::cabEnabled, cabSection.enable);
    harmEnableAttachment   = std::make_unique<ButtonAttachment> (state, ParamID::harmEnabled, harmSection.enable);
    delayEnableAttachment  = std::make_unique<ButtonAttachment> (state, ParamID::delayEnabled, delaySection.enable);
    reverbEnableAttachment = std::make_unique<ButtonAttachment> (state, ParamID::reverbEnabled, verbSection.enable);
    stereoEnableAttachment = std::make_unique<ButtonAttachment> (state, ParamID::stereoEnabled, stereoSection.enable);

    eqSection.enable.onClick = [this]
    {
        if (auto* param = processor.getAPVTS().getParameter (ParamID::eqBypass))
            param->setValueNotifyingHost (eqSection.enable.getToggleState() ? 0.0f : 1.0f);
    };
    modSection.enable.onClick = [this]
    {
        if (auto* param = processor.getAPVTS().getParameter (ParamID::distortionBypass))
            param->setValueNotifyingHost (modSection.enable.getToggleState() ? 0.0f : 1.0f);
    };
    limitSection.enable.onClick = [this]
    {
        if (auto* param = processor.getAPVTS().getParameter (ParamID::limiterBypass))
            param->setValueNotifyingHost (limitSection.enable.getToggleState() ? 0.0f : 1.0f);
    };

    grMeter.setCaption ("GR");
    delayMeter.setCaption ("DELAY");
    reverbMeter.setCaption ("REVERB");
    widthMeter.setCaption ("WIDTH");
    addAndMakeVisible (grMeter);
    addAndMakeVisible (delayMeter);
    addAndMakeVisible (reverbMeter);
    addAndMakeVisible (widthMeter);
    addAndMakeVisible (spectrum);
    addAndMakeVisible (dashboard);

    analysisLabel.setText ("Tone analysis idle. Capture audio, then press Analyze Tone.", juce::dontSendNotification);
    healthLabel.setText ("Tone Health awaiting measurement.", juce::dontSendNotification);
    mixFitLabel.setText ("Mix Fit suggestions appear after analysis.", juce::dontSendNotification);

    harmSection.setExpanded (false);
    delaySection.setExpanded (false);
    verbSection.setExpanded (false);

    setResizable (true, true);
    setResizeLimits (900, 540, 1400, 1100);
    rebuildLayout();
    startTimerHz (12);
}

WarlockAIAudioProcessorEditor::~WarlockAIAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

bool WarlockAIAudioProcessorEditor::isAdvanced() const
{
    if (auto* raw = processor.getAPVTS().getRawParameterValue (ParamID::uiMode))
        return raw->load() >= 0.5f;
    return advancedButton.getToggleState();
}

void WarlockAIAudioProcessorEditor::layoutKnobRow (juce::Rectangle<int> area, std::initializer_list<WarlockKnob*> knobs)
{
    if (knobs.size() == 0)
        return;
    const int w = area.getWidth() / static_cast<int> (knobs.size());
    for (auto* knob : knobs)
        knob->setBounds (area.removeFromLeft (w).reduced (3));
}

void WarlockAIAudioProcessorEditor::rebuildLayout()
{
    const bool advanced = isAdvanced();
    simpleButton.setToggleState (! advanced, juce::dontSendNotification);
    advancedButton.setToggleState (advanced, juce::dontSendNotification);

    const auto vis = [advanced] (juce::Component& c, bool extra = true)
    {
        c.setVisible (advanced && extra);
    };

    vis (satKnob); vis (warmthKnob); vis (biteKnob); vis (distOutKnob);
    vis (gateAtkKnob); vis (gateRelKnob);
    vis (compThreshKnob); vis (compRatioKnob); vis (compAtkKnob);
    vis (compRelKnob); vis (compMakeKnob); vis (compMixKnob);
    vis (ampGainKnob); vis (ampBassKnob); vis (ampMidKnob); vis (ampTrebleKnob);
    vis (ampPresenceKnob); vis (ampResKnob); vis (ampMasterKnob);
    vis (cabMixKnob); vis (cabLowKnob); vis (cabHighKnob);
    vis (eqLowKnob); vis (eqLowMidKnob); vis (eqHighMidKnob); vis (eqHighKnob);
    vis (harmMixKnob); vis (harmDetuneKnob); vis (harmPan1Knob); vis (harmPan2Knob);
    vis (delayTimeKnob); vis (delayFbKnob); vis (delayMixKnob);
    vis (delayLoKnob); vis (delayHiKnob); vis (delayWidthKnob);
    vis (verbSizeKnob); vis (verbDampKnob); vis (verbMixKnob);
    vis (verbWidthKnob); vis (verbPreKnob);
    vis (widthKnob); vis (balanceKnob); vis (haasKnob); vis (midKnob); vis (sideKnob);
    vis (limitKnob); vis (limitRelKnob);
    vis (oversampleBox); vis (ampModeBox); vis (cabRouteBox);
    vis (harmInt1Box); vis (harmInt2Box); vis (delayModeBox); vis (delaySyncBox);
    vis (spectrum); vis (healthLabel); vis (mixFitLabel);
    vis (gateBypass); vis (distBypass); vis (monoButton);
    vis (coreSection); vis (ampSection); vis (cabSection); vis (eqSection); vis (modSection);
    vis (harmSection); vis (delaySection); vis (verbSection); vis (stereoSection); vis (limitSection);
    vis (grMeter); vis (delayMeter); vis (reverbMeter); vis (widthMeter);

    setSize (advanced ? 1280 : 900, advanced ? 980 : 720);
    resized();
}

void WarlockAIAudioProcessorEditor::paint (juce::Graphics& g)
{
    const auto& c = lookAndFeel.colours;
    g.fillAll (c.background);
    auto header = getLocalBounds().removeFromTop (64).toFloat();
    juce::ColourGradient headerGrad (c.panel, 0.0f, 0.0f, c.background, 0.0f, header.getBottom(), false);
    g.setGradientFill (headerGrad);
    g.fillRect (header);
    g.setColour (c.bronzeDim);
    g.fillRect (0, 64, getWidth(), 2);
    g.setColour (c.muted);
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("BLACK FORGE  ·  PHASE 3", 18, 40, 260, 18, juce::Justification::centredLeft);
}

void WarlockAIAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (14);
    auto header = bounds.removeFromTop (48);
    titleLabel.setBounds (header.removeFromLeft (210));
    simpleButton.setBounds (header.removeFromLeft (88).reduced (2));
    advancedButton.setBounds (header.removeFromLeft (100).reduced (2));
    header.removeFromLeft (10);
    modeBox.setBounds (header.removeFromLeft (140).reduced (2, 8));
    if (isAdvanced())
        oversampleBox.setBounds (header.removeFromLeft (100).reduced (2, 8));
    statusLabel.setBounds (header);

    bounds.removeFromTop (8);
    auto simpleRow = bounds.removeFromTop (150);
    const int simpleWidth = simpleRow.getWidth() / 6;
    inputKnob.setBounds  (simpleRow.removeFromLeft (simpleWidth).reduced (4));
    driveKnob.setBounds  (simpleRow.removeFromLeft (simpleWidth).reduced (4));
    toneKnob.setBounds   (simpleRow.removeFromLeft (simpleWidth).reduced (4));
    tightKnob.setBounds  (simpleRow.removeFromLeft (simpleWidth).reduced (4));
    gateKnob.setBounds   (simpleRow.removeFromLeft (simpleWidth).reduced (4));
    outputKnob.setBounds (simpleRow.removeFromLeft (simpleWidth).reduced (4));

    auto actions = bounds.removeFromTop (36);
    analyzeButton.setBounds (actions.removeFromLeft (128).reduced (2));
    autoBuildButton.setBounds (actions.removeFromLeft (148).reduced (2));
    healthButton.setBounds (actions.removeFromLeft (118).reduced (2));
    mixFitButton.setBounds (actions.removeFromLeft (88).reduced (2));
    applyButton.setBounds (actions.removeFromLeft (72).reduced (2));
    undoButton.setBounds (actions.removeFromLeft (72).reduced (2));
    meterLabel.setBounds (actions);

    if (! isAdvanced())
    {
        analysisLabel.setBounds (bounds.removeFromTop (72).reduced (4));
        dashboard.setBounds (bounds.reduced (4));
        return;
    }

    auto dash = bounds.removeFromRight (330);
    spectrum.setBounds (dash.removeFromTop (140).reduced (4));
    dashboard.setBounds (dash.removeFromTop (280).reduced (4));
    analysisLabel.setBounds (dash.removeFromTop (52).reduced (4));
    healthLabel.setBounds (dash.removeFromTop (64).reduced (4));
    mixFitLabel.setBounds (dash.reduced (4));

    auto placeSection = [this] (ForgeSection& section, juce::Rectangle<int> area,
                                std::initializer_list<juce::Component*> extras,
                                std::initializer_list<WarlockKnob*> knobs)
    {
        section.setBounds (area);
        auto inner = area.reduced (8, 4);
        inner.removeFromTop (32);
        if (! section.isExpanded())
            return;

        if (extras.size() > 0)
        {
            auto comboRow = inner.removeFromTop (26);
            const int w = comboRow.getWidth() / static_cast<int> (juce::jmax (1, (int) extras.size()));
            for (auto* extra : extras)
                extra->setBounds (comboRow.removeFromLeft (w).reduced (2, 1));
        }
        layoutKnobRow (inner, knobs);
    };

    auto col = bounds.removeFromLeft (bounds.getWidth());
    const int gap = 6;
    auto take = [&col, gap] (int h)
    {
        auto r = col.removeFromTop (h);
        col.removeFromTop (gap);
        return r;
    };

    placeSection (coreSection, take (coreSection.getPreferredHeight()),
                  { &gateBypass, &grMeter },
                  { &gateAtkKnob, &gateRelKnob, &compThreshKnob, &compRatioKnob, &compAtkKnob, &compRelKnob, &compMakeKnob, &compMixKnob });

    placeSection (ampSection, take (ampSection.getPreferredHeight()),
                  { &ampModeBox },
                  { &ampGainKnob, &ampBassKnob, &ampMidKnob, &ampTrebleKnob, &ampPresenceKnob, &ampResKnob, &ampMasterKnob });

    placeSection (cabSection, take (cabSection.getPreferredHeight()),
                  { &cabRouteBox },
                  { &cabMixKnob, &cabLowKnob, &cabHighKnob });

    placeSection (eqSection, take (eqSection.getPreferredHeight()),
                  {},
                  { &eqLowKnob, &eqLowMidKnob, &eqHighMidKnob, &eqHighKnob });

    placeSection (modSection, take (modSection.getPreferredHeight()),
                  { &distBypass },
                  { &satKnob, &warmthKnob, &biteKnob, &distOutKnob });

    placeSection (harmSection, take (harmSection.getPreferredHeight()),
                  { &harmInt1Box, &harmInt2Box },
                  { &harmMixKnob, &harmDetuneKnob, &harmPan1Knob, &harmPan2Knob });

    placeSection (delaySection, take (delaySection.getPreferredHeight()),
                  { &delayModeBox, &delaySyncBox, &delayMeter },
                  { &delayTimeKnob, &delayFbKnob, &delayMixKnob, &delayLoKnob, &delayHiKnob, &delayWidthKnob });

    placeSection (verbSection, take (verbSection.getPreferredHeight()),
                  { &reverbMeter },
                  { &verbSizeKnob, &verbDampKnob, &verbWidthKnob, &verbPreKnob, &verbMixKnob });

    placeSection (stereoSection, take (stereoSection.getPreferredHeight()),
                  { &monoButton, &widthMeter },
                  { &widthKnob, &balanceKnob, &haasKnob, &midKnob, &sideKnob });

    placeSection (limitSection, take (limitSection.getPreferredHeight()),
                  {},
                  { &limitKnob, &limitRelKnob });
}

void WarlockAIAudioProcessorEditor::timerCallback()
{
    const float cpu = processor.getCpuLoad() * 100.0f;
    const int latency = processor.getReportedLatencySamples();
    const double sr = processor.getCurrentSampleRate();
    const float latencyMs = sr > 0.0 ? 1000.0f * static_cast<float> (latency) / static_cast<float> (sr) : 0.0f;
    meterLabel.setText ("CPU " + juce::String (cpu, 1) + "%   Latency "
                            + juce::String (latency) + " smp / "
                            + juce::String (latencyMs, 2) + " ms",
                        juce::dontSendNotification);

    if (isAdvanced() != advancedButton.getToggleState())
        rebuildLayout();

    refreshMeters();
    syncSectionEnables();
    refreshAnalysisPanel();
}

void WarlockAIAudioProcessorEditor::refreshMeters()
{
    grMeter.setLevel (juce::jlimit (0.0f, 1.0f, processor.getCompressorGainReduction() / 24.0f));
    delayMeter.setLevel (processor.getDelayWetLevel());
    reverbMeter.setLevel (processor.getReverbWetLevel());
    widthMeter.setLevel (processor.getStereoWidthMeter());
}

void WarlockAIAudioProcessorEditor::syncSectionEnables()
{
    auto& state = processor.getAPVTS();
    auto flag = [&state] (const char* id) -> bool
    {
        if (auto* raw = state.getRawParameterValue (id))
            return raw->load() >= 0.5f;
        return false;
    };

    eqSection.enable.setToggleState (! flag (ParamID::eqBypass), juce::dontSendNotification);
    modSection.enable.setToggleState (! flag (ParamID::distortionBypass), juce::dontSendNotification);
    limitSection.enable.setToggleState (! flag (ParamID::limiterBypass), juce::dontSendNotification);
}

void WarlockAIAudioProcessorEditor::refreshAnalysisPanel()
{
    const auto snap = processor.getAnalysisSnapshot();
    const bool busy = processor.isAnalysisBusy() || snap.analysing;
    dashboard.setAnalysing (busy);
    dashboard.setPreviewActive (processor.isMixFitPreviewActive() || snap.previewActive);
    if (busy)
        dashboard.repaint();

    if (! snap.ready)
        return;

    analysisLabel.setText ("TONE  " + snap.analysis.summary
                               + "\nConfidence " + juce::String (snap.analysis.confidence * 100.0f, 0) + "%",
                           juce::dontSendNotification);

    juce::String healthText = "TONE HEALTH  " + juce::String (snap.health.score, 0) + " / 100";
    if (snap.healthResult.problems.empty())
    {
        for (const auto& note : snap.health.notes)
            healthText += "\n- " + note;
    }
    else
    {
        for (const auto& issue : snap.healthResult.problems)
            healthText += "\n- " + juce::String (healthProblemName (issue.problem));
    }
    healthLabel.setText (healthText, juce::dontSendNotification);

    juce::String mixText = snap.mixFitResult.summary.isEmpty()
        ? (snap.mixFit.rationale.isEmpty() ? "Mix Fit suggestions appear after analysis." : snap.mixFit.rationale)
        : snap.mixFitResult.summary;
    if (! snap.recommendation.explanations.empty())
        mixText += "\n" + snap.recommendation.explanations.front();
    mixFitLabel.setText (mixText, juce::dontSendNotification);

    dashboard.setPack (processor.getAiPack());
}
} // namespace WarlockAI

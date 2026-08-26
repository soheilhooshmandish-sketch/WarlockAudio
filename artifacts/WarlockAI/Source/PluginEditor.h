#pragma once

#include "PluginProcessor.h"
#include "UI/AiDashboard.h"
#include "UI/ForgeSection.h"
#include "UI/MeterBar.h"
#include "UI/SpectrumComponent.h"
#include "UI/WarlockKnob.h"
#include "UI/WarlockLookAndFeel.h"
#include <JuceHeader.h>
#include <memory>
#include <vector>

namespace WarlockAI
{
class WarlockAIAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
    explicit WarlockAIAudioProcessorEditor (WarlockAIAudioProcessor&);
    ~WarlockAIAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void rebuildLayout();
    void refreshAnalysisPanel();
    void refreshMeters();
    void syncSectionEnables();
    bool isAdvanced() const;
    void layoutKnobRow (juce::Rectangle<int> area, std::initializer_list<WarlockKnob*> knobs);

    WarlockAIAudioProcessor& processor;
    WarlockLookAndFeel lookAndFeel;

    WarlockKnob inputKnob  { "INPUT",  lookAndFeel.colours.bronze };
    WarlockKnob driveKnob  { "GAIN",   lookAndFeel.colours.distortion };
    WarlockKnob toneKnob   { "TONE",   lookAndFeel.colours.distortion };
    WarlockKnob tightKnob  { "TIGHT",  lookAndFeel.colours.distortion };
    WarlockKnob satKnob    { "SAT",    lookAndFeel.colours.distortion };
    WarlockKnob warmthKnob { "WARMTH", lookAndFeel.colours.bronze };
    WarlockKnob biteKnob   { "BITE",   lookAndFeel.colours.distortion };
    WarlockKnob distOutKnob{ "DIST OUT", lookAndFeel.colours.steel };
    WarlockKnob gateKnob   { "GATE",   lookAndFeel.colours.steel };
    WarlockKnob gateAtkKnob{ "G ATK",  lookAndFeel.colours.steel };
    WarlockKnob gateRelKnob{ "G REL",  lookAndFeel.colours.steel };
    WarlockKnob outputKnob { "OUTPUT", lookAndFeel.colours.bronze };

    WarlockKnob compThreshKnob { "THRESH", lookAndFeel.colours.steel };
    WarlockKnob compRatioKnob  { "RATIO", lookAndFeel.colours.steel };
    WarlockKnob compAtkKnob    { "ATTACK", lookAndFeel.colours.steel };
    WarlockKnob compRelKnob    { "RELEASE", lookAndFeel.colours.steel };
    WarlockKnob compMakeKnob   { "MAKEUP", lookAndFeel.colours.steel };
    WarlockKnob compMixKnob    { "MIX", lookAndFeel.colours.steel };

    WarlockKnob ampGainKnob { "GAIN", lookAndFeel.colours.distortion };
    WarlockKnob ampBassKnob { "BASS", lookAndFeel.colours.bronze };
    WarlockKnob ampMidKnob  { "MID", lookAndFeel.colours.bronze };
    WarlockKnob ampTrebleKnob { "TREBLE", lookAndFeel.colours.bronze };
    WarlockKnob ampPresenceKnob { "PRES", lookAndFeel.colours.distortion };
    WarlockKnob ampResKnob { "RES", lookAndFeel.colours.bronze };
    WarlockKnob ampMasterKnob { "MASTER", lookAndFeel.colours.steel };

    WarlockKnob cabMixKnob { "MIX", lookAndFeel.colours.bronze };
    WarlockKnob cabLowKnob { "LOW CUT", lookAndFeel.colours.steel };
    WarlockKnob cabHighKnob { "HIGH CUT", lookAndFeel.colours.steel };

    WarlockKnob eqLowKnob     { "LOW", lookAndFeel.colours.bronze };
    WarlockKnob eqLowMidKnob  { "L-MID", lookAndFeel.colours.bronze };
    WarlockKnob eqHighMidKnob { "H-MID", lookAndFeel.colours.bronze };
    WarlockKnob eqHighKnob    { "HIGH", lookAndFeel.colours.bronze };

    WarlockKnob harmMixKnob { "MIX", lookAndFeel.colours.ai };
    WarlockKnob harmDetuneKnob { "DETUNE", lookAndFeel.colours.ai };
    WarlockKnob harmPan1Knob { "PAN 1", lookAndFeel.colours.aiCyan };
    WarlockKnob harmPan2Knob { "PAN 2", lookAndFeel.colours.aiCyan };

    WarlockKnob delayTimeKnob { "TIME", lookAndFeel.colours.aiCyan };
    WarlockKnob delayFbKnob   { "FBK",  lookAndFeel.colours.aiCyan };
    WarlockKnob delayMixKnob  { "MIX",  lookAndFeel.colours.aiCyan };
    WarlockKnob delayLoKnob   { "LO", lookAndFeel.colours.aiCyan };
    WarlockKnob delayHiKnob   { "HI", lookAndFeel.colours.aiCyan };
    WarlockKnob delayWidthKnob{ "WIDTH", lookAndFeel.colours.aiCyan };

    WarlockKnob verbSizeKnob { "SIZE", lookAndFeel.colours.aiCyan };
    WarlockKnob verbDampKnob { "DAMP", lookAndFeel.colours.aiCyan };
    WarlockKnob verbMixKnob  { "MIX",  lookAndFeel.colours.aiCyan };
    WarlockKnob verbWidthKnob{ "WIDTH", lookAndFeel.colours.aiCyan };
    WarlockKnob verbPreKnob  { "PRE", lookAndFeel.colours.aiCyan };

    WarlockKnob widthKnob   { "WIDTH", lookAndFeel.colours.steel };
    WarlockKnob balanceKnob { "PAN",   lookAndFeel.colours.steel };
    WarlockKnob haasKnob    { "HAAS",  lookAndFeel.colours.steel };
    WarlockKnob midKnob     { "MID", lookAndFeel.colours.steel };
    WarlockKnob sideKnob    { "SIDE", lookAndFeel.colours.steel };
    WarlockKnob limitKnob   { "CEILING", lookAndFeel.colours.steel };
    WarlockKnob limitRelKnob{ "REL", lookAndFeel.colours.steel };

    juce::ComboBox modeBox;
    juce::ComboBox oversampleBox;
    juce::ComboBox ampModeBox;
    juce::ComboBox cabRouteBox;
    juce::ComboBox harmInt1Box;
    juce::ComboBox harmInt2Box;
    juce::ComboBox delayModeBox;
    juce::ComboBox delaySyncBox;

    juce::TextButton simpleButton { "SIMPLE" };
    juce::TextButton advancedButton { "ADVANCED" };
    juce::TextButton analyzeButton { "ANALYZE TONE" };
    juce::TextButton autoBuildButton { "AUTO BUILD TONE" };
    juce::TextButton healthButton { "TONE HEALTH" };
    juce::TextButton mixFitButton { "MIX FIT" };
    juce::TextButton applyButton { "APPLY" };
    juce::TextButton undoButton { "UNDO" };

    juce::ToggleButton gateBypass { "Gate Bypass" };
    juce::ToggleButton distBypass { "Dist Bypass" };
    juce::ToggleButton monoButton { "MONO" };

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label analysisLabel;
    juce::Label healthLabel;
    juce::Label mixFitLabel;
    juce::Label meterLabel;

    ForgeSection coreSection { "CORE", lookAndFeel.colours.distortion };
    ForgeSection ampSection  { "AMP", lookAndFeel.colours.distortion };
    ForgeSection cabSection  { "CAB", lookAndFeel.colours.bronze };
    ForgeSection eqSection   { "EQ", lookAndFeel.colours.bronze };
    ForgeSection modSection  { "MOD", lookAndFeel.colours.distortion };
    ForgeSection harmSection { "HARMONY", lookAndFeel.colours.ai };
    ForgeSection delaySection{ "DELAY", lookAndFeel.colours.aiCyan };
    ForgeSection verbSection { "REVERB", lookAndFeel.colours.aiCyan };
    ForgeSection stereoSection { "STEREO", lookAndFeel.colours.steel };
    ForgeSection limitSection { "LIMITER", lookAndFeel.colours.steel };

    MeterBar grMeter { lookAndFeel.colours.distortion };
    MeterBar delayMeter { lookAndFeel.colours.aiCyan };
    MeterBar reverbMeter { lookAndFeel.colours.ai };
    MeterBar widthMeter { lookAndFeel.colours.bronze };

    SpectrumComponent spectrum;
    AiDashboard dashboard;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::unique_ptr<ComboAttachment> modeAttachment;
    std::unique_ptr<ComboAttachment> oversampleAttachment;
    std::unique_ptr<ComboAttachment> ampModeAttachment;
    std::unique_ptr<ComboAttachment> cabRouteAttachment;
    std::unique_ptr<ComboAttachment> harmInt1Attachment;
    std::unique_ptr<ComboAttachment> harmInt2Attachment;
    std::unique_ptr<ComboAttachment> delayModeAttachment;
    std::unique_ptr<ComboAttachment> delaySyncAttachment;
    std::unique_ptr<ButtonAttachment> gateBypassAttachment;
    std::unique_ptr<ButtonAttachment> distBypassAttachment;
    std::unique_ptr<ButtonAttachment> ampEnableAttachment;
    std::unique_ptr<ButtonAttachment> cabEnableAttachment;
    std::unique_ptr<ButtonAttachment> harmEnableAttachment;
    std::unique_ptr<ButtonAttachment> delayEnableAttachment;
    std::unique_ptr<ButtonAttachment> reverbEnableAttachment;
    std::unique_ptr<ButtonAttachment> stereoEnableAttachment;
    std::unique_ptr<ButtonAttachment> compEnableAttachment;
    std::unique_ptr<ButtonAttachment> monoAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarlockAIAudioProcessorEditor)
};
} // namespace WarlockAI

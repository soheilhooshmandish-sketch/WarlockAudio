#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
struct WarlockPreset
{
    juce::String name;
    float invocation;
    float possession;
    float cold;
    float grim;
    float sacrifice;
    float release;
    float subMix;
    float psychosis;
    float despair;
    float agony;
    float wrath;
    int diodeMode;
    int altarMode;
    int soulCapture;
};
class WarlockPresetManager
{
public:
    static std::vector<WarlockPreset> getRitualPresets()
    {
        return {
            { "Transilvanian Hunger (True Raw)", 12.0f, 48.5f, 15.0f, -8.0f, -60.0f, 15.0f, 0.00f, 0.15f, -6.0f, -4.0f, 12.0f, 1, 2, 1 },
            { "De Mysteriis Dom Sathanas", 8.5f, 42.0f, 8.0f, 6.0f, -65.0f, 25.0f, 0.20f, 0.05f, 2.0f, 4.0f, 6.0f, 0, 1, 1 },
            { "Pure Holocaust Speed Riffing", 15.0f, 50.0f, 12.0f, -12.0f,-50.0f, 10.0f, 0.00f, 0.35f, -10.0f,-6.0f, 15.0f, 1, 2, 0 },
            { "Abyssal Mind Control Lord", 6.0f, 38.0f, 4.0f, 12.0f, -75.0f, 45.0f, 0.66f, 0.50f, 8.0f, -3.0f, 4.0f, 2, 1, 2 },
            { "Luciferian Dawn Overdrive", 4.5f, 25.0f, 6.0f, 2.0f, -90.0f, 60.0f, 0.10f, 0.00f, 0.0f, 0.0f, 3.0f, 2, 0, 1 }
        };
    }
    static void applyPreset(WarlockPreset preset, juce::AudioProcessorValueTreeState& apvts)
    {
        if (auto* p = apvts.getParameter(PARAM_BOOST_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_BOOST_ID).convertTo0to1(preset.invocation));
        if (auto* p = apvts.getParameter(PARAM_GAIN_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_GAIN_ID).convertTo0to1(preset.possession));
        if (auto* p = apvts.getParameter(PARAM_COLD_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_COLD_ID).convertTo0to1(preset.cold));
        if (auto* p = apvts.getParameter(PARAM_GRIM_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_GRIM_ID).convertTo0to1(preset.grim));
        if (auto* p = apvts.getParameter(PARAM_GATE_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_GATE_ID).convertTo0to1(preset.sacrifice));
        if (auto* p = apvts.getParameter(PARAM_GATERELEASE_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_GATERELEASE_ID).convertTo0to1(preset.release));
        if (auto* p = apvts.getParameter(PARAM_SUB_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_SUB_ID).convertTo0to1(preset.subMix));
        if (auto* p = apvts.getParameter(PARAM_PSYCHOSIS_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_PSYCHOSIS_ID).convertTo0to1(preset.psychosis));
        if (auto* p = apvts.getParameter(PARAM_EQ_LOW_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_EQ_LOW_ID).convertTo0to1(preset.despair));
        if (auto* p = apvts.getParameter(PARAM_EQ_MID_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_EQ_MID_ID).convertTo0to1(preset.agony));
        if (auto* p = apvts.getParameter(PARAM_EQ_HIGH_ID)) p->setValueNotifyingHost(apvts.getParameterRange(PARAM_EQ_HIGH_ID).convertTo0to1(preset.wrath));
       
        if (auto* p = apvts.getParameter(PARAM_DIODE_ID)) p->setValueNotifyingHost(static_cast<float>(preset.diodeMode) / 2.0f);
        if (auto* p = apvts.getParameter(PARAM_CAB_MODE_ID)) p->setValueNotifyingHost(static_cast<float>(preset.altarMode) / 2.0f);
        if (auto* p = apvts.getParameter(PARAM_ROUTING_ID)) p->setValueNotifyingHost(static_cast<float>(preset.soulCapture) / 2.0f);
    }
};

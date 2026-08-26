#include "Core/WarlockParameters.h"

namespace WarlockAI
{
namespace
{
    juce::AudioParameterFloat* makeFloat (const juce::ParameterID& id,
                                          const juce::String& name,
                                          juce::NormalisableRange<float> range,
                                          float defaultValue,
                                          const juce::String& label = {},
                                          std::function<juce::String (float, int)> stringFromValue = nullptr)
    {
        auto attributes = juce::AudioParameterFloatAttributes();
        if (label.isNotEmpty())
            attributes = attributes.withLabel (label);
        if (stringFromValue != nullptr)
            attributes = attributes.withStringFromValueFunction (std::move (stringFromValue));

        return new juce::AudioParameterFloat (id, name, range, defaultValue, attributes);
    }

    juce::AudioParameterBool* makeBool (const juce::ParameterID& id,
                                        const juce::String& name,
                                        bool defaultValue)
    {
        return new juce::AudioParameterBool (id, name, defaultValue);
    }

    juce::AudioParameterChoice* makeChoice (const juce::ParameterID& id,
                                            const juce::String& name,
                                            const juce::StringArray& choices,
                                            int defaultIndex)
    {
        return new juce::AudioParameterChoice (id, name, choices, defaultIndex);
    }

    juce::String dbString (float value, int)
    {
        return juce::String (value, 1) + " dB";
    }

    juce::String msString (float value, int)
    {
        return juce::String (value, 1) + " ms";
    }

    juce::String hzString (float value, int)
    {
        if (value >= 1000.0f)
            return juce::String (value / 1000.0f, 2) + " kHz";

        return juce::String (value, 1) + " Hz";
    }

    juce::String percentString (float value, int)
    {
        return juce::String (juce::roundToInt (value * 100.0f)) + " %";
    }

    juce::String centsString (float value, int)
    {
        return juce::String (value, 1) + " ct";
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto add = [&params] (auto* p)
    {
        params.emplace_back (p);
    };

    add (makeFloat ({ ParamID::inputGain, 1 }, "Input",
                    { -24.0f, 24.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeFloat ({ ParamID::outputGain, 1 }, "Output",
                    { -24.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));

    add (makeFloat ({ ParamID::drive, 1 }, "Drive",
                    { 0.0f, 1.0f, 0.0001f }, 0.45f, {}, percentString));
    add (makeFloat ({ ParamID::tone, 1 }, "Tone",
                    { 0.0f, 1.0f, 0.0001f }, 0.55f, {}, percentString));
    add (makeFloat ({ ParamID::tightness, 1 }, "Tightness",
                    { 0.0f, 1.0f, 0.0001f }, 0.4f, {}, percentString));
    add (makeFloat ({ ParamID::saturation, 1 }, "Saturation",
                    { 0.0f, 1.0f, 0.0001f }, 0.35f, {}, percentString));
    add (makeFloat ({ ParamID::warmth, 1 }, "Warmth",
                    { 0.0f, 1.0f, 0.0001f }, 0.3f, {}, percentString));
    add (makeFloat ({ ParamID::bite, 1 }, "Bite",
                    { 0.0f, 1.0f, 0.0001f }, 0.25f, {}, percentString));
    add (makeFloat ({ ParamID::distortionOut, 1 }, "Dist Out",
                    { -24.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeChoice ({ ParamID::distortionMode, 1 }, "Mode",
                     distortionModeChoices(), static_cast<int> (DistortionMode::Metal)));
    add (makeChoice ({ ParamID::oversample, 1 }, "Oversample",
                     { "Off", "2x", "4x" }, 2));
    add (makeBool ({ ParamID::distortionBypass, 1 }, "Dist Bypass", false));

    add (makeFloat ({ ParamID::gateThreshold, 1 }, "Gate Thresh",
                    { -80.0f, 0.0f, 0.01f }, -48.0f, "dB", dbString));
    add (makeFloat ({ ParamID::gateAttack, 1 }, "Gate Attack",
                    { 0.1f, 40.0f, 0.01f }, 1.5f, "ms", msString));
    add (makeFloat ({ ParamID::gateRelease, 1 }, "Gate Release",
                    { 5.0f, 400.0f, 0.1f }, 80.0f, "ms", msString));
    add (makeFloat ({ ParamID::gateRatio, 1 }, "Gate Ratio",
                    { 1.0f, 20.0f, 0.01f }, 8.0f));
    add (makeBool ({ ParamID::gateBypass, 1 }, "Gate Bypass", false));

    add (makeFloat ({ ParamID::compThreshold, 1 }, "Comp Thresh",
                    { -40.0f, 0.0f, 0.01f }, -18.0f, "dB", dbString));
    add (makeFloat ({ ParamID::compRatio, 1 }, "Comp Ratio",
                    { 1.0f, 20.0f, 0.01f }, 3.0f));
    add (makeFloat ({ ParamID::compAttack, 1 }, "Comp Attack",
                    { 0.1f, 80.0f, 0.01f }, 8.0f, "ms", msString));
    add (makeFloat ({ ParamID::compRelease, 1 }, "Comp Release",
                    { 10.0f, 400.0f, 0.1f }, 120.0f, "ms", msString));
    add (makeFloat ({ ParamID::compMakeup, 1 }, "Comp Makeup",
                    { 0.0f, 24.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeBool ({ ParamID::compBypass, 1 }, "Comp Bypass", true));
    add (makeBool ({ ParamID::compressorEnabled, 1 }, "Comp Enable", false));
    add (makeFloat ({ ParamID::compressorMix, 1 }, "Comp Mix",
                    { 0.0f, 1.0f, 0.0001f }, 1.0f, {}, percentString));

    add (makeFloat ({ ParamID::eqLowGain, 1 }, "EQ Low",
                    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeFloat ({ ParamID::eqLowFreq, 1 }, "EQ Low Freq",
                    { 40.0f, 300.0f, 0.1f }, 110.0f, "Hz", hzString));
    add (makeFloat ({ ParamID::eqLowMidGain, 1 }, "EQ Low Mid",
                    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeFloat ({ ParamID::eqLowMidFreq, 1 }, "EQ Low Mid Freq",
                    { 200.0f, 1200.0f, 0.1f }, 450.0f, "Hz", hzString));
    add (makeFloat ({ ParamID::eqLowMidQ, 1 }, "EQ Low Mid Q",
                    { 0.3f, 4.0f, 0.01f }, 1.0f));
    add (makeFloat ({ ParamID::eqHighMidGain, 1 }, "EQ High Mid",
                    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeFloat ({ ParamID::eqHighMidFreq, 1 }, "EQ High Mid Freq",
                    { 800.0f, 5000.0f, 0.1f }, 1800.0f, "Hz", hzString));
    add (makeFloat ({ ParamID::eqHighMidQ, 1 }, "EQ High Mid Q",
                    { 0.3f, 4.0f, 0.01f }, 1.1f));
    add (makeFloat ({ ParamID::eqHighGain, 1 }, "EQ High",
                    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeFloat ({ ParamID::eqHighFreq, 1 }, "EQ High Freq",
                    { 3000.0f, 14000.0f, 0.1f }, 6500.0f, "Hz", hzString));
    add (makeBool ({ ParamID::eqBypass, 1 }, "EQ Bypass", false));

    add (makeFloat ({ ParamID::delayTime, 1 }, "Delay Time",
                    { 10.0f, 800.0f, 0.1f }, 320.0f, "ms", msString));
    add (makeFloat ({ ParamID::delayFeedback, 1 }, "Delay Feedback",
                    { 0.0f, 0.9f, 0.0001f }, 0.25f, {}, percentString));
    add (makeFloat ({ ParamID::delayMix, 1 }, "Delay Mix",
                    { 0.0f, 1.0f, 0.0001f }, 0.0f, {}, percentString));
    add (makeBool ({ ParamID::delayBypass, 1 }, "Delay Bypass", true));
    add (makeBool ({ ParamID::delayEnabled, 1 }, "Delay Enable", false));
    add (makeFloat ({ ParamID::delayLowCut, 1 }, "Delay Low Cut",
                    { 20.0f, 800.0f, 0.1f }, 80.0f, "Hz", hzString));
    add (makeFloat ({ ParamID::delayHighCut, 1 }, "Delay High Cut",
                    { 1000.0f, 16000.0f, 0.1f }, 8000.0f, "Hz", hzString));
    add (makeFloat ({ ParamID::delayStereoWidth, 1 }, "Delay Width",
                    { 0.0f, 2.0f, 0.0001f }, 1.0f));
    add (makeChoice ({ ParamID::delaySync, 1 }, "Delay Sync", delaySyncChoices(), 0));
    add (makeChoice ({ ParamID::delayMode, 1 }, "Delay Mode", delayModeChoices(), 1));

    add (makeFloat ({ ParamID::reverbSize, 1 }, "Reverb Size",
                    { 0.0f, 1.0f, 0.0001f }, 0.35f, {}, percentString));
    add (makeFloat ({ ParamID::reverbDamping, 1 }, "Reverb Damp",
                    { 0.0f, 1.0f, 0.0001f }, 0.5f, {}, percentString));
    add (makeFloat ({ ParamID::reverbWidth, 1 }, "Reverb Width",
                    { 0.0f, 1.0f, 0.0001f }, 1.0f, {}, percentString));
    add (makeFloat ({ ParamID::reverbMix, 1 }, "Reverb Mix",
                    { 0.0f, 1.0f, 0.0001f }, 0.0f, {}, percentString));
    add (makeBool ({ ParamID::reverbBypass, 1 }, "Reverb Bypass", true));
    add (makeBool ({ ParamID::reverbEnabled, 1 }, "Reverb Enable", false));
    add (makeFloat ({ ParamID::reverbPreDelay, 1 }, "Pre-Delay",
                    { 0.0f, 120.0f, 0.1f }, 18.0f, "ms", msString));

    add (makeFloat ({ ParamID::stereoWidth, 1 }, "Width",
                    { 0.0f, 2.0f, 0.0001f }, 1.0f));
    add (makeFloat ({ ParamID::stereoBalance, 1 }, "Balance",
                    { -1.0f, 1.0f, 0.0001f }, 0.0f));
    add (makeFloat ({ ParamID::stereoHaas, 1 }, "Haas",
                    { 0.0f, 20.0f, 0.01f }, 0.0f, "ms", msString));
    add (makeBool ({ ParamID::stereoBypass, 1 }, "Stereo Bypass", false));
    add (makeBool ({ ParamID::stereoEnabled, 1 }, "Stereo Enable", true));
    add (makeBool ({ ParamID::monoMode, 1 }, "Mono", false));
    add (makeFloat ({ ParamID::midGain, 1 }, "Mid Gain",
                    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));
    add (makeFloat ({ ParamID::sideGain, 1 }, "Side Gain",
                    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));

    add (makeFloat ({ ParamID::limiterCeiling, 1 }, "Limiter",
                    { -12.0f, 0.0f, 0.01f }, -0.3f, "dB", dbString));
    add (makeFloat ({ ParamID::limiterRelease, 1 }, "Lim Release",
                    { 10.0f, 200.0f, 0.1f }, 50.0f, "ms", msString));
    add (makeBool ({ ParamID::limiterBypass, 1 }, "Limiter Bypass", false));

    add (makeBool ({ ParamID::ampEnabled, 1 }, "Amp Enable", true));
    add (makeChoice ({ ParamID::ampMode, 1 }, "Amp Mode",
                     ampModeChoices(), static_cast<int> (AmpMode::Metal)));
    add (makeFloat ({ ParamID::ampGain, 1 }, "Amp Gain",
                    { 0.0f, 1.0f, 0.0001f }, 0.55f, {}, percentString));
    add (makeFloat ({ ParamID::ampBass, 1 }, "Bass",
                    { -12.0f, 12.0f, 0.01f }, 1.5f, "dB", dbString));
    add (makeFloat ({ ParamID::ampMid, 1 }, "Mid",
                    { -12.0f, 12.0f, 0.01f }, -1.0f, "dB", dbString));
    add (makeFloat ({ ParamID::ampTreble, 1 }, "Treble",
                    { -12.0f, 12.0f, 0.01f }, 2.0f, "dB", dbString));
    add (makeFloat ({ ParamID::ampPresence, 1 }, "Presence",
                    { -12.0f, 12.0f, 0.01f }, 1.0f, "dB", dbString));
    add (makeFloat ({ ParamID::ampResonance, 1 }, "Resonance",
                    { 0.0f, 1.0f, 0.0001f }, 0.35f, {}, percentString));
    add (makeFloat ({ ParamID::ampMaster, 1 }, "Amp Master",
                    { -24.0f, 12.0f, 0.01f }, 0.0f, "dB", dbString));

    add (makeBool ({ ParamID::cabEnabled, 1 }, "Cab Enable", true));
    add (makeFloat ({ ParamID::cabMix, 1 }, "Cab Mix",
                    { 0.0f, 1.0f, 0.0001f }, 1.0f, {}, percentString));
    add (makeFloat ({ ParamID::cabLowCut, 1 }, "Cab Low Cut",
                    { 40.0f, 200.0f, 0.1f }, 80.0f, "Hz", hzString));
    add (makeFloat ({ ParamID::cabHighCut, 1 }, "Cab High Cut",
                    { 3000.0f, 12000.0f, 0.1f }, 6500.0f, "Hz", hzString));
    add (makeChoice ({ ParamID::cabMonoStereo, 1 }, "Cab Routing", { "Mono", "Stereo" }, 1));

    add (makeBool ({ ParamID::harmEnabled, 1 }, "Harmony Enable", false));
    add (makeChoice ({ ParamID::harmInterval1, 1 }, "Interval 1",
                     harmonyIntervalChoices(), static_cast<int> (HarmonyInterval::PerfectFifth)));
    add (makeChoice ({ ParamID::harmInterval2, 1 }, "Interval 2",
                     harmonyIntervalChoices(), static_cast<int> (HarmonyInterval::Octave)));
    add (makeFloat ({ ParamID::harmMix, 1 }, "Harmony Mix",
                    { 0.0f, 1.0f, 0.0001f }, 0.25f, {}, percentString));
    add (makeFloat ({ ParamID::harmDetune, 1 }, "Detune",
                    { 0.0f, 25.0f, 0.01f }, 4.0f, "ct", centsString));
    add (makeFloat ({ ParamID::harmPan1, 1 }, "Pan 1",
                    { -1.0f, 1.0f, 0.0001f }, -0.4f));
    add (makeFloat ({ ParamID::harmPan2, 1 }, "Pan 2",
                    { -1.0f, 1.0f, 0.0001f }, 0.4f));

    add (makeChoice ({ ParamID::uiMode, 1 }, "UI Mode", { "Simple", "Advanced" }, 0));

    return { params.begin(), params.end() };
}
} // namespace WarlockAI

#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

namespace WarlockAI
{
namespace ParamID
{
    // Input / output — Phase 1 IDs (unchanged)
    inline constexpr auto inputGain     = "inputGain";
    inline constexpr auto outputGain    = "outputGain";

    // Distortion — Phase 1
    inline constexpr auto drive         = "drive";
    inline constexpr auto tone          = "tone";
    inline constexpr auto tightness     = "tightness";
    inline constexpr auto saturation    = "saturation";
    inline constexpr auto warmth        = "warmth";
    inline constexpr auto bite          = "bite";
    inline constexpr auto distortionOut = "distortionOut";
    inline constexpr auto distortionMode= "distortionMode";
    inline constexpr auto oversample    = "oversample";
    inline constexpr auto distortionBypass = "distortionBypass";

    // Gate — Phase 1
    inline constexpr auto gateThreshold = "gateThreshold";
    inline constexpr auto gateAttack    = "gateAttack";
    inline constexpr auto gateRelease   = "gateRelease";
    inline constexpr auto gateRatio     = "gateRatio";
    inline constexpr auto gateBypass    = "gateBypass";

    // Compressor — Phase 1 IDs kept
    inline constexpr auto compThreshold = "compThreshold";
    inline constexpr auto compRatio     = "compRatio";
    inline constexpr auto compAttack    = "compAttack";
    inline constexpr auto compRelease   = "compRelease";
    inline constexpr auto compMakeup    = "compMakeup";
    inline constexpr auto compBypass    = "compBypass";
    // Phase 2 additions (aliases + new)
    inline constexpr auto compressorEnabled   = "compressorEnabled";
    inline constexpr auto compressorThreshold = "compThreshold";
    inline constexpr auto compressorRatio     = "compRatio";
    inline constexpr auto compressorAttack    = "compAttack";
    inline constexpr auto compressorRelease   = "compRelease";
    inline constexpr auto compressorMakeup    = "compMakeup";
    inline constexpr auto compressorMix       = "compressorMix";

    // EQ — Phase 1
    inline constexpr auto eqLowGain     = "eqLowGain";
    inline constexpr auto eqLowFreq     = "eqLowFreq";
    inline constexpr auto eqLowMidGain  = "eqLowMidGain";
    inline constexpr auto eqLowMidFreq  = "eqLowMidFreq";
    inline constexpr auto eqLowMidQ     = "eqLowMidQ";
    inline constexpr auto eqHighMidGain = "eqHighMidGain";
    inline constexpr auto eqHighMidFreq = "eqHighMidFreq";
    inline constexpr auto eqHighMidQ    = "eqHighMidQ";
    inline constexpr auto eqHighGain    = "eqHighGain";
    inline constexpr auto eqHighFreq    = "eqHighFreq";
    inline constexpr auto eqBypass      = "eqBypass";

    // Delay — Phase 1 IDs kept
    inline constexpr auto delayTime     = "delayTime";
    inline constexpr auto delayFeedback = "delayFeedback";
    inline constexpr auto delayMix      = "delayMix";
    inline constexpr auto delayBypass   = "delayBypass";
    inline constexpr auto delayEnabled  = "delayEnabled";
    inline constexpr auto delayLowCut   = "delayLowCut";
    inline constexpr auto delayHighCut  = "delayHighCut";
    inline constexpr auto delayStereoWidth = "delayStereoWidth";
    inline constexpr auto delaySync     = "delaySync";
    inline constexpr auto delayMode     = "delayMode";

    // Reverb — Phase 1 IDs kept
    inline constexpr auto reverbSize    = "reverbSize";
    inline constexpr auto reverbDamping = "reverbDamping";
    inline constexpr auto reverbWidth   = "reverbWidth";
    inline constexpr auto reverbMix     = "reverbMix";
    inline constexpr auto reverbBypass  = "reverbBypass";
    inline constexpr auto reverbEnabled = "reverbEnabled";
    inline constexpr auto reverbPreDelay= "reverbPreDelay";

    // Stereo — Phase 1 IDs kept
    inline constexpr auto stereoWidth   = "stereoWidth";
    inline constexpr auto stereoBalance = "stereoBalance";
    inline constexpr auto stereoHaas    = "stereoHaas";
    inline constexpr auto stereoBypass  = "stereoBypass";
    inline constexpr auto stereoEnabled = "stereoEnabled";
    inline constexpr auto stereoPan     = "stereoBalance";
    inline constexpr auto monoMode      = "monoMode";
    inline constexpr auto midGain       = "midGain";
    inline constexpr auto sideGain      = "sideGain";

    // Limiter — Phase 1
    inline constexpr auto limiterCeiling= "limiterCeiling";
    inline constexpr auto limiterRelease= "limiterRelease";
    inline constexpr auto limiterBypass = "limiterBypass";

    // Amp — Phase 2
    inline constexpr auto ampEnabled    = "ampEnabled";
    inline constexpr auto ampMode       = "ampMode";
    inline constexpr auto ampGain       = "ampGain";
    inline constexpr auto ampBass       = "ampBass";
    inline constexpr auto ampMid        = "ampMid";
    inline constexpr auto ampTreble     = "ampTreble";
    inline constexpr auto ampPresence   = "ampPresence";
    inline constexpr auto ampResonance  = "ampResonance";
    inline constexpr auto ampMaster     = "ampMaster";

    // Cabinet — Phase 2
    inline constexpr auto cabEnabled    = "cabEnabled";
    inline constexpr auto cabMix        = "cabMix";
    inline constexpr auto cabLowCut     = "cabLowCut";
    inline constexpr auto cabHighCut    = "cabHighCut";
    inline constexpr auto cabMonoStereo = "cabMonoStereo";

    // Harmonizer — Phase 2
    inline constexpr auto harmEnabled   = "harmEnabled";
    inline constexpr auto harmInterval1 = "harmInterval1";
    inline constexpr auto harmInterval2 = "harmInterval2";
    inline constexpr auto harmMix       = "harmMix";
    inline constexpr auto harmDetune    = "harmDetune";
    inline constexpr auto harmPan1      = "harmPan1";
    inline constexpr auto harmPan2      = "harmPan2";

    // UI / workflow
    inline constexpr auto uiMode        = "uiMode";
}

enum class DistortionMode
{
    Tube = 0,
    Vintage,
    Modern,
    Metal,
    Djent,
    Extreme,
    AdaptiveAI,
    NumModes
};

enum class AmpMode
{
    Clean = 0,
    Crunch,
    Rock,
    Modern,
    Metal,
    Death,
    Black,
    Djent,
    NumModes
};

enum class HarmonyInterval
{
    Unison = 0,
    OctaveDown,
    OctaveUp,
    MinorThird,
    MajorThird,
    PerfectFifth,
    Octave,
    NumIntervals
};

enum class DelayMode
{
    Mono = 0,
    Stereo,
    PingPong,
    NumModes
};

enum class DelaySync
{
    Off = 0,
    Quarter,
    Eighth,
    EighthDotted,
    EighthTriplet,
    Sixteenth,
    NumModes
};

enum class UiMode
{
    Simple = 0,
    Advanced
};

inline const char* distortionModeName (int index) noexcept
{
    switch (static_cast<DistortionMode> (index))
    {
        case DistortionMode::Tube:       return "Tube";
        case DistortionMode::Vintage:    return "Vintage";
        case DistortionMode::Modern:     return "Modern";
        case DistortionMode::Metal:      return "Metal";
        case DistortionMode::Djent:      return "Djent";
        case DistortionMode::Extreme:    return "Extreme";
        case DistortionMode::AdaptiveAI: return "Adaptive AI";
        case DistortionMode::NumModes:   break;
    }

    return "Unknown";
}

inline const char* ampModeName (int index) noexcept
{
    switch (static_cast<AmpMode> (index))
    {
        case AmpMode::Clean:    return "Clean";
        case AmpMode::Crunch:   return "Crunch";
        case AmpMode::Rock:     return "Rock";
        case AmpMode::Modern:   return "Modern";
        case AmpMode::Metal:    return "Metal";
        case AmpMode::Death:    return "Death";
        case AmpMode::Black:    return "Black";
        case AmpMode::Djent:    return "Djent";
        case AmpMode::NumModes: break;
    }

    return "Unknown";
}

inline const char* harmonyIntervalName (int index) noexcept
{
    switch (static_cast<HarmonyInterval> (index))
    {
        case HarmonyInterval::Unison:       return "Unison";
        case HarmonyInterval::OctaveDown:   return "Octave Down";
        case HarmonyInterval::OctaveUp:     return "Octave Up";
        case HarmonyInterval::MinorThird:   return "Minor Third";
        case HarmonyInterval::MajorThird:   return "Major Third";
        case HarmonyInterval::PerfectFifth: return "Perfect Fifth";
        case HarmonyInterval::Octave:       return "Octave";
        case HarmonyInterval::NumIntervals: break;
    }

    return "Unknown";
}

inline float harmonyIntervalSemitones (int index) noexcept
{
    switch (static_cast<HarmonyInterval> (index))
    {
        case HarmonyInterval::Unison:       return 0.0f;
        case HarmonyInterval::OctaveDown:   return -12.0f;
        case HarmonyInterval::OctaveUp:     return 12.0f;
        case HarmonyInterval::MinorThird:   return 3.0f;
        case HarmonyInterval::MajorThird:   return 4.0f;
        case HarmonyInterval::PerfectFifth: return 7.0f;
        case HarmonyInterval::Octave:       return 12.0f;
        case HarmonyInterval::NumIntervals: break;
    }

    return 0.0f;
}

inline juce::StringArray distortionModeChoices()
{
    juce::StringArray names;
    const int count = static_cast<int> (DistortionMode::NumModes);
    for (int i = 0; i < count; ++i)
        names.add (distortionModeName (i));
    return names;
}

inline juce::StringArray ampModeChoices()
{
    juce::StringArray names;
    const int count = static_cast<int> (AmpMode::NumModes);
    for (int i = 0; i < count; ++i)
        names.add (ampModeName (i));
    return names;
}

inline juce::StringArray harmonyIntervalChoices()
{
    juce::StringArray names;
    const int count = static_cast<int> (HarmonyInterval::NumIntervals);
    for (int i = 0; i < count; ++i)
        names.add (harmonyIntervalName (i));
    return names;
}

inline juce::StringArray delayModeChoices()
{
    return { "Mono", "Stereo", "Ping Pong" };
}

inline juce::StringArray delaySyncChoices()
{
    return { "Free", "1/4", "1/8", "1/8 D", "1/8 T", "1/16" };
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

template <typename T>
inline T loadAtomicParam (std::atomic<T>* raw) noexcept
{
    return raw != nullptr ? raw->load (std::memory_order_relaxed) : T {};
}

inline float loadParam (std::atomic<float>* raw, float fallback = 0.0f) noexcept
{
    return raw != nullptr ? raw->load (std::memory_order_relaxed) : fallback;
}

inline bool loadBoolParam (std::atomic<float>* raw) noexcept
{
    return loadParam (raw) >= 0.5f;
}

struct ParameterSmoother
{
    void reset (double sampleRate, float rampSeconds, float initial) noexcept
    {
        smoother.reset (sampleRate, static_cast<double> (rampSeconds));
        smoother.setCurrentAndTargetValue (initial);
    }

    void setTarget (float value) noexcept
    {
        smoother.setTargetValue (value);
    }

    float getNext() noexcept
    {
        return smoother.getNextValue();
    }

    float getCurrent() const noexcept
    {
        return smoother.getCurrentValue();
    }

    void skip (int numSamples) noexcept
    {
        smoother.skip (numSamples);
    }

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoother;
};
} // namespace WarlockAI

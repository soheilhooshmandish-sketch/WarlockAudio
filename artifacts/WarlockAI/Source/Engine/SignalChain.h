#pragma once

#include "DSP/NoiseGate.h"
#include "DSP/Compressor.h"
#include "DSP/DistortionProcessor.h"
#include "DSP/AmpProcessor.h"
#include "DSP/CabinetProcessor.h"
#include "DSP/Equalizer.h"
#include "DSP/Harmonizer.h"
#include "DSP/DelayProcessor.h"
#include "DSP/ReverbProcessor.h"
#include "DSP/StereoProcessor.h"
#include "DSP/Limiter.h"
#include <JuceHeader.h>

namespace WarlockAI
{
struct SignalChainParameters
{
    float inputGainDb = 0.0f;
    float outputGainDb = 0.0f;
    float tempoBpm = 120.0f;

    bool gateBypass = false;
    float gateThresholdDb = -48.0f;
    float gateAttackMs = 1.5f;
    float gateReleaseMs = 80.0f;
    float gateRatio = 8.0f;

    bool compBypass = true;
    float compThresholdDb = -18.0f;
    float compRatio = 3.0f;
    float compAttackMs = 8.0f;
    float compReleaseMs = 120.0f;
    float compMakeupDb = 0.0f;
    float compMix = 1.0f;
    float compScHpfHz = 90.0f;
    float compScLpfHz = 0.0f;

    DistortionSettings distortion;

    bool ampEnabled = true;
    int ampMode = 4;
    float ampGain = 0.55f;
    float ampBass = 1.5f;
    float ampMid = -1.0f;
    float ampTreble = 2.0f;
    float ampPresence = 1.0f;
    float ampResonance = 0.35f;
    float ampMasterDb = 0.0f;

    bool cabEnabled = true;
    float cabMix = 1.0f;
    float cabLowCut = 80.0f;
    float cabHighCut = 6500.0f;
    bool cabStereo = true;

    bool eqBypass = false;
    float eqLowGain = 0.0f;
    float eqLowFreq = 110.0f;
    float eqLowMidGain = 0.0f;
    float eqLowMidFreq = 450.0f;
    float eqLowMidQ = 1.0f;
    float eqHighMidGain = 0.0f;
    float eqHighMidFreq = 1800.0f;
    float eqHighMidQ = 1.1f;
    float eqHighGain = 0.0f;
    float eqHighFreq = 6500.0f;

    bool harmEnabled = false;
    int harmInterval1 = 5;
    int harmInterval2 = 6;
    float harmMix = 0.25f;
    float harmDetune = 4.0f;
    float harmPan1 = -0.4f;
    float harmPan2 = 0.4f;

    bool delayBypass = true;
    float delayTimeMs = 320.0f;
    float delayFeedback = 0.25f;
    float delayMix = 0.0f;
    float delayLowCut = 80.0f;
    float delayHighCut = 8000.0f;
    float delayWidth = 1.0f;
    int delayMode = 1;
    int delaySync = 0;

    bool reverbBypass = true;
    float reverbSize = 0.35f;
    float reverbDamping = 0.5f;
    float reverbWidth = 1.0f;
    float reverbMix = 0.0f;
    float reverbPreDelayMs = 18.0f;

    bool stereoBypass = false;
    float stereoWidth = 1.0f;
    float stereoBalance = 0.0f;
    float stereoHaasMs = 0.0f;
    bool monoMode = false;
    float midGainDb = 0.0f;
    float sideGainDb = 0.0f;

    bool limiterBypass = false;
    float limiterCeilingDb = -0.3f;
    float limiterReleaseMs = 50.0f;
};

class SignalChain
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;
    void process (juce::AudioBuffer<float>& buffer) noexcept;
    void setParameters (const SignalChainParameters& params) noexcept;

    int getLatencySamples() const noexcept;

    DistortionProcessor& getDistortion() noexcept { return distortion; }
    Compressor& getCompressor() noexcept { return compressor; }
    const Compressor& getCompressor() const noexcept { return compressor; }
    DelayProcessor& getDelay() noexcept { return delay; }
    const DelayProcessor& getDelay() const noexcept { return delay; }
    ReverbProcessor& getReverb() noexcept { return reverb; }
    const ReverbProcessor& getReverb() const noexcept { return reverb; }
    StereoProcessor& getStereo() noexcept { return stereo; }
    const StereoProcessor& getStereo() const noexcept { return stereo; }
    CabinetProcessor& getCabinet() noexcept { return cabinet; }
    Harmonizer& getHarmonizer() noexcept { return harmonizer; }
    AmpProcessor& getAmp() noexcept { return amp; }

private:
    juce::dsp::Gain<float> inputGain;
    NoiseGate gate;
    Compressor compressor;
    DistortionProcessor distortion;
    AmpProcessor amp;
    CabinetProcessor cabinet;
    Equalizer equalizer;
    Harmonizer harmonizer;
    DelayProcessor delay;
    ReverbProcessor reverb;
    StereoProcessor stereo;
    Limiter limiter;
    juce::dsp::Gain<float> outputGain;

    SignalChainParameters current;
};
} // namespace WarlockAI

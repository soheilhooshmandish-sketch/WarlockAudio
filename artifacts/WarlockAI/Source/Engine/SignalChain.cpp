#include "Engine/SignalChain.h"

namespace WarlockAI
{
void SignalChain::prepare (const juce::dsp::ProcessSpec& spec)
{
    inputGain.prepare (spec);
    gate.prepare (spec);
    compressor.prepare (spec);
    distortion.prepare (spec);
    amp.prepare (spec);
    cabinet.prepare (spec);
    equalizer.prepare (spec);
    harmonizer.prepare (spec);
    delay.prepare (spec);
    reverb.prepare (spec);
    stereo.prepare (spec);
    limiter.prepare (spec);
    outputGain.prepare (spec);
    reset();
}

void SignalChain::reset() noexcept
{
    inputGain.reset();
    gate.reset();
    compressor.reset();
    distortion.reset();
    amp.reset();
    cabinet.reset();
    equalizer.reset();
    harmonizer.reset();
    delay.reset();
    reverb.reset();
    stereo.reset();
    limiter.reset();
    outputGain.reset();
}

int SignalChain::getLatencySamples() const noexcept
{
    return distortion.getLatencySamples()
         + cabinet.getLatencySamples()
         + harmonizer.getLatencySamples();
}

void SignalChain::setParameters (const SignalChainParameters& params) noexcept
{
    current = params;

    inputGain.setGainDecibels (params.inputGainDb);
    outputGain.setGainDecibels (params.outputGainDb);

    gate.setBypassed (params.gateBypass);
    gate.setThresholdDb (params.gateThresholdDb);
    gate.setAttackMs (params.gateAttackMs);
    gate.setReleaseMs (params.gateReleaseMs);
    gate.setRatio (params.gateRatio);

    compressor.setBypassed (params.compBypass);
    compressor.setThresholdDb (params.compThresholdDb);
    compressor.setRatio (params.compRatio);
    compressor.setAttackMs (params.compAttackMs);
    compressor.setReleaseMs (params.compReleaseMs);
    compressor.setMakeupDb (params.compMakeupDb);
    compressor.setMix (params.compMix);
    compressor.setSidechainHpf (params.compScHpfHz);
    compressor.setSidechainLpf (params.compScLpfHz);

    distortion.setSettings (params.distortion);
    distortion.setBypassed (params.distortion.bypassed);

    amp.setEnabled (params.ampEnabled);
    amp.setMode (params.ampMode);
    amp.setGain (params.ampGain);
    amp.setBass (params.ampBass);
    amp.setMid (params.ampMid);
    amp.setTreble (params.ampTreble);
    amp.setPresence (params.ampPresence);
    amp.setResonance (params.ampResonance);
    amp.setMasterDb (params.ampMasterDb);

    cabinet.setEnabled (params.cabEnabled);
    cabinet.setMix (params.cabMix);
    cabinet.setLowCut (params.cabLowCut);
    cabinet.setHighCut (params.cabHighCut);
    cabinet.setStereo (params.cabStereo);

    equalizer.setBypassed (params.eqBypass);
    equalizer.setLowShelf (params.eqLowGain, params.eqLowFreq);
    equalizer.setLowMid (params.eqLowMidGain, params.eqLowMidFreq, params.eqLowMidQ);
    equalizer.setHighMid (params.eqHighMidGain, params.eqHighMidFreq, params.eqHighMidQ);
    equalizer.setHighShelf (params.eqHighGain, params.eqHighFreq);

    harmonizer.setEnabled (params.harmEnabled);
    harmonizer.setInterval1 (params.harmInterval1);
    harmonizer.setInterval2 (params.harmInterval2);
    harmonizer.setMix (params.harmMix);
    harmonizer.setDetuneCents (params.harmDetune);
    harmonizer.setPan1 (params.harmPan1);
    harmonizer.setPan2 (params.harmPan2);

    delay.setBypassed (params.delayBypass);
    delay.setTimeMs (params.delayTimeMs);
    delay.setFeedback (params.delayFeedback);
    delay.setMix (params.delayMix);
    delay.setLowCut (params.delayLowCut);
    delay.setHighCut (params.delayHighCut);
    delay.setWidth (params.delayWidth);
    delay.setMode (params.delayMode);
    delay.setSync (params.delaySync);
    delay.setTempoBpm (params.tempoBpm);

    reverb.setBypassed (params.reverbBypass);
    reverb.setRoomSize (params.reverbSize);
    reverb.setDamping (params.reverbDamping);
    reverb.setWidth (params.reverbWidth);
    reverb.setMix (params.reverbMix);
    reverb.setPreDelayMs (params.reverbPreDelayMs);

    stereo.setBypassed (params.stereoBypass);
    stereo.setWidth (params.stereoWidth);
    stereo.setBalance (params.stereoBalance);
    stereo.setHaasMs (params.stereoHaasMs);
    stereo.setMonoMode (params.monoMode);
    stereo.setMidGainDb (params.midGainDb);
    stereo.setSideGainDb (params.sideGainDb);

    limiter.setBypassed (params.limiterBypass);
    limiter.setCeilingDb (params.limiterCeilingDb);
    limiter.setReleaseMs (params.limiterReleaseMs);
}

void SignalChain::process (juce::AudioBuffer<float>& buffer) noexcept
{
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    inputGain.process (context);
    gate.process (block);
    compressor.process (block);
    distortion.process (block);
    amp.process (block);
    cabinet.process (block);
    equalizer.process (block);
    harmonizer.process (block);
    delay.process (block);
    reverb.process (block);
    stereo.process (block);
    limiter.process (block);
    outputGain.process (context);
}
} // namespace WarlockAI

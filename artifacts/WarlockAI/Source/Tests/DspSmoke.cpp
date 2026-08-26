#include "Engine/SignalChain.h"
#include <cmath>
#include <iostream>

namespace
{
    bool isFiniteBuffer (const juce::AudioBuffer<float>& buffer)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const auto* data = buffer.getReadPointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                if (! std::isfinite (data[i]))
                    return false;
        }
        return true;
    }

    void fillNoise (juce::AudioBuffer<float>& buffer, unsigned seed)
    {
        unsigned state = seed;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                state = state * 1664525u + 1013904223u;
                data[i] = (static_cast<float> (state & 0xffffu) / 32768.0f - 1.0f) * 0.2f;
            }
        }
    }
}

int main()
{
    WarlockAI::SignalChain chain;
    WarlockAI::SignalChainParameters params;
    params.ampEnabled = true;
    params.cabEnabled = true;
    params.compBypass = false;
    params.compMix = 1.0f;
    params.delayBypass = false;
    params.delayMix = 0.25f;
    params.reverbBypass = false;
    params.reverbMix = 0.2f;
    params.harmEnabled = true;
    params.harmMix = 0.2f;
    params.distortion.bypassed = false;
    params.distortion.oversampleIndex = 1;

    juce::dsp::ProcessSpec stereoSpec { 48000.0, 256, 2 };
    chain.prepare (stereoSpec);
    chain.setParameters (params);

    juce::AudioBuffer<float> stereo (2, 256);
    fillNoise (stereo, 0xC0FFEEu);
    chain.process (stereo);
    if (! isFiniteBuffer (stereo))
    {
        std::cerr << "FAIL: stereo produced non-finite samples\n";
        return 1;
    }

    juce::dsp::ProcessSpec monoSpec { 48000.0, 256, 1 };
    chain.prepare (monoSpec);
    chain.setParameters (params);
    juce::AudioBuffer<float> mono (1, 256);
    fillNoise (mono, 0xF00Du);
    chain.process (mono);
    if (! isFiniteBuffer (mono))
    {
        std::cerr << "FAIL: mono produced non-finite samples\n";
        return 1;
    }

    chain.reset();
    params.delayMode = static_cast<int> (WarlockAI::DelayMode::PingPong);
    params.monoMode = true;
    chain.prepare (stereoSpec);
    chain.setParameters (params);
    fillNoise (stereo, 0xBEEFu);
    chain.process (stereo);
    if (! isFiniteBuffer (stereo))
    {
        std::cerr << "FAIL: ping-pong / mono-fold produced non-finite samples\n";
        return 1;
    }

    const int latency = chain.getLatencySamples();
    std::cout << "PASS: mono + stereo processing finite. latency=" << latency << " samples\n";
    return 0;
}

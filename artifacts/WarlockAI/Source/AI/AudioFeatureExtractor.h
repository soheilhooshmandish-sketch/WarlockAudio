#pragma once

#include <JuceHeader.h>
#include <vector>

namespace WarlockAI
{
struct AudioFeatures
{
    float rms = 0.0f;
    float peak = 0.0f;
    float peakDb = -100.0f;
    float rmsDb = -100.0f;
    float spectralCentroidHz = 0.0f;
    float spectralRolloffHz = 0.0f;
    float zeroCrossingRate = 0.0f;
    float dynamicRangeDb = 0.0f;
    float crestFactorDb = 0.0f;
    float brightness = 0.0f;   // 0 dark .. 1 bright
    float density = 0.0f;      // 0 sparse .. 1 saturated
    bool valid = false;

    // Phase 3 — still filled only on the analysis thread
    float spectralFlatness = 0.0f;
    float lowEnergy = 0.0f;
    float lowMidEnergy = 0.0f;
    float midEnergy = 0.0f;
    float highMidEnergy = 0.0f;
    float highEnergy = 0.0f;
    float harshness = 0.0f;
    float muddiness = 0.0f;
    float noiseFloorDb = -100.0f;
    float transientStrength = 0.0f;
    float sustain = 0.0f;
    float pitchStability = 0.0f;
};

class AudioFeatureExtractor
{
public:
    void prepare (double sampleRate, int fftOrder = 11);
    void reset();

    // Offline only. Must never be called from the audio thread.
    AudioFeatures extract (const juce::AudioBuffer<float>& buffer);

private:
    void ensureScratch (int numSamples);

    double sampleRate = 44100.0;
    int fftOrder = 11;
    int fftSize = 2048;
    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
    std::vector<float> fftData;
    std::vector<float> monoScratch;
};
} // namespace WarlockAI

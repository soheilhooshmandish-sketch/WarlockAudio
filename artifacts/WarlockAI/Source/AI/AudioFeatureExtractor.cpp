#include "AI/AudioFeatureExtractor.h"

namespace WarlockAI
{
void AudioFeatureExtractor::prepare (double newSampleRate, int newFftOrder)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    fftOrder = juce::jlimit (8, 14, newFftOrder);
    fftSize = 1 << fftOrder;
    fft = std::make_unique<juce::dsp::FFT> (fftOrder);
    window = std::make_unique<juce::dsp::WindowingFunction<float>> (
        static_cast<size_t> (fftSize), juce::dsp::WindowingFunction<float>::hann, true);
    fftData.assign (static_cast<size_t> (fftSize * 2), 0.0f);
    monoScratch.clear();
}

void AudioFeatureExtractor::reset()
{
    std::fill (fftData.begin(), fftData.end(), 0.0f);
}

void AudioFeatureExtractor::ensureScratch (int numSamples)
{
    if (static_cast<int> (monoScratch.size()) < numSamples)
        monoScratch.resize (static_cast<size_t> (numSamples), 0.0f);
}

AudioFeatures AudioFeatureExtractor::extract (const juce::AudioBuffer<float>& buffer)
{
    AudioFeatures features;
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples <= 0 || numChannels <= 0 || fft == nullptr)
        return features;

    ensureScratch (numSamples);
    std::fill (monoScratch.begin(), monoScratch.begin() + numSamples, 0.0f);

    const float chScale = 1.0f / static_cast<float> (numChannels);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* src = buffer.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            monoScratch[static_cast<size_t> (i)] += src[i] * chScale;
    }

    double sumSq = 0.0;
    float peak = 0.0f;
    int crossings = 0;
    float prev = 0.0f;
    double diffEnergy = 0.0;

    const int hop = juce::jmax (64, numSamples / 32);
    float hopMinRms = 1.0e6f;
    float hopMaxRms = 0.0f;
    double hopSumSq = 0.0;
    int hopCount = 0;
    int hopCross = 0;
    float hopCrossMean = 0.0f;
    float hopCrossVar = 0.0f;
    int hopWindows = 0;

    for (int i = 0; i < numSamples; ++i)
    {
        const float s = monoScratch[static_cast<size_t> (i)];
        sumSq += static_cast<double> (s) * static_cast<double> (s);
        peak = juce::jmax (peak, std::abs (s));
        hopSumSq += static_cast<double> (s) * static_cast<double> (s);
        ++hopCount;

        if (i > 0)
        {
            const float d = s - prev;
            diffEnergy += static_cast<double> (d) * static_cast<double> (d);
            if ((prev >= 0.0f && s < 0.0f) || (prev < 0.0f && s >= 0.0f))
            {
                ++crossings;
                ++hopCross;
            }
        }

        prev = s;

        if (hopCount >= hop)
        {
            const float hopRms = std::sqrt (static_cast<float> (hopSumSq / juce::jmax (1, hopCount)));
            hopMinRms = juce::jmin (hopMinRms, hopRms);
            hopMaxRms = juce::jmax (hopMaxRms, hopRms);

            const float z = static_cast<float> (hopCross) / static_cast<float> (hopCount);
            ++hopWindows;
            const float delta = z - hopCrossMean;
            hopCrossMean += delta / static_cast<float> (hopWindows);
            hopCrossVar += delta * (z - hopCrossMean);

            hopSumSq = 0.0;
            hopCount = 0;
            hopCross = 0;
        }
    }

    const float rms = std::sqrt (static_cast<float> (sumSq / juce::jmax (1, numSamples)));
    features.rms = rms;
    features.peak = peak;
    features.rmsDb = juce::Decibels::gainToDecibels (rms, -100.0f);
    features.peakDb = juce::Decibels::gainToDecibels (peak, -100.0f);
    features.crestFactorDb = features.peakDb - features.rmsDb;
    features.zeroCrossingRate = static_cast<float> (crossings) / static_cast<float> (juce::jmax (1, numSamples));
    features.dynamicRangeDb = juce::Decibels::gainToDecibels (juce::jmax (hopMaxRms, 1.0e-6f), -100.0f)
                            - juce::Decibels::gainToDecibels (juce::jmax (hopMinRms < 1.0e5f ? hopMinRms : rms, 1.0e-6f), -100.0f);
    features.noiseFloorDb = juce::Decibels::gainToDecibels (juce::jmax (hopMinRms < 1.0e5f ? hopMinRms : rms * 0.05f, 1.0e-8f), -100.0f);
    features.transientStrength = juce::jlimit (0.0f, 1.0f,
        std::sqrt (static_cast<float> (diffEnergy / juce::jmax (1, numSamples))) / (rms + 1.0e-6f) * 0.35f);

    const int q = juce::jmax (1, numSamples / 4);
    double firstSq = 0.0, lastSq = 0.0;
    for (int i = 0; i < q; ++i)
        firstSq += static_cast<double> (monoScratch[static_cast<size_t> (i)]) * monoScratch[static_cast<size_t> (i)];
    for (int i = numSamples - q; i < numSamples; ++i)
        lastSq += static_cast<double> (monoScratch[static_cast<size_t> (i)]) * monoScratch[static_cast<size_t> (i)];
    const float firstRms = std::sqrt (static_cast<float> (firstSq / q));
    const float lastRms = std::sqrt (static_cast<float> (lastSq / q));
    features.sustain = juce::jlimit (0.0f, 1.0f, lastRms / (firstRms + 1.0e-6f));

    const float zcrVar = hopWindows > 1 ? hopCrossVar / static_cast<float> (hopWindows - 1) : 0.0f;
    features.pitchStability = juce::jlimit (0.0f, 1.0f, 1.0f - juce::jmin (1.0f, zcrVar * 40.0f));

    const int analysisCount = juce::jmin (fftSize, numSamples);
    std::fill (fftData.begin(), fftData.end(), 0.0f);

    const int start = juce::jmax (0, numSamples - analysisCount);
    for (int i = 0; i < analysisCount; ++i)
        fftData[static_cast<size_t> (i)] = monoScratch[static_cast<size_t> (start + i)];

    window->multiplyWithWindowingTable (fftData.data(), static_cast<size_t> (fftSize));
    fft->performFrequencyOnlyForwardTransform (fftData.data());

    const int bins = fftSize / 2;
    double weighted = 0.0;
    double magnitudeSum = 0.0;
    double logSum = 0.0;
    int logCount = 0;
    double low = 0.0, lowMid = 0.0, mid = 0.0, highMid = 0.0, high = 0.0;

    auto bandOf = [] (double freq) -> int
    {
        if (freq < 80.0)    return 0;
        if (freq < 350.0)   return 1;
        if (freq < 1500.0)  return 2;
        if (freq < 4000.0)  return 3;
        return 4;
    };

    for (int bin = 1; bin < bins; ++bin)
    {
        const double mag = juce::jmax (1.0e-12, static_cast<double> (fftData[static_cast<size_t> (bin)]));
        const double freq = (static_cast<double> (bin) * sampleRate) / static_cast<double> (fftSize);
        weighted += mag * freq;
        magnitudeSum += mag;
        logSum += std::log (mag);
        ++logCount;

        switch (bandOf (freq))
        {
            case 0: low += mag; break;
            case 1: lowMid += mag; break;
            case 2: mid += mag; break;
            case 3: highMid += mag; break;
            default: high += mag; break;
        }
    }

    features.spectralCentroidHz = magnitudeSum > 1.0e-9
        ? static_cast<float> (weighted / magnitudeSum)
        : 0.0f;

    const double rolloffTarget = magnitudeSum * 0.85;
    double running = 0.0;
    features.spectralRolloffHz = static_cast<float> (sampleRate * 0.5);

    for (int bin = 1; bin < bins; ++bin)
    {
        running += static_cast<double> (fftData[static_cast<size_t> (bin)]);
        if (running >= rolloffTarget)
        {
            features.spectralRolloffHz = static_cast<float> ((static_cast<double> (bin) * sampleRate) / static_cast<double> (fftSize));
            break;
        }
    }

    const double arith = magnitudeSum / juce::jmax (1, logCount);
    const double geo = std::exp (logSum / juce::jmax (1, logCount));
    features.spectralFlatness = arith > 1.0e-12 ? juce::jlimit (0.0f, 1.0f, static_cast<float> (geo / arith)) : 0.0f;

    const double bandTotal = juce::jmax (1.0e-12, low + lowMid + mid + highMid + high);
    features.lowEnergy     = static_cast<float> (low / bandTotal);
    features.lowMidEnergy  = static_cast<float> (lowMid / bandTotal);
    features.midEnergy     = static_cast<float> (mid / bandTotal);
    features.highMidEnergy = static_cast<float> (highMid / bandTotal);
    features.highEnergy    = static_cast<float> (high / bandTotal);

    features.brightness = juce::jlimit (0.0f, 1.0f, (features.spectralCentroidHz - 400.0f) / 3600.0f);
    features.density = juce::jlimit (0.0f, 1.0f, (features.rms * 4.0f) + (1.0f - juce::jlimit (0.0f, 1.0f, features.crestFactorDb / 18.0f)));
    features.muddiness = juce::jlimit (0.0f, 1.0f, features.lowMidEnergy * 1.4f + (1.0f - features.brightness) * 0.35f);
    features.harshness = juce::jlimit (0.0f, 1.0f, features.highMidEnergy * 0.55f + features.highEnergy * 0.9f + features.brightness * 0.25f);
    features.valid = true;
    return features;
}
} // namespace WarlockAI

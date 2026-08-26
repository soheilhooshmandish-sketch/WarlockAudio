#include "AI/AIEngine.h"
#include "AI/AudioFeatureExtractor.h"
#include "AI/MixFit.h"
#include "AI/ONNXToneModel.h"
#include "AI/StyleDetector.h"
#include "AI/ToneAnalyzer.h"
#include "AI/ToneDesigner.h"
#include "AI/ToneHealth.h"
#include "DSP/DistortionProcessor.h"
#include "Core/WarlockParameters.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace
{
    juce::AudioBuffer<float> makeSilence (int n = 8192)
    {
        juce::AudioBuffer<float> b (1, n);
        b.clear();
        return b;
    }

    juce::AudioBuffer<float> makeSine (float hz, float amp, double sr, int n = 8192)
    {
        juce::AudioBuffer<float> b (1, n);
        for (int i = 0; i < n; ++i)
            b.setSample (0, i, amp * std::sin (juce::MathConstants<float>::twoPi * hz * static_cast<float> (i) / static_cast<float> (sr)));
        return b;
    }

    juce::AudioBuffer<float> makeHighGain (double sr, int n = 8192)
    {
        auto b = makeSine (110.0f, 0.9f, sr, n);
        for (int i = 0; i < n; ++i)
        {
            float s = b.getSample (0, i);
            s = juce::jlimit (-0.95f, 0.95f, s * 8.0f);
            if ((i % 180) < 8)
                s *= 1.4f;
            b.setSample (0, i, s);
        }
        return b;
    }

    juce::AudioBuffer<float> makeNoise (int n = 8192)
    {
        juce::AudioBuffer<float> b (1, n);
        unsigned state = 0xC0FFEEu;
        for (int i = 0; i < n; ++i)
        {
            state = state * 1664525u + 1013904223u;
            b.setSample (0, i, (static_cast<float> (state & 0xffffu) / 32768.0f - 1.0f) * 0.25f);
        }
        return b;
    }

    bool inRange (float v, float lo, float hi) { return v >= lo && v <= hi; }

    int fail (const char* msg)
    {
        std::cerr << "FAIL: " << msg << "\n";
        return 1;
    }
}

int main()
{
    constexpr double sr = 48000.0;
    WarlockAI::AudioFeatureExtractor extractor;
    extractor.prepare (sr, 11);
    WarlockAI::ToneAnalyzer analyzer;
    WarlockAI::ToneHealth health;
    WarlockAI::StyleDetector styles;
    WarlockAI::ToneDesigner designer;
    WarlockAI::MixFit mixFit;
    WarlockAI::AIEngine engine;
    engine.prepare (sr);

    {
        const auto f = extractor.extract (makeSilence());
        if (! f.valid) return fail ("silence should still be a valid capture");
        if (f.rms > 1.0e-4f) return fail ("silence RMS should be near zero");
        const auto h = health.evaluateDetailed (f, analyzer.analyseDetailed (f));
        bool weak = false;
        for (const auto& p : h.problems)
            if (p.problem == WarlockAI::HealthProblem::WeakSignal) weak = true;
        if (! weak) return fail ("silence should report WeakSignal");
    }

    {
        const auto f = extractor.extract (makeSine (220.0f, 0.2f, sr));
        if (! f.valid) return fail ("clean sine invalid");
        if (f.spectralFlatness > 0.6f) return fail ("sine should not be spectrally flat");
        const auto a = analyzer.analyseDetailed (f);
        if (! inRange (a.brightness, 0.0f, 1.0f)) return fail ("brightness range");
        const auto cls = analyzer.analyse (f);
        if (cls.classification == WarlockAI::ToneClass::Unknown && cls.confidence > 0.8f)
            return fail ("unknown classification with high confidence");
    }

    {
        const auto f = extractor.extract (makeHighGain (sr));
        if (f.density < 0.2f) return fail ("high-gain density too low");
        const auto h = health.evaluateDetailed (f, analyzer.analyseDetailed (f));
        if (! inRange (h.score, 0.0f, 100.0f)) return fail ("health score range");
    }

    {
        const auto f = extractor.extract (makeNoise());
        if (f.spectralFlatness < 0.15f) return fail ("noise should have higher flatness");
        const auto h = health.evaluateDetailed (f, analyzer.analyseDetailed (f));
        bool noiseHit = false;
        for (const auto& p : h.problems)
            if (p.problem == WarlockAI::HealthProblem::ExcessiveNoise) noiseHit = true;
        if (! noiseHit && f.spectralFlatness > 0.5f)
            return fail ("noisy capture should flag ExcessiveNoise");
    }

    {
        const auto f = extractor.extract (makeHighGain (sr));
        const auto a = analyzer.analyseDetailed (f);
        const auto st = styles.detect (f, a);
        if (st.confidence > 0.92f) return fail ("style confidence must stay conservative");
        if (st.confidence < 0.05f) return fail ("style confidence unexpectedly zero");
        WarlockAI::CurrentToneState cur;
        const auto rec = designer.recommend (f, a, st, cur);
        if (! inRange (rec.drive, 0.0f, 1.0f) || ! inRange (rec.cabLowCut, 40.0f, 200.0f)
            || ! inRange (rec.compressorRatio, 1.0f, 20.0f))
            return fail ("recommendation outside safe ranges");
        if (rec.explanations.empty()) return fail ("recommendation missing explanation");
    }

    {
        const auto f = extractor.extract (makeHighGain (sr));
        const auto a = analyzer.analyseDetailed (f);
        WarlockAI::CurrentToneState cur;
        cur.drive = 0.8f;
        const auto fit = mixFit.fit (f, a, 60.0f, cur, WarlockAI::DistortionMode::Metal);
        if (fit.afterEstimatedScore + 0.01f < fit.beforeScore) return fail ("mix fit should not lower estimated score");
        for (const auto& ch : fit.changes)
        {
            if (ch.parameterId == WarlockAI::ParamID::drive && std::abs (ch.after - ch.before) > 0.2f)
                return fail ("mix fit drive change too aggressive");
            if ((ch.parameterId == WarlockAI::ParamID::eqLowMidGain || ch.parameterId == WarlockAI::ParamID::eqHighGain)
                && std::abs (ch.after - ch.before) > 6.0f)
                return fail ("mix fit EQ change too aggressive");
        }
    }

    {
        WarlockAI::DistortionProcessor dist;
        juce::dsp::ProcessSpec spec { sr, 256, 2 };
        dist.prepare (spec);
        WarlockAI::DistortionSettings s;
        s.mode = static_cast<int> (WarlockAI::DistortionMode::AdaptiveAI);
        s.drive = 0.5f;
        s.adaptiveRmsDb = -24.0f;
        dist.setSettings (s);
        juce::AudioBuffer<float> buf (2, 256);
        for (int i = 0; i < 256; ++i)
        {
            buf.setSample (0, i, 0.2f * std::sin (0.1f * static_cast<float> (i)));
            buf.setSample (1, i, buf.getSample (0, i));
        }
        for (int block = 0; block < 24; ++block)
        {
            if (block == 8)
            {
                s.adaptiveRmsDb = -6.0f;
                s.adaptivePicking = 0.9f;
                dist.setSettings (s);
            }
            juce::dsp::AudioBlock<float> blockView (buf);
            dist.process (blockView);
            for (int i = 0; i < 256; ++i)
                if (! std::isfinite (buf.getSample (0, i)))
                    return fail ("adaptive distortion produced non-finite samples");
        }
    }

    {
        auto hg = makeHighGain (sr);
        WarlockAI::CurrentToneState cur;
        const auto pack = engine.analyse (hg, cur, WarlockAI::DistortionMode::Metal);
        if (! pack.ready) return fail ("AIEngine did not produce a result");
        const auto copy = engine.copyPublished();
        if (copy.healthResult.score != pack.healthResult.score) return fail ("published pack mismatch");
        const auto hints = engine.loadHints();
        if (! std::isfinite (hints.rmsDb)) return fail ("adaptive hints not finite");
    }

    {
        auto layout = WarlockAI::createParameterLayout();
        (void) layout;
    }

    {
        WarlockAI::ONNXToneModel onnx;
        WarlockAI::CurrentToneState cur;
        WarlockAI::ToneAnalysisResult analysis;
        WarlockAI::AudioFeatures features;
        const auto rec = onnx.predict (features, analysis, cur);
        if (rec.confidence != 0.0f)
            return fail ("ONNX stub must not report a trained model");
        if (! rec.reason.contains ("not linked"))
            return fail ("ONNX stub must state that runtime is not linked");
    }

    std::cout << "PASS: feature extraction, silence, clean, high-gain, noise, health, "
                 "recommendations, style confidence, mix-fit, adaptive smoothing\n";
    return 0;
}

#pragma once

#include "AI/ToneAnalyzer.h"
#include <JuceHeader.h>
#include <array>

namespace WarlockAI
{
enum class GuitarStyle
{
    ModernMetal = 0,
    DeathMetal,
    BlackMetal,
    ThrashMetal,
    Metalcore,
    Djent,
    ProgressiveMetal,
    DoomMetal,
    HeavyMetal,
    HardRock,
    Clean,
    Ambient,
    NumStyles
};

inline const char* guitarStyleName (GuitarStyle style) noexcept
{
    switch (style)
    {
        case GuitarStyle::ModernMetal:      return "Modern Metal";
        case GuitarStyle::DeathMetal:       return "Death Metal";
        case GuitarStyle::BlackMetal:       return "Black Metal";
        case GuitarStyle::ThrashMetal:      return "Thrash Metal";
        case GuitarStyle::Metalcore:        return "Metalcore";
        case GuitarStyle::Djent:            return "Djent";
        case GuitarStyle::ProgressiveMetal: return "Progressive Metal";
        case GuitarStyle::DoomMetal:        return "Doom Metal";
        case GuitarStyle::HeavyMetal:       return "Heavy Metal";
        case GuitarStyle::HardRock:         return "Hard Rock";
        case GuitarStyle::Clean:            return "Clean";
        case GuitarStyle::Ambient:          return "Ambient";
        case GuitarStyle::NumStyles:        break;
    }
    return "Unknown";
}

struct StyleDetectionResult
{
    GuitarStyle style = GuitarStyle::HeavyMetal;
    float confidence = 0.0f;
    juce::String label { "Unknown" };
    juce::String rationale;
    std::array<float, static_cast<size_t> (GuitarStyle::NumStyles)> scores {};
};

class StyleDetector
{
public:
    StyleDetectionResult detect (const AudioFeatures& features,
                                 const ToneAnalysisResult& analysis) const;
};
} // namespace WarlockAI

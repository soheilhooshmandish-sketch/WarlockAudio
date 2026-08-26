#pragma once

#include "AI/AIEngine.h"
#include "UI/WarlockLookAndFeel.h"
#include <JuceHeader.h>

namespace WarlockAI
{
class AiDashboard final : public juce::Component
{
public:
    AiDashboard();

    void paint (juce::Graphics& g) override;
    void setPack (const AIResultPack& pack);
    void setAnalysing (bool shouldAnalyse) noexcept;
    void setPreviewActive (bool active) noexcept;
    bool isAnalysing() const noexcept { return analysing; }

private:
    AIResultPack pack;
    bool analysing = false;
    bool previewActive = false;
};
} // namespace WarlockAI

#include "UI/AiDashboard.h"

namespace WarlockAI
{
AiDashboard::AiDashboard()
{
    setOpaque (false);
}

void AiDashboard::setPack (const AIResultPack& newPack)
{
    pack = newPack;
    repaint();
}

void AiDashboard::setAnalysing (bool shouldAnalyse) noexcept
{
    if (analysing == shouldAnalyse)
        return;
    analysing = shouldAnalyse;
    repaint();
}

void AiDashboard::setPreviewActive (bool active) noexcept
{
    previewActive = active;
    repaint();
}

void AiDashboard::paint (juce::Graphics& g)
{
    ForgeColours c;
    auto bounds = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (c.panel);
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (c.ai.withAlpha (0.55f));
    g.fillRect (bounds.getX(), bounds.getY(), 3.0f, bounds.getHeight());
    g.setColour (c.outline);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    auto inner = getLocalBounds().reduced (14, 10);
    g.setColour (c.ai);
    g.setFont (juce::FontOptions().withHeight (13.0f).withStyle ("Bold"));
    g.drawText ("TONE AI", inner.removeFromTop (18), juce::Justification::centredLeft);

    if (analysing)
    {
        const float t = static_cast<float> (juce::Time::getMillisecondCounter() % 1000) / 1000.0f;
        auto ring = juce::Rectangle<float> (static_cast<float> (getWidth() - 34), 12.0f, 16.0f, 16.0f);
        g.setColour (c.ai.withAlpha (0.25f));
        g.drawEllipse (ring, 2.0f);
        juce::Path arc;
        arc.addCentredArc (ring.getCentreX(), ring.getCentreY(), 7.0f, 7.0f, 0.0f,
                           t * juce::MathConstants<float>::twoPi,
                           t * juce::MathConstants<float>::twoPi + 1.8f, true);
        g.setColour (c.ai);
        g.strokePath (arc, juce::PathStrokeType (2.0f));
        g.setColour (c.muted);
        g.setFont (juce::FontOptions (12.0f));
        g.drawText ("Analysing locally...", inner.removeFromTop (22), juce::Justification::centredLeft);
        return;
    }

    const int score = pack.ready ? juce::roundToInt (pack.healthResult.score) : 0;
    g.setColour (c.text);
    g.setFont (juce::FontOptions().withHeight (22.0f).withStyle ("Bold"));
    g.drawText ("Tone Score  " + juce::String (score) + "/100",
                inner.removeFromTop (26), juce::Justification::centredLeft);

    g.setFont (juce::FontOptions (13.0f));
    g.setColour (c.aiCyan);
    const auto styleName = pack.ready ? pack.style.label : juce::String ("-");
    const int stylePct = pack.ready ? juce::roundToInt (pack.style.confidence * 100.0f) : 0;
    g.drawText ("Style  " + styleName + "  " + juce::String (stylePct) + "%",
                inner.removeFromTop (20), juce::Justification::centredLeft);

    g.setColour (c.bronze);
    g.drawText ("Tone Health  " + (pack.ready ? pack.healthResult.grade : juce::String ("Idle")),
                inner.removeFromTop (20), juce::Justification::centredLeft);

    inner.removeFromTop (6);
    g.setColour (c.muted);
    g.setFont (juce::FontOptions (11.5f));
    g.drawText ("Detected", inner.removeFromTop (16), juce::Justification::centredLeft);
    g.setColour (c.text);
    if (pack.ready && ! pack.healthResult.positives.empty())
    {
        for (int i = 0; i < juce::jmin (3, static_cast<int> (pack.healthResult.positives.size())); ++i)
            g.drawText ("+ " + pack.healthResult.positives[static_cast<size_t> (i)],
                        inner.removeFromTop (16), juce::Justification::centredLeft);
    }
    else
    {
        g.drawText ("+ Awaiting capture", inner.removeFromTop (16), juce::Justification::centredLeft);
    }

    inner.removeFromTop (4);
    g.setColour (c.distortion);
    g.setFont (juce::FontOptions (11.5f));
    g.drawText ("Warnings", inner.removeFromTop (16), juce::Justification::centredLeft);
    g.setColour (c.text);
    int shown = 0;
    if (pack.ready)
    {
        for (const auto& issue : pack.healthResult.problems)
        {
            if (shown >= 3)
                break;
            g.drawFittedText ("- " + issue.description, inner.removeFromTop (32),
                              juce::Justification::topLeft, 2);
            ++shown;
        }
    }
    if (shown == 0)
        g.drawText ("- None", inner.removeFromTop (16), juce::Justification::centredLeft);

    if (pack.ready && ! pack.recommendation.explanations.empty())
    {
        inner.removeFromTop (4);
        g.setColour (c.aiCyan);
        g.setFont (juce::FontOptions (11.0f));
        g.drawFittedText (pack.recommendation.explanations.front(),
                          inner.removeFromTop (36), juce::Justification::topLeft, 3);
    }

    if (previewActive)
    {
        g.setColour (c.bronze);
        g.setFont (juce::FontOptions().withHeight (11.0f).withStyle ("Bold"));
        g.drawText ("MIX FIT PREVIEW", inner.removeFromBottom (16), juce::Justification::centredLeft);
    }
}
} // namespace WarlockAI

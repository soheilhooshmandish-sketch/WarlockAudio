#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <cmath>
// ==============================================================================
// THE INVERTED PENTAGRAM LOOKANDFEEL COMPONENT RE-DRAW ENGINE
// ==============================================================================
class WarlockOccultLookAndFeel : public juce::LookAndFeel_V4
{
public:
    WarlockOccultLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(150, 10, 10));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB(20, 2, 2));
    }
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
                          float startAngle, float endAngle, juce::Slider& slider) override
    {
        auto radius = static_cast<float>(juce::jmin(width, height) / 2.0 - 6.0);
        auto centreX = static_cast<float>(x + width * 0.5);
        auto centreY = static_cast<float>(y + height * 0.5);
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = startAngle + sliderPosProportional * (endAngle - startAngle);

        g.setColour(juce::Colour::fromRGB(12, 12, 14));
        g.fillEllipse(rx, ry, rw, rw);

        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawEllipse(rx, ry, rw, rw, 3.5f);

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        juce::Path star;
        float scalingFactor = radius * 0.85f;
        float pointsX[5];
        float pointsY[5];
        for (int i = 0; i < 5; ++i)
        {
            float localAngle = angle + static_cast<float>(i * 2.0 * juce::MathConstants<double>::pi / 5.0) + juce::MathConstants<float>::pi;
            pointsX[i] = centreX + std::sin(localAngle) * scalingFactor;
            pointsY[i] = centreY - std::cos(localAngle) * scalingFactor;
        }
        star.startNewSubPath(pointsX[0], pointsY[0]);
        star.lineTo(pointsX[2], pointsY[2]);
        star.lineTo(pointsX[4], pointsY[4]);
        star.lineTo(pointsX[1], pointsY[1]);
        star.lineTo(pointsX[3], pointsY[3]);
        star.closeSubPath();
        g.strokePath(star, juce::PathStrokeType(2.0f, juce::PathStrokeType::mitered));

        g.setColour(juce::Colour::fromRGB(255, 40, 40));
        g.fillEllipse(pointsX[0] - 3.0f, pointsY[0] - 3.0f, 6.0f, 6.0f);
    }
};
// ==============================================================================
// CORE EDITOR PANEL DEFINITIONS MATRIX
// ==============================================================================
class WarlockAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    WarlockAudioProcessorEditor (WarlockAudioProcessor&);
    ~WarlockAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    WarlockAudioProcessor& audioProcessor;
    WarlockOccultLookAndFeel satanicLookAndFeel;
    juce::Slider boostSlider, gainSlider, coldSlider, grimSlider, gateSlider, releaseSlider;
    juce::Slider subSlider, psychosisSlider;
    juce::Slider eqLowSlider, eqMidSlider, eqHighSlider, levelSlider;
    juce::ComboBox diodeSelector, cabSelector, routingSelector, ritualSelector;
    juce::Image logoImage;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> boostAttach, gainAttach, coldAttach, grimAttach, gateAttach, releaseAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subAttach, psychosisAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqLowAttach, eqMidAttach, eqHighAttach, levelAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> diodeAttach, cabAttach, routingAttach;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarlockAudioProcessorEditor)
};

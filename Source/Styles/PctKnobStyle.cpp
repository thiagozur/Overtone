#include "PctKnobStyle.h"

PctKnobStyle::PctKnobStyle()
{

}

void PctKnobStyle::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> (static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)).reduced (10.0f);
    auto tooltipGap = bounds.removeFromBottom (15);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreY = bounds.getCentreY();
    auto centreX = bounds.getCentreX();

    const int numTicks = 9;
    const float tickRadiusOuter = radius * 1.1f;
    const float tickRadiusInner = radius * 0.9f;

    g.setColour (juce::Colours::white.withAlpha (0.8f));

    for (int i = 0; i < numTicks; ++i)
    {
        float angle = rotaryStartAngle + (i / static_cast<float>(numTicks - 1)) * (rotaryEndAngle - rotaryStartAngle);

        auto outerPoint = juce::Point<float> (centreX + tickRadiusOuter * std::sin (angle), centreY - tickRadiusOuter * std::cos (angle));
        auto innerPoint = juce::Point<float> (centreX + tickRadiusInner * std::sin (angle), centreY - tickRadiusInner * std::cos (angle));

        g.drawLine (juce::Line<float> (innerPoint, outerPoint), 1.5f);
    }

    g.setFont (syne.withHeight (12.0f));
    g.setColour (juce::Colours::white.withAlpha (0.8f));

    auto minPoint = juce::Point<float> (centreX + (radius + 12.0f) * std::sin (rotaryStartAngle), centreY - (radius + 12.0f) * std::cos (rotaryStartAngle));
    g.drawText (juce::String (slider.getRange().getStart() * 100), juce::Rectangle<float> (22.0f, 12.0f).withCentre (minPoint), juce::Justification::centred, false);
    
    auto maxPoint = juce::Point<float> (centreX + (radius + 12.0f) * std::sin (rotaryEndAngle), centreY - (radius + 12.0f) * std::cos (rotaryEndAngle));
    g.drawText (juce::String (slider.getRange().getEnd() * 100), juce::Rectangle<float> (22.0f, 12.0f).withCentre (maxPoint), juce::Justification::centred, false);

    auto knobRadius = radius * 0.75f;
    auto knobBounds = juce::Rectangle<float> (centreX - knobRadius, centreY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

    juce::Colour knobBaseColour (0XFFF5EBD3);
    g.setColour (knobBaseColour);
    g.fillEllipse (knobBounds);

    juce::ColourGradient borderGrad (knobBaseColour.brighter (0.25f), centreX, centreY - knobRadius, knobBaseColour.darker (2.0f), centreX, centreY + knobRadius, false);
    g.setGradientFill (borderGrad);
    g.drawEllipse (knobBounds, 3.0f);

    float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        
    juce::Path cursor;
    auto cursorLength = knobRadius * 0.55f;
    auto cursorThickness = 2.0f;
    auto overhang = 2.0f;

    cursor.addRectangle (-cursorThickness * 0.5f, -knobRadius - overhang, cursorThickness, cursorLength + overhang);
    cursor.applyTransform (juce::AffineTransform::rotation (currentAngle).translated (centreX, centreY));

    g.setColour (knobBaseColour.darker (6.0f).withAlpha (0.8f));
    g.fillPath (cursor);

    if (slider.isMouseButtonDown())
    {
        juce::String valueText = juce::String(slider.getValue() * 100) + "%";
        if (valueText.isEmpty())
            valueText = juce::String (slider.getValue(), 1);
        
        juce::Font tooltipFont (syne.withHeight (11.0f));
        g.setFont (tooltipFont);
        
        juce::GlyphArrangement glyphs;
        glyphs.addFittedText (tooltipFont, valueText, 0.0f, 0.0f, 1000.0f, 20.0f, juce::Justification::left, 1);
        int textWidth = static_cast<int> (glyphs.getBoundingBox (0, -1, true).getWidth()) + 14;
        int textHeight = 18;

        float bubbleX = centreX - (textWidth / 2.0f);
        float bubbleY = centreY + knobRadius + 8.0f;

        juce::Rectangle<float> bubbleArea (bubbleX, bubbleY, static_cast<float>(textWidth), static_cast<float>(textHeight));

        g.setColour (juce::Colours::black.withAlpha (0.4f));
        g.fillRoundedRectangle (bubbleArea.translated (0.0f, 1.5f), 4.0f);

        g.setColour (slider.findColour(OvertoneStyle::darkBackgroundColourId));
        g.fillRoundedRectangle (bubbleArea, 4.0f);

        g.setColour (slider.findColour (OvertoneStyle::accentColourId).withAlpha (0.8f));
        g.drawRoundedRectangle (bubbleArea, 4.0f, 1.0f);

        g.setColour (juce::Colours::white);
        g.drawText (valueText, bubbleArea, juce::Justification::centred, false);
    }
}
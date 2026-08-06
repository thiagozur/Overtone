#pragma once

#include <JuceHeader.h>
#include "OvertoneStyle.h"

class PctKnobStyle : public OvertoneStyle
{
public:
    PctKnobStyle();
    ~PctKnobStyle() override = default;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
};
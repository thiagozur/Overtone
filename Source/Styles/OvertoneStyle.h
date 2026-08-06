#pragma once

#include <JuceHeader.h>

class OvertoneStyle : public juce::LookAndFeel_V4
{
public:
    enum OvertoneColourIds
    {
        backgroundColourId = 0x2000001,
        darkBackgroundColourId = 0x2000002,
        accentColourId = 0x2000003,
        pastelToneId = 0x2000004
    };

    OvertoneStyle();
    ~OvertoneStyle() override = default;

    void drawLabel (juce::Graphics& g, juce::Label& label) override;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
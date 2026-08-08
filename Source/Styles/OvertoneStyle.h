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

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override;

    juce::AlertWindow* createAlertWindow (const juce::String& title, const juce::String& message, const juce::String& button1, const juce::String& button2, const juce::String& button3, juce::MessageBoxIconType iconType, int numButtons, juce::Component* associatedComponent) override;

    void drawAlertBox (juce::Graphics& g, juce::AlertWindow& alert, const juce::Rectangle<int>& textArea, juce::TextLayout& textLayout) override;

    juce::Font getAlertWindowMessageFont() override;
    juce::Font getAlertWindowTitleFont() override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::FontOptions montserrat;
    juce::FontOptions syne;
    juce::FontOptions syneBold;

    static juce::Font getMontserratFont (juce::Component& comp, float height)
    {
        if (auto* style = dynamic_cast<OvertoneStyle*> (&comp.getLookAndFeel()))
            return style->montserrat.withHeight (height);

        return juce::Font (juce::FontOptions().withHeight (height));
    }

    static juce::Font getSyneFont (juce::Component& comp, float height)
    {
        if (auto* style = dynamic_cast<OvertoneStyle*> (&comp.getLookAndFeel()))
            return style->syne.withHeight (height);

        return juce::Font (juce::FontOptions().withHeight (height));
    }

    static juce::Font getSyneBoldFont (juce::Component& comp, float height)
    {
        if (auto* style = dynamic_cast<OvertoneStyle*> (&comp.getLookAndFeel()))
            return style->syneBold.withHeight (height);

        return juce::Font (juce::FontOptions().withHeight (height));
    }    

private:
    juce::Typeface::Ptr montserratTypeface;
    juce::Typeface::Ptr syneTypeface;
    juce::Typeface::Ptr syneBoldTypeface;
};
#include "OvertoneStyle.h"

OvertoneStyle::OvertoneStyle()
{
    montserratTypeface = juce::Typeface::createSystemTypefaceFor (
        BinaryData::MontserratBold_ttf,
        BinaryData::MontserratBold_ttfSize
    );

    if (montserratTypeface != nullptr)
        montserrat = juce::FontOptions (montserratTypeface).withHeight (18.0f);

    syneTypeface = juce::Typeface::createSystemTypefaceFor (
        BinaryData::SyneBold_ttf,
        BinaryData::SyneBold_ttfSize
    );

    if (syneTypeface != nullptr)
        syne = juce::FontOptions (syneTypeface).withHeight (12.0f);

    syneBoldTypeface = juce::Typeface::createSystemTypefaceFor (
        BinaryData::SyneExtraBold_ttf,
        BinaryData::SyneExtraBold_ttfSize
    );

    if (syneBoldTypeface != nullptr)
        syneBold = juce::FontOptions (syneBoldTypeface).withHeight (12.0f);

    setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF5E6153));
    setColour (juce::ComboBox::arrowColourId, juce::Colours::white.withAlpha (0.9f));
    setColour (juce::ComboBox::textColourId, juce::Colours::white.withAlpha (0.8f));

    setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xFF2D360C).darker (0.5f));
    setColour (juce::TextEditor::textColourId, juce::Colours::white);
    setColour (juce::TextEditor::highlightColourId, juce::Colour (0xFFADC944).withAlpha (0.4f));
    setColour (juce::TextEditor::outlineColourId, juce::Colour (0xFFADC944).withAlpha (0.5f));
    setColour (juce::TextEditor::focusedOutlineColourId, juce::Colour (0xFFADC944));

    setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xFF00A0D2));

    setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.8f));

    setColour (backgroundColourId, juce::Colour (0xFF2D360C));
    setColour (darkBackgroundColourId, juce::Colour (0xFF2D360C).darker (0.9f));
    setColour (accentColourId, juce::Colour (0xFFADC944));
    setColour (pastelToneId, juce::Colour (0xFF4C5C12));
}

void OvertoneStyle::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId));

    if (! label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;

        g.setFont (syne.withHeight (16.0f));
        g.setColour (label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha));

        auto area = label.getLocalBounds().toFloat();
        g.drawFittedText (label.getText(), area.getSmallestIntegerContainer(), juce::Justification::centred, 2);
    }
}

void OvertoneStyle::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
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
    g.drawText (juce::String (static_cast<int>(slider.getRange().getStart())), juce::Rectangle<float> (22.0f, 12.0f).withCentre (minPoint), juce::Justification::centred, false);
    
    auto maxPoint = juce::Point<float> (centreX + (radius + 12.0f) * std::sin (rotaryEndAngle), centreY - (radius + 12.0f) * std::cos (rotaryEndAngle));
    g.drawText (juce::String (static_cast<int>(slider.getRange().getEnd())), juce::Rectangle<float> (22.0f, 12.0f).withCentre (maxPoint), juce::Justification::centred, false);

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
        juce::String valueText = slider.getTextFromValue (slider.getValue());
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

void OvertoneStyle::drawComboBox (juce::Graphics& g, int width, int height, bool /* isButtonDown */, int /* buttonX */, int /* buttonY */, int /* buttonW */, int /* buttonH */, juce::ComboBox& box)
{
    auto cornerSize = 4.0f;
    auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();

    g.setColour (box.findColour (darkBackgroundColourId));
    g.fillRoundedRectangle(bounds.reduced (2.0f), cornerSize);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (bounds.reduced (2.0f), cornerSize, 1.0f);

    juce::Path arrow;
    float arrowW = 8.0f;
    float arrowH = 4.0f;
    float centerX = width - 18.0f;
    float centerY = height / 2.0f;

    arrow.startNewSubPath (centerX - arrowW / 2.0f, centerY - arrowH / 2.0f);
    arrow.lineTo (centerX + arrowW / 2.0f, centerY - arrowH /2.0f);
    arrow.lineTo (centerX, centerY + arrowH / 2.0f);
    arrow.closeSubPath();

    g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha (box.isEnabled() ? 1.0f : 0.3f));
    g.fillPath (arrow);
}

void OvertoneStyle::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    g.fillAll (findColour (darkBackgroundColourId));

    g.setColour (findColour (accentColourId).withAlpha (0.4f));
    g.drawRect (0, 0, width, height, 1);
}

void OvertoneStyle::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool /* isTicked */, bool /* hasSubMenu */, const juce::String& text, const juce::String& /* shortcutKeyText */, const juce::Drawable* /* icon */, const juce::Colour* /* textColour */)
{
    if (isSeparator)
    {
        auto r = area.reduced (5, 0);
        g.setColour (juce::Colours::white.withAlpha (0.1f));
        g.fillRect (r.removeFromTop (1).withHeight (1));
        return;
    }

    auto r = area.reduced (1);

    if (isHighlighted && isActive)
    {
        g.setColour (findColour (accentColourId));

        g.fillRect (r);

        g.setColour (findColour (darkBackgroundColourId));
    }

    else
        g.setColour (juce::Colours::white.withAlpha (0.8f).withMultipliedAlpha (isActive ? 1.0f : 0.5f));

    g.setFont (syne.withHeight (14.0f));

    g.drawText (text, r.reduced (12, 0), juce::Justification::centredLeft, true); 
}

juce::AlertWindow* OvertoneStyle::createAlertWindow (const juce::String& title, const juce::String& message, const juce::String& button1, const juce::String& button2, const juce::String& button3, juce::MessageBoxIconType iconType, int numButtons, juce::Component* associatedComponent)
{
    auto* alert = LookAndFeel_V4::createAlertWindow (title, message, button1, button2, button3, iconType, numButtons, associatedComponent);

    if (alert != nullptr)
    {
        alert->setOpaque (false);

        for (auto* child : alert->getChildren())
        {
            if (dynamic_cast<juce::Label*>(child) != nullptr)
                child->setVisible (false);
        }
    }

    return alert;
}

juce::Font OvertoneStyle::getAlertWindowTitleFont()
{
    return juce::Font (montserrat.withHeight (22.0f)).boldened();
}

juce::Font OvertoneStyle::getAlertWindowMessageFont()
{
    return juce::Font (syne.withHeight (18.0f));
}

void OvertoneStyle::drawAlertBox (juce::Graphics& g, juce::AlertWindow& alert, const juce::Rectangle<int>& textArea, juce::TextLayout& textLayout)
{
    auto bounds = alert.getLocalBounds().toFloat();
    auto cornerRadius = 4.0f;

    g.setColour (alert.findColour (darkBackgroundColourId));
    g.fillRoundedRectangle (bounds, cornerRadius);

    g.setColour (findColour (accentColourId));
    g.drawRoundedRectangle (bounds.reduced (0.5f), cornerRadius, 1.5f);

    textLayout.draw (g, textArea.toFloat().reduced (20.0f, 0.0f));
}

void OvertoneStyle::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& /* backgroundColour */, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerSize = 4.0f;

    auto baseColour = button.getToggleState() ? button.findColour (juce::TextButton::buttonOnColourId) : button.findColour (darkBackgroundColourId);

    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker (0.4f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter (0.15f);

    if (! button.isEnabled())
        baseColour = baseColour.withAlpha (0.35f);

    g.setColour (baseColour);
    g.fillRoundedRectangle (bounds.reduced (2.0f), cornerSize);

    auto outlineColour = button.isEnabled() ? (shouldDrawButtonAsHighlighted ? juce::Colour (0xFF5E6153).brighter (0.15f) : juce::Colour (0xFF5E6153)) : juce::Colour (0xFF535660).withAlpha (0.2f);

    g.setColour (outlineColour);
    g.drawRoundedRectangle (bounds.reduced (2.0f), cornerSize, 1.0f);
}

void OvertoneStyle::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool /* shouldDrawButtonAsHighlighted */, bool /* shouldDrawButtonAsDown */)
{
    auto area = button.getLocalBounds().toFloat();

    g.setFont (syne.withHeight (16.0f));

    auto alpha = button.isEnabled() ? 1.0f : 0.5f;
    g.setColour (juce::Colours::white.withAlpha (0.8f).withMultipliedAlpha (alpha));

    g.drawFittedText (button.getButtonText(), area.getSmallestIntegerContainer(), juce::Justification::centred, 2);
}
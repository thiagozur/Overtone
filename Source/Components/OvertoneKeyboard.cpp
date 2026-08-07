#include "OvertoneKeyboard.h"

void OvertoneKeyboard::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float cornerSize = 8.0f;

    juce::Path chassisPath;
    chassisPath.addRoundedRectangle (bounds, cornerSize);
    g.reduceClipRegion (chassisPath);

    g.setColour (juce::Colour (0xFF12160C));
    g.fillPath (chassisPath);

    g.setColour (getLookAndFeel().findColour (OvertoneStyle::darkBackgroundColourId));
    g.drawRoundedRectangle (bounds.reduced (2.0f), cornerSize - 1.0f, 4.0f);

    auto keyBedArea = bounds.reduced (7.0f);
    {
        juce::Graphics::ScopedSaveState saver (g);
        g.reduceClipRegion (keyBedArea.toNearestInt());

        juce::MidiKeyboardComponent::paint (g);

        juce::ColourGradient shadow (juce::Colour (0xCC0A0D07), 0.0f, keyBedArea.getY(), juce::Colours::transparentBlack, 0.0f, keyBedArea.getY() + 10.0f, false);
        g.setGradientFill (shadow);
        g.fillRect (keyBedArea.removeFromTop (10.0f));
    }

    g.setColour (getLookAndFeel().findColour (OvertoneStyle::pastelToneId));
    g.drawRoundedRectangle (bounds.reduced (5.5f), cornerSize - 3.0f, 1.5f);
}

void OvertoneKeyboard::drawWhiteNote (int /* midiNoteNumber */, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour /* lineColour */, juce::Colour /* textColour */)
{
    auto baseCream = juce::Colour (0xFFF5EBD3);
    auto hoverCream = juce::Colour (0xFFE1DCC8);
    auto activeOlive = juce::Colour (0xFFA5B94B);
    auto outlineDark = juce::Colour (0xFF14190F);

    auto fillColour = isDown ? activeOlive : (isOver ? hoverCream : baseCream);
        
    auto keyRect = area.reduced (0.25f, 1.0f);

    g.setColour (fillColour);
    g.fillRoundedRectangle (keyRect, 2.0f);

    g.setColour (outlineDark);
    g.drawRoundedRectangle (keyRect, 2.0f, 1.0f);
}

void OvertoneKeyboard::drawBlackNote (int /* midiNoteNumber */, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour /* noteFillColour */)
{
    auto baseDark = juce::Colour (0xFF191E14);
    auto hoverDark = juce::Colour (0xFF2D3723);
    auto activeOrange = juce::Colour (0xFFEBAB3D);
    auto edgeHighlight = juce::Colour (0x26FFFFFF);
    auto outlineDark = juce::Colour (0xFF000000);

    auto fillColour = isDown ? activeOrange : (isOver ? hoverDark : baseDark);
    auto keyRect = area.reduced (0.5f, 0.0f);

    g.setColour (fillColour);
    g.fillRoundedRectangle (keyRect, 2.0f);

    g.setColour (edgeHighlight);
    g.fillRect (keyRect.removeFromTop (2.0f));

    g.setColour (outlineDark);
    g.drawRoundedRectangle (area.reduced (0.5f, 0.0f), 2.0f, 1.0f);
}
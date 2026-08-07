#pragma once

#include <JuceHeader.h>
#include "../Styles/OvertoneStyle.h"

class OvertoneKeyboard : public juce::MidiKeyboardComponent
{
public:
    OvertoneKeyboard (juce::MidiKeyboardState& state, Orientation orientation) : juce::MidiKeyboardComponent (state, orientation)
    {
    
    }

    void paint (juce::Graphics& g) override;

    void drawWhiteNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour) override;

    void drawBlackNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour noteFillColour) override;
};
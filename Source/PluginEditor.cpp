#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertoneAudioProcessorEditor::OvertoneAudioProcessorEditor (OvertoneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard), genericEditor (p)
{
    addAndMakeVisible (genericEditor);

    addAndMakeVisible (keyboardComponent);
    setWantsKeyboardFocus (true);

    setSize (500, 700);
}

OvertoneAudioProcessorEditor::~OvertoneAudioProcessorEditor() {}

void OvertoneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void OvertoneAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    keyboardComponent.setBounds (bounds.removeFromTop (80).reduced (5));
    
    genericEditor.setBounds (bounds);
}
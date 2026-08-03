#include "PluginProcessor.h"
#include "PluginEditor.h"

FrequenzAudioProcessorEditor::FrequenzAudioProcessorEditor (FrequenzAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), genericEditor (p)
{
    addAndMakeVisible (genericEditor);

    triggerButton.setButtonText ("Play C4 Note (Hold)");
    addAndMakeVisible (triggerButton);

    triggerButton.onStateChange = [this]()
    {
        if (triggerButton.isDown())
            audioProcessor.triggerNoteOn();
        else
            audioProcessor.triggerNoteOff();
    };

    setSize (450, 650);
}

FrequenzAudioProcessorEditor::~FrequenzAudioProcessorEditor() {}

void FrequenzAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void FrequenzAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    triggerButton.setBounds (bounds.removeFromTop (50).reduced (10, 5));
    
    genericEditor.setBounds (bounds);
}
#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertoneAudioProcessorEditor::OvertoneAudioProcessorEditor (OvertoneAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      orbitNodes (audioProcessor.getAPVTS()),
      envelopeGraph (audioProcessor.getAPVTS()),
      noisePanel (audioProcessor.getAPVTS()),
      keyboardComponent (audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible (orbitNodes);
    addAndMakeVisible (envelopeGraph);
    addAndMakeVisible (noisePanel);

    addAndMakeVisible (keyboardComponent);
    keyboardComponent.setKeyWidth (22.0f);

    setSize (850, 580);
}

OvertoneAudioProcessorEditor::~OvertoneAudioProcessorEditor() {}

void OvertoneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (18, 20, 24));
}

void OvertoneAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (10);

    keyboardComponent.setBounds (bounds.removeFromBottom (70));
    
    bounds.removeFromBottom (10);

    orbitNodes.setBounds (bounds.removeFromLeft (420));

    bounds.removeFromLeft (10);

    envelopeGraph.setBounds (bounds.removeFromTop (210));

    bounds.removeFromTop (10);

    noisePanel.setBounds (bounds);
}
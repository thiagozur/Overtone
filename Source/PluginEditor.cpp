#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertoneAudioProcessorEditor::OvertoneAudioProcessorEditor (OvertoneAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      headerBar (audioProcessor.getAPVTS(), audioProcessor.getPresetManager()),
      orbitNodes (audioProcessor),
      envelopeGraph (audioProcessor.getAPVTS()),
      noisePanel (audioProcessor.getAPVTS()),
      rack (audioProcessor.getAPVTS()), 
      keyboardComponent (audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&overtoneStyle);

    addAndMakeVisible (headerBar);

    headerBar.onPageSwitched = [this] (int pageIndex)
    {
        updatePageVisibility (pageIndex);
    };

    addAndMakeVisible (orbitNodes);
    addAndMakeVisible (envelopeGraph);
    addAndMakeVisible (noisePanel);

    addAndMakeVisible (keyboardComponent);
    keyboardComponent.setKeyWidth (22.0f);

    addAndMakeVisible (rack);

    setSize (850, 620);
}

OvertoneAudioProcessorEditor::~OvertoneAudioProcessorEditor() {}

void OvertoneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (OvertoneStyle::backgroundColourId));
}

void OvertoneAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    headerBar.setBounds (bounds.removeFromTop (42));

    bounds.reduce (10, 10);

    keyboardComponent.setBounds (bounds.removeFromBottom (70));
    bounds.removeFromBottom (10);

    if (headerBar.getCurrentPageIndex() == 0)
    {
        orbitNodes.setBounds (bounds.removeFromLeft (420));
        bounds.removeFromLeft (10);

        envelopeGraph.setBounds (bounds.removeFromBottom ((bounds.getHeight() - 10) / 2));
        bounds.removeFromBottom (10);

        noisePanel.setBounds (bounds);
    }
    else if (headerBar.getCurrentPageIndex() == 1)
        rack.setBounds (bounds);
}

void OvertoneAudioProcessorEditor::updatePageVisibility (int pageIndex)
{
    bool isSynthPage = (pageIndex == 0);
    bool isFxPage = (pageIndex == 1);

    orbitNodes.setVisible (isSynthPage);
    envelopeGraph.setVisible (isSynthPage);
    noisePanel.setVisible (isSynthPage);
    rack.setVisible (isFxPage);
    
    resized();
}
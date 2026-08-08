#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertoneAudioProcessorEditor::OvertoneAudioProcessorEditor (OvertoneAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      headerBar (audioProcessor.getAPVTS(), audioProcessor.getPresetManager()),
      orbitNodes (audioProcessor),
      envelopeGraph (audioProcessor.getAPVTS()),
      noisePanel (audioProcessor, audioProcessor.getAPVTS()),
      rack (audioProcessor.getAPVTS()), 
      keyboardComponent (audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    juce::LookAndFeel::setDefaultLookAndFeel (&overtoneStyle);
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
    keyboardComponent.setAvailableRange (36, 96);
    keyboardComponent.setScrollButtonsVisible (false);

    addAndMakeVisible (rack);

    setSize (850, 650);
}

OvertoneAudioProcessorEditor::~OvertoneAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

void OvertoneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (OvertoneStyle::backgroundColourId));

    auto bounds = getLocalBounds();
    bounds.removeFromTop (42); //headerBarBounds
    bounds.reduce (10, 10);
    bounds.removeFromBottom (90); //keyboardBounds
    bounds.removeFromBottom (10);
    bounds.removeFromLeft (420); //orbitNodesBounds
    bounds.removeFromLeft (10);
    bounds.removeFromBottom ((bounds.getHeight() - 10) / 2); //adsrBounds
    bounds.removeFromBottom (5);
    auto adsrTitleBounds = bounds.removeFromBottom (20).reduced (8, 0);
    auto noiseSourceTitleBounds = bounds.removeFromTop (20).reduced (8, 0);

    g.setColour (juce::Colours::white.withAlpha (0.7f));
    g.setFont (OvertoneStyle::getMontserratFont(*this, 20.0f));
    g.drawText ("Envolvente", adsrTitleBounds, juce::Justification::centredLeft);
    g.drawText ("Fuente de Ruido", noiseSourceTitleBounds, juce::Justification::centredLeft);
}

void OvertoneAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    headerBar.setBounds (bounds.removeFromTop (42));

    bounds.reduce (10, 10);

    auto keyboardArea = bounds.removeFromBottom (90).reduced (48, 6);
    keyboardComponent.setBounds (keyboardArea);

    constexpr float numWhiteKeys = 36.0f;
    float keyWidth = static_cast<float>(keyboardArea.getWidth()) / numWhiteKeys;
    keyboardComponent.setKeyWidth (keyWidth);

    bounds.removeFromBottom (10);

    if (headerBar.getCurrentPageIndex() == 0)
    {
        orbitNodes.setBounds (bounds.removeFromLeft (420));
        bounds.removeFromLeft (10);

        envelopeGraph.setBounds (bounds.removeFromBottom ((bounds.getHeight() - 10) / 2));
        bounds.removeFromBottom (5);
        bounds.removeFromBottom (20); //adsrTitleBounds
        bounds.removeFromBottom (10);
        bounds.removeFromTop (20); //noiseSourceTitleBounds
        bounds.removeFromTop (5);

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
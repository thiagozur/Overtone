#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertoneAudioProcessorEditor::OvertoneAudioProcessorEditor (OvertoneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard), genericEditor (p)
{
    addAndMakeVisible (keyboardComponent);
    addAndMakeVisible (loadFileButton);
    addAndMakeVisible (genericEditor);

    loadFileButton.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Selecione un archivo de ruido",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav;*.mp3;*.aiff;*.flac"
        );

        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file.existsAsFile())
            {
                if (audioProcessor.loadCustomNoiseFile(file))
                {
                    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.apvts.getParameter("noise_source")))
                        *choiceParam = 5;

                    loadFileButton.setButtonText("Cargado: " + file.getFileName() + "(¿Cambiar?)");
                }
            }
        });
    };

    setWantsKeyboardFocus (true);

    setSize (500, 750);
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

    loadFileButton.setBounds (bounds.removeFromTop (40).reduced (10, 5));
    
    genericEditor.setBounds (bounds);
}
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class OvertoneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OvertoneAudioProcessorEditor (OvertoneAudioProcessor&);
    ~OvertoneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OvertoneAudioProcessor& audioProcessor;

    juce::MidiKeyboardComponent keyboardComponent;
    juce::GenericAudioProcessorEditor genericEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertoneAudioProcessorEditor)
};
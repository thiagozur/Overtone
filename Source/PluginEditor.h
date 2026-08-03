#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class FrequenzAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FrequenzAudioProcessorEditor (FrequenzAudioProcessor&);
    ~FrequenzAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    FrequenzAudioProcessor& audioProcessor;

    juce::TextButton triggerButton { "Play C4 Note" };
    juce::GenericAudioProcessorEditor genericEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequenzAudioProcessorEditor)
};
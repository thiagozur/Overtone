#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/OrbitNodes.h"
#include "Components/EnvelopeGraph.h"
#include "Components/NoiseSource.h"

class OvertoneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OvertoneAudioProcessorEditor (OvertoneAudioProcessor&);
    ~OvertoneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OvertoneAudioProcessor& audioProcessor;

    OrbitNodes orbitNodes;
    EnvelopeGraph envelopeGraph;
    NoiseSource noisePanel;

    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertoneAudioProcessorEditor)
};
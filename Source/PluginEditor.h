#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/HeaderBar.h"
#include "Components/OrbitNodes.h"
#include "Components/EnvelopeGraph.h"
#include "Components/NoiseSource.h"
#include "Components/Rack.h"

class OvertoneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OvertoneAudioProcessorEditor (OvertoneAudioProcessor&);
    ~OvertoneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OvertoneAudioProcessor& audioProcessor;

    HeaderBar headerBar;
    OrbitNodes orbitNodes;
    EnvelopeGraph envelopeGraph;
    NoiseSource noisePanel;
    Rack rack;

    juce::MidiKeyboardComponent keyboardComponent;

    void updatePageVisibility (int pageIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertoneAudioProcessorEditor)
};
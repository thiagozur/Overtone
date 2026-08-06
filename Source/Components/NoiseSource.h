#pragma once

#include <JuceHeader.h>
#include "../Styles/OvertoneStyle.h"

class NoiseSource : public juce::Component, public juce::FileDragAndDropTarget
{   
public:
    NoiseSource (juce::AudioProcessorValueTreeState& apvtsToUse);
    ~NoiseSource() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragMove (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::ComboBox noiseSelector;
    juce::Slider noiseGainSlider;
    juce::Label noiseGainLabel;

    juce::Slider qSlider;
    juce::Label qLabel;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<ComboAttachment> selectorAttach;
    std::unique_ptr<SliderAttachment> gainAttach;
    std::unique_ptr<SliderAttachment> qAttach;

    bool isDraggingFileOver = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseSource)
};
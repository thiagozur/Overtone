#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../Styles/PctKnobStyle.h"

class NoiseSource : public juce::Component
{   
public:
    NoiseSource (OvertoneAudioProcessor& p, juce::AudioProcessorValueTreeState& apvtsToUse);
    ~NoiseSource() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    OvertoneAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    PctKnobStyle pctKnobStyle;

    juce::ComboBox noiseSelector;
    juce::TextButton importNoiseButton { "Importar ruido " };
    void importButtonClicked();
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::Slider noiseGainSlider;
    juce::Label noiseGainLabel;

    juce::Slider qSlider;
    juce::Label qLabel;

    juce::Slider directNoiseSlider;
    juce::Label directNoiseLabel;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<ComboAttachment> selectorAttach;
    std::unique_ptr<SliderAttachment> gainAttach;
    std::unique_ptr<SliderAttachment> qAttach;
    std::unique_ptr<SliderAttachment> directNoiseAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseSource)
};
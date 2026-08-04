#pragma once

#include <JuceHeader.h>

class Rack : public juce::Component
{
public:
    Rack (juce::AudioProcessorValueTreeState& apvtsToUse);
    ~Rack() override;
    
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::Slider driveSlider;
    juce::Label driveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;

    juce::Slider widthSlider;
    juce::Label widthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttach;

    juce::Slider reverbMixSlider;
    juce::Label reverbMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbMixAttach;

    juce::Slider reverbSizeSlider;
    juce::Label reverbSizeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbSizeAttach;

    juce::Slider shimmerAmountSlider;
    juce::Label shimmerAmountLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shimmerAmountAttach;

    void setupRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void drawRackModule (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Rack)
};
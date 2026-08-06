#pragma once

#include <JuceHeader.h>
#include "../Styles/PctKnobStyle.h"

class EnvelopeGraph : public juce::Component, private juce::AudioProcessorValueTreeState::Listener, private juce::Timer
{
public:
    EnvelopeGraph (juce::AudioProcessorValueTreeState& aptvsToUse);
    ~EnvelopeGraph() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvts;
    PctKnobStyle pctKnobStyle;

    std::atomic<float> attackVal { 0.1f };
    std::atomic<float> decayVal { 0.3f };
    std::atomic<float> sustainVal { 0.8f };
    std::atomic<float> releaseVal { 0.5f };

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> attackAttach;
    std::unique_ptr<SliderAttachment> decayAttach;
    std::unique_ptr<SliderAttachment> sustainAttach;
    std::unique_ptr<SliderAttachment> releaseAttach;

    const juce::Colour graphColour { juce::Colour::fromRGB (255, 180, 80) };
    const juce::Colour fillColour { juce::Colour::fromRGBA (255, 180, 80, 30) };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeGraph)
};
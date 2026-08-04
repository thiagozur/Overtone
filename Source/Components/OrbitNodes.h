#pragma once

#include <JuceHeader.h>

class OrbitNodes : public juce::Component, private juce::AudioProcessorValueTreeState::Listener, private juce::Timer
{
public:
    OrbitNodes (juce::AudioProcessorValueTreeState& apvtsToUse);
    ~OrbitNodes() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void timerCallback() override;

    void updateNodePositions();
    int getSelectedNodeIndex (juce::Point<float> mousePos) const;

    juce::AudioProcessorValueTreeState& apvts;

    static constexpr int numHarmonics = 8;

    std::array<std::atomic<float>, numHarmonics> harmonicGains;
    std::atomic<float> currentQ { 50.0f };

    juce::Point<float> centerPoint;
    float minOrbitRadius = 35.0f;
    float maxOrbitRadius = 100.0f;

    struct NodeInfo
    {
        juce::Point<float> currentPos;
        float baseAngle = 0.0f;
        bool isDragging = false;
    };

    std::array<NodeInfo, numHarmonics> nodes;
    int activeDraggingIndex = -1;

    const juce::Colour coreColour { juce::Colour::fromRGB (255, 180, 80) };
    const juce::Colour nodeColour { juce::Colour::fromRGB (80, 200, 255) };
    const juce::Colour tetherColour { juce::Colour::fromRGBA (80, 200, 255, 100) };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbitNodes)
};
#include "OrbitNodes.h"

OrbitNodes::OrbitNodes (juce::AudioProcessorValueTreeState& apvtsToUse) : apvts (apvtsToUse)
{
    for (int i = 0; i < numHarmonics; ++i)
    {
        juce::String paramID = "harmonic_gain_" + juce::String (i + 1);
        apvts.addParameterListener (paramID, this);
        harmonicGains[i].store (*apvts.getRawParameterValue (paramID));

        nodes[i].baseAngle = juce::MathConstants<float>::twoPi * (static_cast<float>(i) / static_cast<float>(numHarmonics)) - juce::MathConstants<float>::halfPi;
    }

    apvts.addParameterListener ("resonance_q", this);
    currentQ.store (*apvts.getRawParameterValue ("resonance_q"));

    startTimerHz (60);
}

OrbitNodes::~OrbitNodes()
{
    for (int i = 0; i < numHarmonics; ++i)
        apvts.removeParameterListener ("harmonic_gain_" + juce::String (i + 1), this);

    apvts.removeParameterListener ("resonance_q", this);
}

void OrbitNodes::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "resonance_q")
        currentQ.store (newValue);
    else if (parameterID.startsWith ("harmonic_gain_"))
    {
        int index = parameterID.getTrailingIntValue() - 1;
        if (index >= 0 && index < numHarmonics)
            harmonicGains[index].store (newValue);
    }
}

void OrbitNodes::timerCallback()
{
    updateNodePositions();
    repaint();
}

void OrbitNodes::resized()
{
    centerPoint = getLocalBounds().getCentre().toFloat();
    minOrbitRadius = 35.0f;
    maxOrbitRadius = juce::jmin (getWidth(), getHeight()) * 0.4f;
    updateNodePositions();
}

void OrbitNodes::updateNodePositions()
{
    for (int i = 0; i < numHarmonics; ++i)
    {
        if (nodes[i].isDragging)
            continue;
        
        float gain = harmonicGains[i].load();
        float dist = juce::jmap (gain, 0.0f, 1.0f, minOrbitRadius, maxOrbitRadius);

        nodes[i].currentPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * dist,
            std::sin (nodes[i].baseAngle) * dist
        );
    }
}

void OrbitNodes::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (18, 20, 24));

    float qVal = currentQ.load();
    float normQ = juce::jmap (qVal, 5.0f, 150.0f, 1.0f, 0.05f); 

    g.setColour (juce::Colours::white.withAlpha (0.04f));
    g.drawEllipse (centerPoint.x - maxOrbitRadius, centerPoint.y - maxOrbitRadius, maxOrbitRadius * 2.0f, maxOrbitRadius * 2.0f, 1.0f);
    g.drawEllipse (centerPoint.x - minOrbitRadius, centerPoint.y - minOrbitRadius, minOrbitRadius * 2.0f, minOrbitRadius * 2.0f, 1.0f);

    for (int i = 0; i < numHarmonics; ++i)
    {
        auto pos = nodes[i].currentPos;
        float gain = harmonicGains[i].load();

        juce::Point<float> minPos = centerPoint + juce::Point<float> (std::cos (nodes[i].baseAngle) * minOrbitRadius, std::sin (nodes[i].baseAngle) * minOrbitRadius);
        juce::Point<float> maxPos = centerPoint + juce::Point<float> (std::cos (nodes[i].baseAngle) * maxOrbitRadius, std::sin (nodes[i].baseAngle) * maxOrbitRadius);
        
        g.setColour (juce::Colours::white.withAlpha (0.08f));
        g.drawLine (minPos.x, minPos.y, maxPos.x, maxPos.y, 1.0f);

        g.setColour (tetherColour.withMultipliedAlpha (0.2f + gain * 0.8f));
        g.drawLine (minPos.x, minPos.y, pos.x, pos.y, 1.5f + gain * 1.5f);

        if (gain > 0.01f)
        {
            float ringRadius = (10.0f + gain * 14.0f) * normQ; 
            float ringAlpha = juce::jlimit (0.05f, 0.7f, normQ * 0.6f);

            g.setColour (nodeColour.withAlpha (ringAlpha));
            g.drawEllipse (pos.x - ringRadius, pos.y - ringRadius, ringRadius * 2.0f, ringRadius * 2.0f, 1.0f + normQ * 1.0f);
        }

        float nodeRadius = 7.0f + gain * 3.0f;
        g.setColour (nodes[i].isDragging ? juce::Colours::white : nodeColour.withMultipliedAlpha (0.4f + gain * 0.6f));
        g.fillEllipse (pos.x - nodeRadius, pos.y - nodeRadius, nodeRadius * 2.0f, nodeRadius * 2.0f);

        float labelRadius = maxOrbitRadius + 26.0f; 
        
        juce::Point<float> labelPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * labelRadius,
            std::sin (nodes[i].baseAngle) * labelRadius
        );

        g.setColour (gain > 0.05f ? juce::Colours::white : juce::Colours::white.withAlpha (0.35f));
        g.setFont (juce::FontOptions (14.0f, juce::Font::bold));
        g.drawText (juce::String (i + 1), static_cast<int>(labelPos.x - 12), static_cast<int>(labelPos.y - 12), 24, 24, juce::Justification::centred);
    }

    float coreRadius = 14.0f;
    g.setColour (coreColour.withAlpha (0.25f));
    g.fillEllipse (centerPoint.x - coreRadius * 1.4f, centerPoint.y - coreRadius * 1.4f, coreRadius * 2.8f, coreRadius * 2.8f);

    g.setColour (coreColour);
    g.fillEllipse (centerPoint.x - coreRadius, centerPoint.y - coreRadius, coreRadius * 2.0f, coreRadius * 2.0f);
}

int OrbitNodes::getSelectedNodeIndex (juce::Point<float> mousePos) const
{
    for (int i = 0; i < numHarmonics; ++i)
    {
        if (mousePos.getDistanceFrom (nodes[i].currentPos) <= 18.0f)
            return i;
    }

    return -1;
}

void OrbitNodes::mouseDown (const juce::MouseEvent& e)
{
    activeDraggingIndex = getSelectedNodeIndex (e.position);
    if (activeDraggingIndex >= 0)
        nodes[activeDraggingIndex].isDragging = true;
}

void OrbitNodes::mouseDrag (const juce::MouseEvent& e)
{
    if (activeDraggingIndex < 0)
        return;
    
    float dist = centerPoint.getDistanceFrom (e.position);
    float newGain = juce::jmap (juce::jlimit (minOrbitRadius, maxOrbitRadius, dist), minOrbitRadius, maxOrbitRadius, 0.0f, 1.0f);

    float clampedDist = juce::jmap (newGain, 0.0f, 1.0f, minOrbitRadius, maxOrbitRadius);
    nodes[activeDraggingIndex].currentPos = centerPoint + juce::Point<float> (
        std::cos (nodes[activeDraggingIndex].baseAngle) * clampedDist,
        std::sin (nodes[activeDraggingIndex].baseAngle) * clampedDist
    );

    harmonicGains[activeDraggingIndex].store (newGain);

    juce::String paramID = "harmonic_gain_" + juce::String (activeDraggingIndex + 1);
    if (auto* param = apvts.getParameter (paramID))
        param->setValueNotifyingHost (newGain);
}

void OrbitNodes::mouseUp (const juce::MouseEvent& /* e */)
{
    if (activeDraggingIndex >= 0)
    {
        nodes[activeDraggingIndex].isDragging = false;
        activeDraggingIndex = -1;
    }
}
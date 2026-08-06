#include "OrbitNodes.h"
#include <iostream>

OrbitNodes::OrbitNodes (OvertoneAudioProcessor& processorToUse) : processor (processorToUse), apvts (processor.getAPVTS())
{
    for (int i = 0; i < numHarmonics; ++i)
    {
        juce::String paramID = "harmonic_gain_" + juce::String (i + 1);
        apvts.addParameterListener (paramID, this);
        
        if (auto* param = apvts.getRawParameterValue (paramID))
            harmonicGains[i].store (param->load());
        else
            harmonicGains[i].store (1.0f);

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
    envelopeLevel = processor.getCurrentEnvelopeLevel();

    updateNodePositions();
    repaint();
}

void OrbitNodes::resized()
{
    auto bounds = getLocalBounds().toFloat();
    centerPoint = bounds.getCentre();
    
    float padding = 45.0f;
    maxOrbitRadius = (std::min (bounds.getWidth(), bounds.getHeight()) * 0.5f) - padding;
    minOrbitRadius = 45.0f;

    updateNodePositions();
}

void OrbitNodes::updateNodePositions()
{
    const float maxPossibleNodeRadius = 24.0f;
    const float innerArenaRadius = maxOrbitRadius - (maxPossibleNodeRadius * 0.5f);

    for (int i = 0; i < numHarmonics; ++i)
    {
        if (nodes[i].isDragging)
            continue;
        
        float gain = harmonicGains[i].load();
        float dist = juce::jmap (gain, 0.0f, 1.0f, minOrbitRadius, innerArenaRadius);

        nodes[i].currentPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * dist,
            std::sin (nodes[i].baseAngle) * dist
        );
    }
}

void OrbitNodes::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (OvertoneStyle::backgroundColourId));

    float qVal = currentQ.load();
    float normQ = juce::jmap (qVal, 5.0f, 150.0f, 1.0f, 0.0f); 

    const float maxPossibleNodeRadius = 24.0f; 
    float innerArenaRadius = maxOrbitRadius - (maxPossibleNodeRadius * 0.6f);

    juce::Path arenaDisc;
    arenaDisc.addEllipse (centerPoint.x - maxOrbitRadius - 5.0f, centerPoint.y - maxOrbitRadius - 5.0f, maxOrbitRadius * 2.0f + 10.0f, maxOrbitRadius * 2.0f + 10.0f);

    g.setColour (getLookAndFeel().findColour (OvertoneStyle::darkBackgroundColourId).darker (2.0f));
    g.fillPath (arenaDisc);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawEllipse (centerPoint.x - maxOrbitRadius - 5.0f, centerPoint.y - maxOrbitRadius - 5.0f, maxOrbitRadius * 2.0f + 10.0f, maxOrbitRadius * 2.0f + 10.0f, 1.5f);
    g.setColour (juce::Colours::white.withAlpha (0.04f));
    g.drawEllipse (centerPoint.x - minOrbitRadius + 12.0f, centerPoint.y - minOrbitRadius + 12.0f, minOrbitRadius * 2.0f - 24.0f, minOrbitRadius * 2.0f - 24.0f, 1.0f);

    for (int i = 0; i < numHarmonics; ++i)
    {
        float gain = harmonicGains[i].load();

        juce::Point<float> minPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * minOrbitRadius, 
            std::sin (nodes[i].baseAngle) * minOrbitRadius
        );
        juce::Point<float> maxPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * innerArenaRadius, 
            std::sin (nodes[i].baseAngle) * innerArenaRadius
        );

        juce::Point<float> pos = minPos + (maxPos - minPos) * gain;

        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.drawLine (minPos.x, minPos.y, maxPos.x, maxPos.y, 1.0f);

        juce::Colour tetherColour (getLookAndFeel().findColour (OvertoneStyle::accentColourId).withAlpha (0.4f));
        g.setColour (tetherColour.withMultipliedAlpha (0.2f + gain * 0.8f));
        g.drawLine (minPos.x, minPos.y, pos.x, pos.y, 1.5f + gain * 1.5f);
        
        float baseRadius = (7.0f + gain * 5.0f) * (1.0f + normQ * 0.35f);
        float glowOffset = 1.0f + 2.0f * (gain + (1.0f - gain) * normQ);
        float maxNodeRadius = baseRadius + glowOffset;
        
        juce::Colour nodeColour (getLookAndFeel().findColour (OvertoneStyle::accentColourId));
        juce::Colour baseColor = nodes[i].isDragging ? juce::Colours::white : nodeColour.withMultipliedAlpha (0.5f + gain * 0.5f);

        juce::Colour centerColor = baseColor;
        juce::Colour edgeColor = baseColor.withAlpha (0.0f);

        float coreStopPos = juce::jmap (normQ, 0.0f, 1.0f, 0.72f, 0.2f);
        float coreOpacity = juce::jmap (normQ, 0.0f, 1.0f, 1.0f, 0.75f);

        juce::ColourGradient grad (centerColor.withAlpha (coreOpacity * 0.5f), pos.x, pos.y, edgeColor, pos.x + maxNodeRadius, pos.y, true);

        grad.addColour (coreStopPos, centerColor.withAlpha (coreOpacity * 0.5f));

        float midStopPos = coreStopPos + (1.0f - coreStopPos) * 0.5f;
        float midOpacity = juce::jmap (normQ, 0.0f, 1.0f, coreOpacity * 0.25f, coreOpacity * 0.15f);
        grad.addColour (midStopPos, centerColor.withAlpha (midOpacity));

        grad.addColour (1.0f, edgeColor);

        g.setGradientFill (grad);
        g.fillEllipse (pos.x - maxNodeRadius, pos.y - maxNodeRadius, maxNodeRadius * 2.0f, maxNodeRadius * 2.0f);

        float innerDiscRadius = 3.5f + gain * 2.0f * (4.0f - normQ * 2.0f);
        g.setColour (baseColor.withAlpha (0.95f));
        g.fillEllipse (pos.x - innerDiscRadius, pos.y - innerDiscRadius, innerDiscRadius * 2.0f, innerDiscRadius * 2.0f);

        float badgeRadius = maxOrbitRadius + 25.0f;
        juce::Point<float> badgePos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * badgeRadius,
            std::sin (nodes[i].baseAngle) * badgeRadius
        );

        bool isActive = nodes[i].isDragging || (gain > 0.05f);
        float badgeSize = isActive ? 22.0f : 20.0f;
        juce::Rectangle<float> badgeBounds (badgePos.x - (badgeSize / 2.0f), badgePos.y - (badgeSize / 2.0f), badgeSize, badgeSize);

        juce::Colour badgeBg = getLookAndFeel().findColour (OvertoneStyle::darkBackgroundColourId).darker (0.5f);
        g.setColour (badgeBg);
        g.fillEllipse (badgeBounds);

        juce::Colour borderCol = isActive ? getLookAndFeel().findColour (OvertoneStyle::pastelToneId).brighter (0.15f) : juce::Colours::white.withAlpha (0.15f);
        g.setColour (borderCol);
        g.drawEllipse (badgeBounds, isActive ? 1.5f : 1.0f);

        juce::Colour textCol = isActive ? (nodes[i].isDragging ? getLookAndFeel().findColour (OvertoneStyle::accentColourId).brighter (0.3f) : getLookAndFeel().findColour (OvertoneStyle::accentColourId)) : juce::Colours::white.withAlpha (0.4f);
        g.setColour (textCol);
        g.setFont (juce::FontOptions (14.0f, isActive ? juce::Font::bold : juce::Font::plain));
        g.drawText (juce::String (i + 1), badgeBounds, juce::Justification::centred, false);
    }

    float coreRadius = juce::jmap (envelopeLevel, 0.0f, 1.0f, 6.0f, 18.0f);
    float glowRadius = coreRadius * 1.8f;
    float glowAlpha = juce::jmap (envelopeLevel, 0.0f, 1.0f, 0.15f, 0.45f);

    juce::ColourGradient coreGlow (coreColour.withAlpha (glowAlpha), centerPoint.x, centerPoint.y, coreColour.withAlpha (0.0f), centerPoint.x + glowRadius, centerPoint.y, true);
    g.setGradientFill (coreGlow);
    g.fillEllipse (centerPoint.x - glowRadius, centerPoint.y - glowRadius, glowRadius * 2.0f, glowRadius * 2.0f);

    juce::Colour dynamicCoreColor = coreColour.interpolatedWith (juce::Colours::white, envelopeLevel * 0.35f);
    g.setColour (dynamicCoreColor);
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
    
    const float maxPossibleNodeRadius = 18.0f;
    const float innerArenaRadius = maxOrbitRadius - (maxPossibleNodeRadius * 0.6f);

    float dist = centerPoint.getDistanceFrom (e.position);

    float newGain = juce::jmap (juce::jlimit (minOrbitRadius, innerArenaRadius, dist), minOrbitRadius, innerArenaRadius, 0.0f, 1.0f);

    float clampedDist = juce::jmap (newGain, 0.0f, 1.0f, minOrbitRadius, innerArenaRadius);
    
    nodes[activeDraggingIndex].currentPos = centerPoint + juce::Point<float> (
        std::cos (nodes[activeDraggingIndex].baseAngle) * clampedDist,
        std::sin (nodes[activeDraggingIndex].baseAngle) * clampedDist
    );

    harmonicGains[activeDraggingIndex].store (newGain);

    juce::String paramID = "harmonic_gain_" + juce::String (activeDraggingIndex + 1);
    if (auto* param = apvts.getParameter (paramID))
        param->setValueNotifyingHost (newGain);

    repaint();
}

void OrbitNodes::mouseUp (const juce::MouseEvent& /* e */)
{
    if (activeDraggingIndex >= 0)
    {
        nodes[activeDraggingIndex].isDragging = false;
        activeDraggingIndex = -1;
    }
}

void OrbitNodes::mouseDoubleClick (const juce::MouseEvent& e)
{
    juce::Point<float> clickPos = e.position;

    float distFromCenter = clickPos.getDistanceFrom (centerPoint);
    float centerHitRadius = 25.0f;

    if (distFromCenter <= centerHitRadius)
    {
        juce::Random rng;
        for (int i = 0; i < numHarmonics; ++i)
        {
            float randomGain = rng.nextFloat();
            if (auto* param = apvts.getParameter ("harmonic_gain_" + juce::String (i + 1)))
                param->setValueNotifyingHost (randomGain);
        }

        repaint();
        return;
    }

    float maxPossibleNodeRadius = 24.0f;
    float innerArenaRadius = maxOrbitRadius - (maxPossibleNodeRadius * 0.6f);

    for (int i = 0; i < numHarmonics; ++i)
    {
        float gain = harmonicGains[i].load();

        juce::Point<float> minPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * minOrbitRadius,
            std::sin (nodes[i].baseAngle) * minOrbitRadius
        );
        juce::Point<float> maxPos = centerPoint + juce::Point<float> (
            std::cos (nodes[i].baseAngle) * innerArenaRadius,
            std::sin (nodes[i].baseAngle) * innerArenaRadius
        );

        juce::Point<float> nodePos = minPos + (maxPos - minPos) * gain;

        if (clickPos.getDistanceFrom (nodePos) <= 18.0f)
        {
            if (auto* param = apvts.getParameter ("harmonic_gain_" + juce::String(i + 1)))
                param->setValueNotifyingHost (1.0f);

            repaint();
            return;
        }
    }
}
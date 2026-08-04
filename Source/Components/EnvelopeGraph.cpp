#include "EnvelopeGraph.h"

EnvelopeGraph::EnvelopeGraph (juce::AudioProcessorValueTreeState& apvtsToUse) : apvts (apvtsToUse)
{
    auto setupSlider = [this] (juce::Slider& slider, juce::Label& label, const juce::String& name)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 14);
        addAndMakeVisible (slider);

        label.setText (name, juce::dontSendNotification);
        label.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.7f));
        addAndMakeVisible (label);
    };

    setupSlider (attackSlider, attackLabel, "A");
    setupSlider (decaySlider, decayLabel, "D");
    setupSlider (sustainSlider, sustainLabel, "S");
    setupSlider (releaseSlider, releaseLabel, "R");

    attackAttach = std::make_unique<SliderAttachment> (apvts, "attack", attackSlider);
    decayAttach = std::make_unique<SliderAttachment> (apvts, "decay", decaySlider);
    sustainAttach = std::make_unique<SliderAttachment> (apvts, "sustain", sustainSlider);
    releaseAttach = std::make_unique<SliderAttachment> (apvts, "release", releaseSlider);

    apvts.addParameterListener ("attack", this);
    apvts.addParameterListener ("decay", this);
    apvts.addParameterListener ("sustain", this);
    apvts.addParameterListener ("release", this);

    attackVal.store (*apvts.getRawParameterValue ("attack"));
    decayVal.store (*apvts.getRawParameterValue ("decay"));
    sustainVal.store (*apvts.getRawParameterValue ("sustain"));
    releaseVal.store (*apvts.getRawParameterValue ("release"));

    startTimerHz (30);
}

EnvelopeGraph::~EnvelopeGraph()
{
    apvts.removeParameterListener ("attack", this);
    apvts.removeParameterListener ("decay", this);
    apvts.removeParameterListener ("sustain", this);
    apvts.removeParameterListener ("release", this);
}

void EnvelopeGraph::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "attack")
        attackVal.store (newValue);
    else if (parameterID == "decay")
        decayVal.store (newValue);
    else if (parameterID == "sustain")
        sustainVal.store (newValue);
    else if (parameterID == "release")
        releaseVal.store (newValue);
}

void EnvelopeGraph::timerCallback()
{
    repaint();
}

void EnvelopeGraph::resized()
{
    auto bounds = getLocalBounds();

    auto knobArea = bounds.removeFromBottom (75);
    int knobWidth = knobArea.getWidth() / 4;

    auto setKnobBounds = [&knobArea, knobWidth] (juce::Slider& slider, juce::Label& label, int /* index */)
    {
        auto area = knobArea.removeFromLeft (knobWidth);
        label.setBounds (area.removeFromTop (16));
        slider.setBounds (area.reduced (2));
    };

    setKnobBounds (attackSlider, attackLabel, 0);
    setKnobBounds (decaySlider, decayLabel, 1);
    setKnobBounds (sustainSlider, sustainLabel, 2);
    setKnobBounds (releaseSlider, releaseLabel, 3);
}

void EnvelopeGraph::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (18, 20, 24));

    auto graphArea = getLocalBounds().removeFromTop (getHeight() - 75).toFloat().reduced (12.0f);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (graphArea, 4.0f, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.04f));
    
    float y100 = graphArea.getY();
    float y50  = graphArea.getY() + graphArea.getHeight() * 0.5f;
    float y0   = graphArea.getBottom();

    g.drawHorizontalLine (static_cast<int>(y50), graphArea.getX(), graphArea.getRight());

    g.setColour (juce::Colours::white.withAlpha (0.3f));
    g.setFont (juce::FontOptions (9.0f));
    g.drawText ("1.0", static_cast<int>(graphArea.getX() + 4), static_cast<int>(y100 + 2), 24, 10, juce::Justification::left);
    g.drawText ("0.5", static_cast<int>(graphArea.getX() + 4), static_cast<int>(y50 - 5), 24, 10, juce::Justification::left);
    g.drawText ("0.0", static_cast<int>(graphArea.getX() + 4), static_cast<int>(y0 - 12), 24, 10, juce::Justification::left);

    float a = attackVal.load();  
    float d = decayVal.load();
    float s = sustainVal.load(); 
    float r = releaseVal.load();

    float totalTime = juce::jmax (0.1f, a + d + 0.8f + r); 
    float width = graphArea.getWidth();
    float height = graphArea.getHeight();

    float p1X = graphArea.getX();
    float p1Y = graphArea.getBottom();

    float p2X = p1X + (a / totalTime) * width;
    float p2Y = graphArea.getY();

    float p3X = p2X + (d / totalTime) * width;
    float p3Y = graphArea.getBottom() - (s * height);

    float p4X = p3X + (0.8f / totalTime) * width;
    float p4Y = p3Y;

    float p5X = p4X + (r / totalTime) * width;
    float p5Y = graphArea.getBottom();

    g.setColour (juce::Colours::white.withAlpha (0.04f));
    g.drawVerticalLine (static_cast<int>(p2X), graphArea.getY(), graphArea.getBottom());
    g.drawVerticalLine (static_cast<int>(p3X), graphArea.getY(), graphArea.getBottom());
    g.drawVerticalLine (static_cast<int>(p4X), graphArea.getY(), graphArea.getBottom());

    juce::Path envPath;
    envPath.startNewSubPath (p1X, p1Y);
    envPath.lineTo (p2X, p2Y);
    envPath.lineTo (p3X, p3Y);
    envPath.lineTo (p4X, p4Y);
    envPath.lineTo (p5X, p5Y);

    juce::Path fillPath = envPath;
    fillPath.lineTo (p5X, graphArea.getBottom());
    fillPath.lineTo (p1X, graphArea.getBottom());
    fillPath.closeSubPath();

    g.setColour (fillColour);
    g.fillPath (fillPath);

    g.setColour (graphColour);
    g.strokePath (envPath, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved));

    auto drawNode = [&g] (float x, float y)
    {
        g.setColour (juce::Colours::white);
        g.fillEllipse (x - 3.5f, y - 3.5f, 7.0f, 7.0f);
        g.setColour (juce::Colour::fromRGB (80, 200, 255));
        g.drawEllipse (x - 5.5f, y - 5.5f, 11.0f, 11.0f, 1.0f);
    };

    drawNode (p2X, p2Y);
    drawNode (p3X, p3Y);
    drawNode (p4X, p4Y);
}
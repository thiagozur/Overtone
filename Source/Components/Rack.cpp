#include "Rack.h"

Rack::Rack (juce::AudioProcessorValueTreeState& apvtsToUse) : apvts (apvtsToUse)
{
    setupRotarySlider (driveSlider, driveLabel, "Drive");
    driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "drive", driveSlider);

    widthSlider.setLookAndFeel (&pctKnobStyle);
    setupRotarySlider (widthSlider, widthLabel, "Wide Detune");
    widthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "stereo_width", widthSlider);

    chorusSlider.setLookAndFeel (&pctKnobStyle);
    setupRotarySlider (chorusSlider, chorusLabel, "Chorus Mix");
    chorusAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "chorus_amount", chorusSlider);

    reverbMixSlider.setLookAndFeel (&pctKnobStyle);
    setupRotarySlider (reverbMixSlider, reverbMixLabel, "Mix");
    reverbMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "reverb_mix", reverbMixSlider);

    reverbSizeSlider.setLookAndFeel (&pctKnobStyle);
    setupRotarySlider (reverbSizeSlider, reverbSizeLabel, "Size");
    reverbSizeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "reverb_size", reverbSizeSlider);

    shimmerAmountSlider.setLookAndFeel (&pctKnobStyle);
    setupRotarySlider (shimmerAmountSlider, shimmerAmountLabel, "Shimmer");
    shimmerAmountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "shimmer_amount", shimmerAmountSlider);
}

Rack::~Rack()
{

}

void Rack::setupRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void Rack::drawRackModule (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title)
{
    auto bounds = area.toFloat();

    g.setColour (getLookAndFeel().findColour (OvertoneStyle::darkBackgroundColourId));
    g.fillRoundedRectangle (bounds, 6.0f);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.fillEllipse (bounds.getX() + 8.0f, bounds.getCentreY() - 4.0f, 8.0f, 8.0f);
    g.fillEllipse (bounds.getRight() - 16.0f, bounds.getCentreY() - 4.0f, 8.0f, 8.0f);

    g.setColour (juce::Colours::white.withAlpha (0.8f));
    g.setFont (OvertoneStyle::getMontserratFont(*this, 15.0f));
    g.drawText (title, static_cast<int>(bounds.getX() + 30.0f), static_cast<int>(bounds.getY() + 10.0f), 200, 20, juce::Justification::left);

    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.drawHorizontalLine (static_cast<int>(bounds.getY() + 32.0f), bounds.getX() + 30.0f, bounds.getRight() - 30.0f);
}

void Rack::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (OvertoneStyle::backgroundColourId));

    auto bounds = getLocalBounds().reduced (10);
    int gap = 8;
    int moduleHeight = (bounds.getHeight() - (gap * 2)) / 3;

    auto driveArea = bounds.removeFromTop (moduleHeight);
    drawRackModule (g, driveArea, "01. SOFT CLIPPING");

    bounds.removeFromTop (gap);

    auto widthArea = bounds.removeFromTop (moduleHeight);
    drawRackModule (g, widthArea, "02. MODULACIÓN");

    bounds.removeFromTop (gap);

    auto reverbArea = bounds;
    drawRackModule (g, reverbArea, "03. SHIMMER REVERB");
}

void Rack::resized()
{
    auto bounds = getLocalBounds().reduced (10);
    int gap = 8;
    int moduleHeight = (bounds.getHeight() - (gap * 2)) / 3;

    int knobWidth = 80;
    int knobHeight = 80;
    int labelHeight = 16;
    int contentYOffset = 38;
    int startX = bounds.getX() + 40;
    int spacing = 110;

    auto driveArea = bounds.removeFromTop (moduleHeight);
    driveLabel.setBounds (startX, driveArea.getY() + contentYOffset, knobWidth, labelHeight);
    driveSlider.setBounds (startX, driveArea.getY() + contentYOffset + labelHeight, knobWidth, knobHeight);

    bounds.removeFromTop (gap);

    auto modArea = bounds.removeFromTop (moduleHeight);
    widthLabel.setBounds (startX, modArea.getY() + contentYOffset, knobWidth, labelHeight);
    widthSlider.setBounds (startX, modArea.getY() + contentYOffset + labelHeight, knobWidth, knobHeight);

    chorusLabel.setBounds (startX + spacing, modArea.getY() + contentYOffset, knobWidth, labelHeight);
    chorusSlider.setBounds (startX + spacing, modArea.getY() + contentYOffset + labelHeight, knobWidth, knobHeight);

    bounds.removeFromTop (gap);

    auto reverbArea = bounds;
    reverbMixLabel.setBounds (startX, reverbArea.getY() + contentYOffset, knobWidth, labelHeight);
    reverbMixSlider.setBounds (startX, reverbArea.getY() + contentYOffset + labelHeight, knobWidth, knobHeight);

    shimmerAmountLabel.setBounds (startX + spacing, reverbArea.getY() + contentYOffset, knobWidth, labelHeight);
    shimmerAmountSlider.setBounds (startX + spacing, reverbArea.getY() + contentYOffset + labelHeight, knobWidth, knobHeight);

    reverbSizeLabel.setBounds (startX + (spacing * 2), reverbArea.getY() + contentYOffset, knobWidth, labelHeight);
    reverbSizeSlider.setBounds (startX + (spacing * 2), reverbArea.getY() + contentYOffset + labelHeight, knobWidth, knobHeight);
}
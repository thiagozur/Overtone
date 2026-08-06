#include "NoiseSource.h"

NoiseSource::NoiseSource (juce::AudioProcessorValueTreeState& apvtsToUse) : apvts (apvtsToUse)
{
    noiseSelector.addItem ("Bosque", 1);
    noiseSelector.addItem ("Lluvia", 2);
    noiseSelector.addItem ("Arroyo", 3);
    noiseSelector.addItem ("VHS", 4);
    noiseSelector.addItem ("Vinilo", 5);
    noiseSelector.addItem ("Ruido Importado", 6);
    addAndMakeVisible (noiseSelector);

    selectorAttach = std::make_unique<ComboAttachment> (apvts, "noise_source", noiseSelector);

    gainAttach = std::make_unique<SliderAttachment> (apvts, "master_gain", noiseGainSlider);

    noiseGainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    noiseGainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (noiseGainSlider);

    noiseGainLabel.setText ("GAIN", juce::dontSendNotification);
    noiseGainLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    noiseGainLabel.setJustificationType (juce::Justification::centred);
    noiseGainLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.7f));
    addAndMakeVisible (noiseGainLabel);

    qAttach = std::make_unique<SliderAttachment> (apvts, "resonance_q", qSlider);

    qSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    qSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (qSlider);

    qLabel.setText ("Q", juce::dontSendNotification);
    qLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    qLabel.setJustificationType (juce::Justification::centred);
    qLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.7f));
    addAndMakeVisible (qLabel);    
}

NoiseSource::~NoiseSource()
{

}

void NoiseSource::resized()
{
    auto bounds = getLocalBounds().reduced (8);

    auto dropArea = bounds.removeFromTop (bounds.getHeight() / 3);
    noiseSelector.setBounds (dropArea.removeFromTop (36));

    auto knobArea = bounds;
    int knobWidth = knobArea.getWidth() / 2;

    auto qArea = knobArea.removeFromLeft (knobWidth);
    qLabel.setBounds (qArea.removeFromTop (16));
    qSlider.setBounds (qArea.reduced (2));

    auto gainArea = knobArea;
    noiseGainLabel.setBounds (gainArea.removeFromTop (16));
    noiseGainSlider.setBounds (gainArea.reduced (2));
}

void NoiseSource::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (getLookAndFeel().findColour (OvertoneStyle::darkBackgroundColourId));
    g.fillRoundedRectangle (bounds, 4.0f);

    g.setColour (isDraggingFileOver ? juce::Colour::fromRGB (80, 200, 255) : juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (bounds, 4.0f, isDraggingFileOver ? 2.0f : 1.0f);

    if (isDraggingFileOver)
    {
        g.setColour (juce::Colour::fromRGBA (80, 200, 255, 30));
        g.fillRoundedRectangle (bounds, 4.0f);
        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText ("Soltar WAV aquí", getLocalBounds(), juce::Justification::centred);
    }
}

bool NoiseSource::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.endsWithIgnoreCase (".wav"))
            return true;
    }
    return false;
}

void NoiseSource::fileDragMove (const juce::StringArray& /* files */, int /* x */, int /* y */)
{
    if (! isDraggingFileOver)
    {
        isDraggingFileOver = true;
        repaint();
    }
}

void NoiseSource::fileDragExit (const juce::StringArray& /* files */)
{
    isDraggingFileOver = false;
    repaint();
}

void NoiseSource::filesDropped (const juce::StringArray& files, int /* x */, int /* y */)
{
    isDraggingFileOver = false;
    repaint();

    for (const auto& filePath : files)
    {
        if (filePath.endsWithIgnoreCase (".wav"))
        {
            juce::File customFile (filePath);
            noiseSelector.setSelectedId (6, juce::sendNotificationSync);
            break;
        }
    }
}
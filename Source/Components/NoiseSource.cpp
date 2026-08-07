#include "NoiseSource.h"

NoiseSource::NoiseSource (OvertoneAudioProcessor& p, juce::AudioProcessorValueTreeState& apvtsToUse) : processor (p), apvts (apvtsToUse)
{
    noiseSelector.addItem ("Bosque", 1);
    noiseSelector.addItem ("Lluvia", 2);
    noiseSelector.addItem ("Arroyo", 3);
    noiseSelector.addItem ("Fuego", 4);
    noiseSelector.addItem ("Olas", 5);
    noiseSelector.addItem ("Proyector", 6);
    noiseSelector.addItem ("VHS", 7);
    noiseSelector.addItem ("Vinilo", 8);
    noiseSelector.addItem ("Ruido Blanco", 9);
    noiseSelector.addItem ("Ruido Browniano", 10);
    noiseSelector.addItem ("Ruido Importado", 11);
    addAndMakeVisible (noiseSelector);

    selectorAttach = std::make_unique<ComboAttachment> (apvts, "noise_source", noiseSelector);

    importNoiseButton.onClick = [this] { importButtonClicked(); };
    addAndMakeVisible (importNoiseButton);

    gainAttach = std::make_unique<SliderAttachment> (apvts, "master_gain", noiseGainSlider);

    noiseGainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    noiseGainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (noiseGainSlider);

    noiseGainLabel.setText ("Gain", juce::dontSendNotification);
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

    directNoiseSlider.setLookAndFeel (&pctKnobStyle);
    directNoiseAttach = std::make_unique<SliderAttachment> (apvts, "direct_noise", directNoiseSlider);

    directNoiseSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    directNoiseSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (directNoiseSlider);

    directNoiseLabel.setText ("Ruido Original", juce::dontSendNotification);
    directNoiseLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    directNoiseLabel.setJustificationType (juce::Justification::centred);
    directNoiseLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.7f));
    addAndMakeVisible (directNoiseLabel);
}

NoiseSource::~NoiseSource()
{

}

void NoiseSource::importButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Seleccionar archivo de ruido WAV",
        juce::File::getSpecialLocation (juce::File::userHomeDirectory),
        "*.wav"
    );

    auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync (fileChooserFlags, [this] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file.existsAsFile())
        {
            if (processor.loadCustomNoiseFile (file))
                noiseSelector.setSelectedId (11, juce::sendNotificationSync);
        }
    });
}

void NoiseSource::resized()
{
    auto bounds = getLocalBounds().reduced (8);

    auto selectionArea = bounds.removeFromTop (bounds.getHeight() / 3);
    auto selectionControlsArea = selectionArea.removeFromTop (35);
    auto noiseSelectorArea = selectionControlsArea.removeFromLeft (selectionControlsArea.getWidth() / 2 - 5);
    noiseSelector.setBounds (noiseSelectorArea);
    selectionControlsArea.removeFromLeft (10);
    importNoiseButton.setBounds (selectionControlsArea);

    auto knobArea = bounds;
    int knobWidth = knobArea.getWidth() / 3;

    auto qArea = knobArea.removeFromLeft (knobWidth);
    qLabel.setBounds (qArea.removeFromTop (16));
    qSlider.setBounds (qArea.reduced (2));

    auto directNoiseArea = knobArea.removeFromLeft (knobWidth);
    directNoiseLabel.setBounds (directNoiseArea.removeFromTop (16));
    directNoiseSlider.setBounds (directNoiseArea.reduced (2));

    auto gainArea = knobArea;
    noiseGainLabel.setBounds (gainArea.removeFromTop (16));
    noiseGainSlider.setBounds (gainArea.reduced (2));
}

void NoiseSource::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (getLookAndFeel().findColour (OvertoneStyle::darkBackgroundColourId));
    g.fillRoundedRectangle (bounds, 4.0f);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
}


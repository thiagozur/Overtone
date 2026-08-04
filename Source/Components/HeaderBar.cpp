#include "HeaderBar.h"

HeaderBar::HeaderBar (juce::AudioProcessorValueTreeState& apvtsToUse, PresetManager& presetManagerToUse) : apvts (apvtsToUse), presetManager (presetManagerToUse)
{
    addAndMakeVisible (synthPageButton);
    addAndMakeVisible (fxPageButton);

    synthPageButton.onClick = [this]
    {
        currentPageIndex = 0;
        updateButtonStyles();
        if (onPageSwitched) onPageSwitched (0);
    };

    fxPageButton.onClick = [this]
    {
        currentPageIndex = 1;
        updateButtonStyles();
        if (onPageSwitched) onPageSwitched (1);
    };

    updateButtonStyles();

    addAndMakeVisible (presetSelector);
    presetSelector.onChange = [this]
    {
        int selectedId = presetSelector.getSelectedId();
        if (selectedId > 0)
            presetManager.loadPreset (selectedId - 1);
    };

    addAndMakeVisible (savePresetButton);
    savePresetButton.onClick = [this]
    {
        auto textWindow = std::make_unique<juce::AlertWindow> ("Guardar Preset", "Introducir nombre:", juce::MessageBoxIconType::NoIcon, this);
        textWindow->addTextEditor ("presetName", "Mi preset", "Nombre:");
        textWindow->addButton ("Guardar", 1, juce::KeyPress (juce::KeyPress::returnKey));
        textWindow->addButton ("Cancelar", 0, juce::KeyPress (juce::KeyPress::escapeKey));

        cwRelease = std::move (textWindow);

        cwRelease->enterModalState (true, juce::ModalCallbackFunction::create ([this] (int result)
        {
            auto window = std::move (cwRelease);

            if (window != nullptr && result == 1)
            {
                if (auto* nameEditor = window->getTextEditor ("presetName"))
                {
                    juce::String presetName = nameEditor->getText().trim();
                    
                    if (presetName.isNotEmpty())
                    {
                        presetManager.saveUserPreset (presetName);
                        updatePresetList();

                        const auto& presets = presetManager.getAllPresets();
                        for (size_t i = 0; i < presets.size(); ++i)
                        {
                            if (presets[i].name == presetName && !presets[i].isFactory)
                            {
                                presetSelector.setSelectedId (static_cast<int>(i + 1), juce::dontSendNotification);
                                break;
                            }
                        }
                    }
                }
            }
            else
                updatePresetList();
        }));
    };

    addAndMakeVisible (deletePresetButton);
    deletePresetButton.onClick = [this]
    {
        int currentIdx = presetManager.getCurrentPresetIndex();
        const auto& presets = presetManager.getAllPresets();

        if (currentIdx >= 0 && currentIdx < static_cast<int>(presets.size()))
        {
            if (!presets[currentIdx].isFactory)
            {
                juce::String deletedPresetName = presets[currentIdx].name;
                juce::String message = "¿Estás seguro de que deseas eliminar " + deletedPresetName + " permanentemente?";

                juce::AlertWindow::showOkCancelBox (
                    juce::MessageBoxIconType::WarningIcon,
                    "¿Borrar preset?",
                    message,
                    "Sí, borrar",
                    "Cancelar",
                    this,
                    juce::ModalCallbackFunction::create ([this, deletedPresetName] (int result) 
                    {
                        if (result == 1) 
                        {   
                            presetManager.deleteUserPreset (deletedPresetName);
                            updatePresetList();
                            presetSelector.setSelectedId (1, juce::sendNotificationAsync);
                        }
                        else
                            updatePresetList();
                    })
                );
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync (
                    juce::MessageBoxIconType::WarningIcon,
                    "No se puede borrar",
                    "Los presets de fábrica no pueden ser borrados.",
                    "OK",
                    this
                );
            }
        }
    };

    updatePresetList();
}

HeaderBar::~HeaderBar()
{

}

void HeaderBar::updatePresetList()
{
    presetSelector.clear (juce::dontSendNotification);

    const auto& presets = presetManager.getAllPresets();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        juce::String displayName = presets[i].name;
        presetSelector.addItem (displayName, static_cast<int>(i + 1));
    }

    presetManager.restoreCurrentPresetIndexFromState();

    int currentIdx = presetManager.getCurrentPresetIndex();
    if (currentIdx >= 0 && currentIdx < static_cast<int>(presets.size()))
        presetSelector.setSelectedId (currentIdx + 1, juce::dontSendNotification);
}

void HeaderBar::updateButtonStyles()
{
    const juce::Colour activeCol = juce::Colour::fromRGB (80, 200, 255);
    const juce::Colour inactiveCol = juce::Colour::fromRGB (30, 34, 42);

    synthPageButton.setColour (juce::TextButton::buttonColourId, currentPageIndex == 0 ? activeCol : inactiveCol);
    synthPageButton.setColour (juce::TextButton::textColourOffId, currentPageIndex == 0 ? juce::Colours::black : juce::Colours::white);

    fxPageButton.setColour (juce::TextButton::buttonColourId, currentPageIndex == 1 ? activeCol : inactiveCol);
    fxPageButton.setColour (juce::TextButton::textColourOffId, currentPageIndex == 1 ? juce::Colours::black : juce::Colours::white);
}

void HeaderBar::resized()
{
    auto bounds = getLocalBounds().reduced (6, 4);

    bounds.removeFromLeft (130);

    auto tabArea = bounds.removeFromLeft (180).reduced (0, 2);
    int tabWidth = tabArea.getWidth() / 2;
    synthPageButton.setBounds (tabArea.removeFromLeft (tabWidth).reduced (2, 0));
    fxPageButton.setBounds (tabArea.reduced (2, 0));

    auto presetArea = bounds.removeFromRight (280);
    deletePresetButton.setBounds (presetArea.removeFromRight (45).reduced (2, 2));
    savePresetButton.setBounds (presetArea.removeFromRight (55).reduced (2, 2));
    presetSelector.setBounds (presetArea.reduced (4, 2));
}

void HeaderBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (24, 27, 34));

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawHorizontalLine (getHeight() - 1, 0.0f, static_cast<float> (getWidth()));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    g.drawText ("OVERTONE", 10, 0, 120, getHeight(), juce::Justification::centredLeft);
}
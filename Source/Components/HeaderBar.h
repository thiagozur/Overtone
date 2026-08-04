#pragma once

#include <JuceHeader.h>
#include "../Tools/PresetManager.h"

class HeaderBar : public juce::Component
{
public:
    HeaderBar (juce::AudioProcessorValueTreeState& apvtsToUse, PresetManager& presetManagerToUse);
    ~HeaderBar() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void updatePresetList();

    std::function<void(int)> onPageSwitched;
    int getCurrentPageIndex() const { return currentPageIndex; }

private:
    juce::AudioProcessorValueTreeState& apvts;
    PresetManager& presetManager;

    juce::TextButton synthPageButton { "RUIDO" };
    juce::TextButton fxPageButton { "FX" };

    juce::ComboBox presetSelector;
    juce::TextButton savePresetButton { "GUARDAR" };
    juce::TextButton deletePresetButton { "BORRAR" };

    std::unique_ptr<juce::AlertWindow> cwRelease;

    int currentPageIndex = 0;

    void updateButtonStyles();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderBar)
};
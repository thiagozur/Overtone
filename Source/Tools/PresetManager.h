#pragma once

#include <JuceHeader.h>

struct Preset
{
    juce::String name;
    juce::ValueTree state;
    bool isFactory = false;
};

class PresetManager
{
public:
    PresetManager (juce::AudioProcessorValueTreeState& apvtsToUse);

    void saveUserPreset (const juce::String& presetName);
    void deleteUserPreset (const juce::String& presetName);
    void loadPreset (int index);

    const std::vector<Preset>& getAllPresets() const { return presets; }
    void restoreCurrentPresetIndexFromState();
    int getCurrentPresetIndex() const { return currentPresetIndex; }

    void updatePresetList();

private:
    void loadFactoryPresets();
    juce::File getUserPresetDirectory() const;

    juce::AudioProcessorValueTreeState& apvts;
    std::vector<Preset> presets;
    int currentPresetIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
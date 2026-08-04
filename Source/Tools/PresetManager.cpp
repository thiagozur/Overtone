#include "PresetManager.h"

PresetManager::PresetManager (juce::AudioProcessorValueTreeState& apvtsToUse) : apvts (apvtsToUse)
{
    updatePresetList();
}

juce::File PresetManager::getUserPresetDirectory() const
{
    juce::File dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile ("Overtone").getChildFile ("Presets");
    
    if (!dir.exists())
        dir.createDirectory();

    return dir;
}

void PresetManager::loadFactoryPresets()
{
    Preset initPreset;
    initPreset.name = "Default";
    initPreset.state = apvts.copyState();
    initPreset.isFactory = true;
    presets.push_back (initPreset);

    Preset forestPad;
    forestPad.name = "Forest Resonances";
    forestPad.isFactory = true;

    juce::ValueTree vt = apvts.copyState();
    vt.setProperty ("noise_source", 0, nullptr);
    vt.setProperty ("noise_source", 0, nullptr);
    vt.setProperty ("resonance_q", 80.0f, nullptr);
    vt.setProperty ("reverb_mix", 0.5f, nullptr);
    vt.setProperty ("shimmer_amount", 0.4f, nullptr);
    forestPad.state = vt;

    presets.push_back (forestPad);
}

void PresetManager::updatePresetList()
{
    presets.clear();

    loadFactoryPresets();

    juce::File userDir = getUserPresetDirectory();
    juce::Array<juce::File> presetFiles = userDir.findChildFiles (juce::File::TypesOfFileToFind::findFiles, false, "*.xml");

    for (const auto& file : presetFiles)
    {
        std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse (file);
        if (xml != nullptr)
        {
            Preset userPreset;
            userPreset.name = file.getFileNameWithoutExtension();
            userPreset.state = juce::ValueTree::fromXml (*xml);
            userPreset.isFactory = false;
            presets.push_back (userPreset);
        }
    }
}

void PresetManager::loadPreset (int index)
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
    {
        currentPresetIndex = index;
        apvts.replaceState (presets[index].state);
    }
}

void PresetManager::saveUserPreset (const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    juce::File targetFile = getUserPresetDirectory().getChildFile (presetName + ".xml");

    auto currentState = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (currentState.createXml());

    if (xml != nullptr)
    {
        xml->writeTo (targetFile);
        updatePresetList();

        for (size_t i = 0; i < presets.size(); ++i)
        {
            if (presets[i].name == presetName && !presets[i].isFactory)
            {
                currentPresetIndex = static_cast<int>(i);
                break;
            }
        }
    }
}

void PresetManager::deleteUserPreset (const juce::String& presetName)
{
    for (const auto& preset : presets)
    {
        if (preset.name == presetName)
        {
            if (preset.isFactory)
                return;
            
            juce::File fileToDelete = getUserPresetDirectory().getChildFile (presetName + ".xml");
            if (fileToDelete.existsAsFile())
            {
                fileToDelete.deleteFile();
                updatePresetList();
                loadPreset (0);
            }

            break;
        }
    }
}
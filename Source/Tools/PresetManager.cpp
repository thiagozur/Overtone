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
    auto setParamValue = [] (juce::ValueTree& tree, const juce::String& paramID, float value)
    {
        auto child = tree.getChildWithProperty ("id", paramID);
        if (child.isValid())
            child.setProperty ("value", value, nullptr);
        else
        {
            juce::ValueTree paramNode ("PARAM");
            paramNode.setProperty ("id", paramID, nullptr);
            paramNode.setProperty ("value", value, nullptr);
            tree.addChild (paramNode, -1, nullptr);
        }
    };

    Preset initPreset;
    initPreset.name = "Default";
    initPreset.state = apvts.copyState().createCopy();
    initPreset.isFactory = true;
    presets.push_back (initPreset);

    Preset riverPadSoft;
    riverPadSoft.name = "Arroyo Pad Soft";
    riverPadSoft.isFactory = true;

    /* Preset forestPad;
    forestPad.name = "Forest Pad";
    forestPad.isFactory = true;

    Preset forestPad;
    forestPad.name = "Forest Pad";
    forestPad.isFactory = true;

    Preset forestPad;
    forestPad.name = "Forest Pad";
    forestPad.isFactory = true;

    Preset forestPad;
    forestPad.name = "Forest Pad";
    forestPad.isFactory = true;

    Preset forestPad;
    forestPad.name = "Forest Pad";
    forestPad.isFactory = true; */

    juce::ValueTree vt1 = apvts.copyState().createCopy();
    juce::ValueTree vt2 = apvts.copyState().createCopy();
    juce::ValueTree vt3 = apvts.copyState().createCopy();
    juce::ValueTree vt4 = apvts.copyState().createCopy();
    juce::ValueTree vt5 = apvts.copyState().createCopy();
    juce::ValueTree vt6 = apvts.copyState().createCopy();

    setParamValue (vt1, "noise_source", 2.0f);
    setParamValue (vt1, "harmonic_gain_1", 1.0f);
    setParamValue (vt1, "harmonic_gain_2", 0.4f);
    setParamValue (vt1, "harmonic_gain_3", 0.7f);
    setParamValue (vt1, "harmonic_gain_4", 0.95f);
    setParamValue (vt1, "harmonic_gain_5", 0.5f);
    setParamValue (vt1, "harmonic_gain_6", 0.4f);
    setParamValue (vt1, "harmonic_gain_7", 0.5f);
    setParamValue (vt1, "harmonic_gain_8", 0.2f);
    setParamValue (vt1, "direct_noise", 0.05f);
    setParamValue (vt1, "resonance_q", 150.0f);
    setParamValue (vt1, "attack", 1.5f);
    setParamValue (vt1, "sustain", 1.0f);
    setParamValue (vt1, "release", 0.75f);
    setParamValue (vt1, "stereo_width", 0.002f);
    setParamValue (vt1, "reverb_mix", 0.45f);
    setParamValue (vt1, "shimmer_amount", 0.25f);
    setParamValue (vt1, "reverb_size", 0.75f);
    riverPadSoft.state = vt1;
    presets.push_back (riverPadSoft);
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
        apvts.replaceState (presets[index].state.createCopy());

        apvts.state.setProperty ("currentPresetIndex", currentPresetIndex, nullptr);
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

void PresetManager::restoreCurrentPresetIndexFromState()
{
    if (apvts.state.hasProperty ("currentPresetIndex"))
        currentPresetIndex = apvts.state.getProperty ("currentPresetIndex");
    else
        currentPresetIndex = 0;
}
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

    auto createBaseState = []()
    {
        return juce::ValueTree ("Parameters");
    };

    Preset initPreset;
    initPreset.name = "Default";
    initPreset.state = createBaseState();
    initPreset.isFactory = true;
    presets.push_back (initPreset);

    Preset riverPadSoft;
    riverPadSoft.name = "Arroyo Pad Soft";
    riverPadSoft.isFactory = true;

    juce::ValueTree vt1 = createBaseState();
    setParamValue (vt1, "noise_source", 2.0f);
    setParamValue (vt1, "harmonic_gain_1", 1.0f);
    setParamValue (vt1, "harmonic_gain_2", 0.4f);
    setParamValue (vt1, "harmonic_gain_3", 0.7f);
    setParamValue (vt1, "harmonic_gain_4", 1.0f);
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

    Preset storm;
    storm.name = "Tormenta";
    storm.isFactory = true;

    juce::ValueTree vt2 = createBaseState();
    setParamValue (vt2, "noise_source", 1.0f);
    setParamValue (vt2, "harmonic_gain_1", 1.0f);
    setParamValue (vt2, "harmonic_gain_2", 0.2f);
    setParamValue (vt2, "harmonic_gain_3", 0.4f);
    setParamValue (vt2, "harmonic_gain_4", 0.0f);
    setParamValue (vt2, "harmonic_gain_5", 0.0f);
    setParamValue (vt2, "harmonic_gain_6", 0.0f);
    setParamValue (vt2, "harmonic_gain_7", 0.0f);
    setParamValue (vt2, "harmonic_gain_8", 0.0f);
    setParamValue (vt2, "resonance_q", 50.0f);
    setParamValue (vt2, "direct_noise", 0.1f);
    setParamValue (vt2, "master_gain", -7.5f);
    setParamValue (vt2, "attack", 0.001f);
    setParamValue (vt2, "decay", 0.2f);
    setParamValue (vt2, "sustain", 0.7f);
    setParamValue (vt2, "release", 0.2f);
    setParamValue (vt2, "drive", 10.0f);
    setParamValue (vt2, "stereo_width", 0.007f);
    setParamValue (vt2, "chorus_amount", 0.0f);
    setParamValue (vt2, "reverb_mix", 0.4f);
    setParamValue (vt2, "shimmer_amount", 0.0f);
    setParamValue (vt2, "reverb_size", 0.90f);

    storm.state = vt2;
    presets.push_back (storm);
    
    Preset vinylPluck;
    vinylPluck.name = "Pluck Vinilo";
    vinylPluck.isFactory = true;

    juce::ValueTree vt3 = createBaseState();
    setParamValue (vt3, "noise_source", 7.0f);
    setParamValue (vt3, "harmonic_gain_1", 1.0f);
    setParamValue (vt3, "harmonic_gain_2", 0.4f);
    setParamValue (vt3, "harmonic_gain_3", 0.7f);
    setParamValue (vt3, "harmonic_gain_4", 1.0f);
    setParamValue (vt3, "harmonic_gain_5", 0.5f);
    setParamValue (vt3, "harmonic_gain_6", 0.4f);
    setParamValue (vt3, "harmonic_gain_7", 0.5f);
    setParamValue (vt3, "harmonic_gain_8", 0.2f);
    setParamValue (vt3, "resonance_q", 150.0f);
    setParamValue (vt3, "direct_noise", 0.05f);
    setParamValue (vt3, "master_gain", 7.0f);
    setParamValue (vt3, "attack", 0.001f);
    setParamValue (vt3, "decay", 0.3f);
    setParamValue (vt3, "sustain", 0.17f);
    setParamValue (vt3, "release", 0.1f);
    setParamValue (vt3, "drive", 10.0f);
    setParamValue (vt3, "stereo_width", 0.005f);
    setParamValue (vt3, "chorus_amount", 0.45f);
    setParamValue (vt3, "reverb_mix", 0.4f);
    setParamValue (vt3, "shimmer_amount", 0.0f);
    setParamValue (vt3, "reverb_size", 0.15f);

    vinylPluck.state = vt3;
    presets.push_back (vinylPluck);
    
    Preset candle;
    candle.name = "Vela";
    candle.isFactory = true;

    juce::ValueTree vt4 = createBaseState();
    setParamValue (vt4, "noise_source", 3.0f);
    setParamValue (vt4, "harmonic_gain_1", 0.4f);
    setParamValue (vt4, "harmonic_gain_2", 0.2f);
    setParamValue (vt4, "harmonic_gain_3", 0.7f);
    setParamValue (vt4, "harmonic_gain_4", 0.7f);
    setParamValue (vt4, "harmonic_gain_5", 0.0f);
    setParamValue (vt4, "harmonic_gain_6", 0.7f);
    setParamValue (vt4, "harmonic_gain_7", 0.0f);
    setParamValue (vt4, "harmonic_gain_8", 0.3f);
    setParamValue (vt4, "resonance_q", 60.0f);
    setParamValue (vt4, "direct_noise", 0.0f);
    setParamValue (vt4, "master_gain", 0.0f);
    setParamValue (vt4, "attack", 0.15f);
    setParamValue (vt4, "decay", 0.3f);
    setParamValue (vt4, "sustain", 0.95f);
    setParamValue (vt4, "release", 0.6f);
    setParamValue (vt4, "drive", 1.0f);
    setParamValue (vt4, "stereo_width", 0.0f);
    setParamValue (vt4, "chorus_amount", 0.0f);
    setParamValue (vt4, "reverb_mix", 0.3f);
    setParamValue (vt4, "shimmer_amount", 0.0f);
    setParamValue (vt4, "reverb_size", 0.6f);

    candle.state = vt4;
    presets.push_back (candle);
    
    Preset brokenProjector;
    brokenProjector.name = "Proyector Roto";
    brokenProjector.isFactory = true;

    juce::ValueTree vt5 = createBaseState();
    setParamValue (vt5, "noise_source", 5.0f);
    setParamValue (vt5, "harmonic_gain_1", 1.0f);
    setParamValue (vt5, "harmonic_gain_2", 0.0f);
    setParamValue (vt5, "harmonic_gain_3", 0.2f);
    setParamValue (vt5, "harmonic_gain_4", 0.1f);
    setParamValue (vt5, "harmonic_gain_5", 0.0f);
    setParamValue (vt5, "harmonic_gain_6", 0.0f);
    setParamValue (vt5, "harmonic_gain_7", 0.0f);
    setParamValue (vt5, "harmonic_gain_8", 0.0f);
    setParamValue (vt5, "resonance_q", 35.0f);
    setParamValue (vt5, "direct_noise", 0.0f);
    setParamValue (vt5, "master_gain", 0.0f);
    setParamValue (vt5, "attack", 0.02f);
    setParamValue (vt5, "decay", 0.3f);
    setParamValue (vt5, "sustain", 1.0f);
    setParamValue (vt5, "release", 0.5f);
    setParamValue (vt5, "drive", 1.6f);
    setParamValue (vt5, "stereo_width", 0.0f);
    setParamValue (vt5, "chorus_amount", 0.25f);
    setParamValue (vt5, "reverb_mix", 0.18f);
    setParamValue (vt5, "shimmer_amount", 0.75f);
    setParamValue (vt5, "reverb_size", 0.1f);

    brokenProjector.state = vt5;
    presets.push_back (brokenProjector);
    
    Preset VHSSine;
    VHSSine.name = "VHS Sine";
    VHSSine.isFactory = true;

    juce::ValueTree vt6 = createBaseState();
    setParamValue (vt6, "noise_source", 6.0f);
    setParamValue (vt6, "harmonic_gain_1", 1.0f);
    setParamValue (vt6, "harmonic_gain_2", 0.0f);
    setParamValue (vt6, "harmonic_gain_3", 0.0f);
    setParamValue (vt6, "harmonic_gain_4", 0.0f);
    setParamValue (vt6, "harmonic_gain_5", 0.0f);
    setParamValue (vt6, "harmonic_gain_6", 0.0f);
    setParamValue (vt6, "harmonic_gain_7", 0.0f);
    setParamValue (vt6, "harmonic_gain_8", 0.0f);
    setParamValue (vt6, "resonance_q", 150.0f);
    setParamValue (vt6, "direct_noise", 0.0f);
    setParamValue (vt6, "master_gain", 5.0f);
    setParamValue (vt6, "attack", 0.05f);
    setParamValue (vt6, "decay", 0.3f);
    setParamValue (vt6, "sustain", 1.0f);
    setParamValue (vt6, "release", 0.1f);
    setParamValue (vt6, "drive", 5.0f);
    setParamValue (vt6, "stereo_width", 0.0f);
    setParamValue (vt6, "chorus_amount", 0.0f);
    setParamValue (vt6, "reverb_mix", 0.0f);
    setParamValue (vt6, "shimmer_amount", 0.0f);
    setParamValue (vt6, "reverb_size", 0.5f);

    VHSSine.state = vt6;
    presets.push_back (VHSSine);
    
    Preset oceanPad;
    oceanPad.name = "Pad Océano";
    oceanPad.isFactory = true;

    juce::ValueTree vt7 = createBaseState();
    setParamValue (vt7, "noise_source", 4.0f);
    setParamValue (vt7, "harmonic_gain_1", 1.0f);
    setParamValue (vt7, "harmonic_gain_2", 0.9f);
    setParamValue (vt7, "harmonic_gain_3", 0.8f);
    setParamValue (vt7, "harmonic_gain_4", 0.6f);
    setParamValue (vt7, "harmonic_gain_5", 0.5f);
    setParamValue (vt7, "harmonic_gain_6", 0.4f);
    setParamValue (vt7, "harmonic_gain_7", 0.3f);
    setParamValue (vt7, "harmonic_gain_8", 0.1f);
    setParamValue (vt7, "resonance_q", 110.0f);
    setParamValue (vt7, "direct_noise", 0.35f);
    setParamValue (vt7, "master_gain", 0.0f);
    setParamValue (vt7, "attack", 2.0f);
    setParamValue (vt7, "decay", 0.95f);
    setParamValue (vt7, "sustain", 0.8f);
    setParamValue (vt7, "release", 1.3f);
    setParamValue (vt7, "drive", 0.0f);
    setParamValue (vt7, "stereo_width", 0.012f);
    setParamValue (vt7, "chorus_amount", 0.0f);
    setParamValue (vt7, "reverb_mix", 0.45f);
    setParamValue (vt7, "shimmer_amount", 0.25f);
    setParamValue (vt7, "reverb_size", 0.7f);

    oceanPad.state = vt7;
    presets.push_back (oceanPad);
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
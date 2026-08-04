cmake_minimum_required(VERSION 3.22)

project(Overtone VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(juce)

juce_add_plugin(Overtone
    COMPANY_NAME "thiagozur"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Tzur
    PLUGIN_CODE Ovtn
    FORMATS VST3 Standalone
    PRODUCT_NAME "Overtone"
)

target_compile_definitions(Overtone
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_sources(Overtone PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/Tools/SampleLoopPlayer.h
    Source/Tools/PresetManager.h
    Source/Tools/PresetManager.cpp
    Source/Components/OrbitNodes.h
    Source/Components/OrbitNodes.cpp
    Source/Components/EnvelopeGraph.h
    Source/Components/EnvelopeGraph.cpp
    Source/Components/NoiseSource.h
    Source/Components/NoiseSource.cpp
)

juce_add_binary_data(FactoryAudioData
    SOURCES
        Resources/Noise/birds.wav
        Resources/Noise/rain.wav
        Resources/Noise/river.wav
        Resources/Noise/vhs.wav
        Resources/Noise/vinyl.wav
)

target_link_libraries(Overtone
    PRIVATE
        FactoryAudioData
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_audio_formats
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(Overtone)
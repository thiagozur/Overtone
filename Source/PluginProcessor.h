#pragma once

#include <JuceHeader.h>

class OvertoneAudioProcessor  : public juce::AudioProcessor
{
public:
    OvertoneAudioProcessor();
    ~OvertoneAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void triggerNoteOn();
    void triggerNoteOff();

    juce::AudioProcessorValueTreeState apvts;

    juce::MidiKeyboardState keyboardState;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateFilterCoefficients (float baseFrequencyHz, float currentQ);

    juce::Random random;

    static constexpr int numHarmonics = 8;
    static constexpr int numChannels = 2;

    std::array<std::array<juce::dsp::IIR::Filter<float>, numHarmonics>, numChannels> bandpassFilters;
    std::array<std::array<juce::dsp::IIR::Filter<float>, numHarmonics>, numChannels> allpassFilters;

    std::array<std::atomic<float>*, numHarmonics> harmonicGainParams { nullptr };
    std::atomic<float>* qParam { nullptr };
    std::atomic<float>* masterGainParam { nullptr };

    double currentSampleRate = 44100.0;
    float currentBaseFreq = 261.6256f;
    float cachedQ = 50.0f;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    std::atomic<float>* attackParam { nullptr };
    std::atomic<float>* decayParam { nullptr };
    std::atomic<float>* sustainParam { nullptr };
    std::atomic<float>* releaseParam { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertoneAudioProcessor)
};
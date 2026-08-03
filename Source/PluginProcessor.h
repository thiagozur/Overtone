#pragma once

#include <JuceHeader.h>

class FrequenzAudioProcessor  : public juce::AudioProcessor
{
public:
    FrequenzAudioProcessor();
    ~FrequenzAudioProcessor() override;

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

private:
    juce::Random random;

    static constexpr int numHarmonics = 8;
    static constexpr int numChannels = 2;

    std::array<std::array<juce::dsp::IIR::Filter<float>, numHarmonics>, numChannels> bandpassFilters;
    std::array<std::array<juce::dsp::IIR::Filter<float>, numHarmonics>, numChannels> allpassFilters;

    double currentSampleRate = 44100.0;
    const float baseFreqC4 = 261.6256f; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequenzAudioProcessor)
};
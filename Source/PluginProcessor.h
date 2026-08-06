#pragma once

#include <JuceHeader.h>
#include "Tools/SampleLoopPlayer.h"
#include "Tools/PresetManager.h"

struct FilterVoice
{
    int currentNote = -1;
    bool isActive = false;
    float baseFreq = 440.0f;
    float velocity = 0.0f;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    static constexpr int numHarmonics = 8;

    juce::dsp::IIR::Filter<float> allpassFilters[2][numHarmonics];
    juce::dsp::IIR::Filter<float> bandpassFilters[2][numHarmonics];

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        adsr.setSampleRate (spec.sampleRate);
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int h = 0; h < numHarmonics; ++h)
            {
                allpassFilters[ch][h].prepare (spec);
                bandpassFilters[ch][h].prepare (spec);
            }
        }
    }

    void updateCoefficients (float noteFreq, float currentQ, float detuneAmount, double sampleRate)
    {
        baseFreq = noteFreq;
        for (int i = 0; i < numHarmonics; ++i)
        {
            float harmonicBase = baseFreq * (i + 1);
            float freqL = harmonicBase * (1.0f + detuneAmount * 0.5f);
            float freqR = harmonicBase * (1.0f - detuneAmount * 0.5f);

            if (freqL < sampleRate * 0.49f && freqR < sampleRate * 0.49f)
            {
                allpassFilters[0][i].coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass (sampleRate, freqL, currentQ);
                bandpassFilters[0][i].coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch (sampleRate, freqL, currentQ);

                allpassFilters[1][i].coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass (sampleRate, freqR, currentQ);
                bandpassFilters[1][i].coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch (sampleRate, freqR, currentQ);
            }
            else
            {
                allpassFilters[0][i].reset();
                bandpassFilters[0][i].reset();
                allpassFilters[1][i].reset();
                bandpassFilters[1][i].reset();
            }
        }
    }

    void noteOn (int note, float vel, float q, float width, double sampleRate, const juce::ADSR::Parameters& newAdsrParams)
    {
        currentNote = note;
        velocity = vel;
        isActive = true;

        updateCoefficients (static_cast<float>(juce::MidiMessage::getMidiNoteInHertz (note)), q, width, sampleRate);

        adsrParams = newAdsrParams;
        adsr.setParameters (adsrParams);

        adsr.noteOn();
    }

    void noteOff()
    {
        adsr.noteOff();
    }

    void reset()
    {
        isActive = false;
        currentNote = -1;
        adsr.reset();
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int h = 0; h < numHarmonics; ++h)
            {
                allpassFilters[ch][h].reset();
                bandpassFilters[ch][h].reset();
            }
        }       
    }
};

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

    bool loadCustomNoiseFile (const juce::File& file);

    juce::AudioProcessorValueTreeState apvts;
    PresetManager& getPresetManager() { return presetManager; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }

    float getCurrentEnvelopeLevel() const { return currentEnvelopeLevel.load(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    PresetManager presetManager { apvts };

    juce::Random random;

    static constexpr int maxPolyphony = 8;
    std::array<FilterVoice, maxPolyphony> voices;

    FilterVoice* findFreeVoice();
    FilterVoice* findVoiceForNote (int midiNoteNumber);

    static constexpr int numHarmonics = 8;
    static constexpr int numChannels = 2;

    std::array<std::atomic<float>*, numHarmonics> harmonicGainParams { nullptr };
    std::atomic<float>* driveParam { nullptr };
    std::atomic<float>* widthParam { nullptr };
    std::atomic<float>* qParam { nullptr };
    std::atomic<float>* masterGainParam { nullptr };
    std::atomic<float>* directNoiseParam { nullptr };

    double currentSampleRate = 44100.0;

    std::atomic<float>* attackParam { nullptr };
    std::atomic<float>* decayParam { nullptr };
    std::atomic<float>* sustainParam { nullptr };
    std::atomic<float>* releaseParam { nullptr };
    juce::ADSR::Parameters lastAdsrParams;

    std::atomic<float> currentEnvelopeLevel { 0.0f };

    void updateNoiseSourceSelection (int choice);
    SampleLoopPlayer factorySamplePlayer;
    SampleLoopPlayer customSamplePlayer;
    int currentSourceChoice = -1;
    juce::AudioBuffer<float> noiseBuffer;

    std::atomic<float>* noiseSourceParam { nullptr };

    void getNoiseSample (float& noiseL, float& noiseR, int sourceChoice);

    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    juce::AudioBuffer<float> shimmerFeedbackBuffer;
    int shimmerWritePos = 0;
    float shimmerPhase = 0.0f;

    std::atomic<float>* reverbMixParam { nullptr };
    std::atomic<float>* reverbSizeParam { nullptr };
    std::atomic<float>* shimmerParam { nullptr };

    juce::dsp::Chorus<float> chorus;

    std::atomic<float>* chorusAmountParam { nullptr };

    juce::MidiKeyboardState keyboardState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertoneAudioProcessor)
};
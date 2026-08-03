#include "PluginProcessor.h"
#include "PluginEditor.h"

FrequenzAudioProcessor::FrequenzAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

}

FrequenzAudioProcessor::~FrequenzAudioProcessor()
{

}

const juce::String FrequenzAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FrequenzAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FrequenzAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FrequenzAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FrequenzAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FrequenzAudioProcessor::getNumPrograms()
{
    return 1;
}

int FrequenzAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FrequenzAudioProcessor::setCurrentProgram (int index)
{

}

const juce::String FrequenzAudioProcessor::getProgramName (int index)
{
    return {};
}

void FrequenzAudioProcessor::changeProgramName (int index, const juce::String& newName)
{

}

void FrequenzAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;

    const float Q = 90.0f; 

    for (int i = 0; i < numHarmonics; ++i)
    {
        float harmonicFreq = baseFreqC4 * (i + 1);

        if (harmonicFreq < sampleRate * 0.49f)
        {
            auto apCoeffs = juce::dsp::IIR::Coefficients<float>::makeAllPass (sampleRate, harmonicFreq, Q);
            auto notchCoeffs = juce::dsp::IIR::Coefficients<float>::makeNotch (sampleRate, harmonicFreq, Q);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                allpassFilters[ch][i].coefficients = apCoeffs;
                allpassFilters[ch][i].reset();
                allpassFilters[ch][i].prepare (spec);

                bandpassFilters[ch][i].coefficients = notchCoeffs;
                bandpassFilters[ch][i].reset();
                bandpassFilters[ch][i].prepare (spec);
            }
        }
    }
}

void FrequenzAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FrequenzAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false; 

    return true;
  #endif
}
#endif

void FrequenzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* leftChannel = buffer.getWritePointer (0);
    auto* rightChannel = buffer.getWritePointer (1);

    const float gainAdjustment = 4.0f; 

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float noiseL = random.nextFloat() * 2.0f - 1.0f;
        float noiseR = random.nextFloat() * 2.0f - 1.0f;

        float harmonicSumL = 0.0f;
        float harmonicSumR = 0.0f;

        for (int h = 0; h < numHarmonics; ++h)
        {
            float apL = allpassFilters[0][h].processSample (noiseL);
            float notchL = bandpassFilters[0][h].processSample (noiseL);
            harmonicSumL += (apL - notchL);

            float apR = allpassFilters[1][h].processSample (noiseR);
            float notchR = bandpassFilters[1][h].processSample (noiseR);
            harmonicSumR += (apR - notchR);
        }

        leftChannel[sample] = harmonicSumL * gainAdjustment;
        rightChannel[sample] = harmonicSumR * gainAdjustment;
    }
}

bool FrequenzAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* FrequenzAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void FrequenzAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void FrequenzAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FrequenzAudioProcessor();
}
#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertoneAudioProcessor::OvertoneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    masterGainParam = apvts.getRawParameterValue ("master_gain");
    qParam = apvts.getRawParameterValue ("resonance_q");
    attackParam = apvts.getRawParameterValue ("attack");
    decayParam = apvts.getRawParameterValue ("decay");
    sustainParam = apvts.getRawParameterValue ("sustain");
    releaseParam = apvts.getRawParameterValue ("release");
    
    for (int i = 0; i < numHarmonics; ++i)
    {
        juce::String paramID = "harmonic_gain_" + juce::String(i + 1);
        harmonicGainParams[i] = apvts.getRawParameterValue (paramID);
    }
}

OvertoneAudioProcessor::~OvertoneAudioProcessor()
{

}

juce::AudioProcessorValueTreeState::ParameterLayout OvertoneAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "master_gain", 1 },
        "Master Gain",
        juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "resonance_q", 1 },
        "Resonance (Q)",
        juce::NormalisableRange<float>(5.0f, 150.0f, 0.1f, 0.4f),
        50.0f
    ));

    for (int i = 0; i < numHarmonics; ++i)
    {
        juce::String paramID = "harmonic_gain_" + juce::String (i + 1);
        juce::String paramName = "Harmonic " + juce::String (i + 1);
        
        params.push_back (std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { paramID, 1 },
            paramName,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f),
            1.0f
        ));
    }

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "attack", 1 },
        "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.1f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "decay", 1 },
        "Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.3f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sustain", 1 },
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.8f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "release", 1 },
        "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.5f
    ));

    return { params.begin(), params.end() };
}

const juce::String OvertoneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OvertoneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OvertoneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OvertoneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OvertoneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OvertoneAudioProcessor::getNumPrograms()
{
    return 1;
}

int OvertoneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OvertoneAudioProcessor::setCurrentProgram (int index)
{

}

const juce::String OvertoneAudioProcessor::getProgramName (int index)
{
    return {};
}

void OvertoneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{

}

void OvertoneAudioProcessor::triggerNoteOn()
{
    adsr.noteOn();
}

void OvertoneAudioProcessor::triggerNoteOff()
{
    adsr.noteOff();
}

void OvertoneAudioProcessor::updateFilterCoefficients (float baseFrequencyHz, float currentQ)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;

    for (int i = 0; i < numHarmonics; ++i)
    {
        float harmonicFreq = baseFrequencyHz * (i + 1);

        if (harmonicFreq < currentSampleRate * 0.49f)
        {
            auto apCoeffs = juce::dsp::IIR::Coefficients<float>::makeAllPass (currentSampleRate, harmonicFreq, currentQ);
            auto notchCoeffs = juce::dsp::IIR::Coefficients<float>::makeNotch (currentSampleRate, harmonicFreq, currentQ);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                allpassFilters[ch][i].coefficients = apCoeffs;
                bandpassFilters[ch][i].coefficients = notchCoeffs;
            }
        }
    }

    currentBaseFreq = baseFrequencyHz;
    cachedQ = currentQ;
}

void OvertoneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;

    adsr.setSampleRate (sampleRate);

    float currentQ = qParam->load();

    for (int i = 0; i < numHarmonics; ++i)
    {
        float harmonicFreq = currentBaseFreq * (i + 1);

        if (harmonicFreq < sampleRate * 0.49f)
        {
            auto apCoeffs = juce::dsp::IIR::Coefficients<float>::makeAllPass (sampleRate, harmonicFreq, currentQ);
            auto notchCoeffs = juce::dsp::IIR::Coefficients<float>::makeNotch (sampleRate, harmonicFreq, currentQ);

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

    cachedQ = currentQ;
}

void OvertoneAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OvertoneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OvertoneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            int noteNumber = msg.getNoteNumber();
            float frequencyHz = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz (noteNumber));
            updateFilterCoefficients (frequencyHz, qParam->load());
            adsr.noteOn();
        }
        else if (msg.isNoteOff())
            adsr.noteOff();
    }

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float targetQ = qParam->load();
    if (std::abs (targetQ - cachedQ) > 0.01f)
        updateFilterCoefficients (currentBaseFreq, targetQ);

    adsrParams.attack = attackParam->load();
    adsrParams.decay = decayParam->load();
    adsrParams.sustain = sustainParam->load();
    adsrParams.release = releaseParam->load();
    adsr.setParameters (adsrParams);
    
    float rawMasterDb = masterGainParam->load();
    float masterGainLinear = juce::Decibels::decibelsToGain (rawMasterDb);

    std::array<float, numHarmonics> harmonicGains;
    for (int i = 0; i < numHarmonics; ++i)
        harmonicGains[i] = harmonicGainParams[i]->load();

    auto* leftChannel = buffer.getWritePointer (0);
    auto* rightChannel = buffer.getWritePointer (1);

    const float globalGainCompensation = 4.0f; 

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float env = adsr.getNextSample();

        float noiseL = random.nextFloat() * 2.0f - 1.0f;
        float noiseR = random.nextFloat() * 2.0f - 1.0f;

        float harmonicSumL = 0.0f;
        float harmonicSumR = 0.0f;

        for (int h = 0; h < numHarmonics; ++h)
        {
            float gain = harmonicGains[h];
            if (gain <= 0.0001f)
                continue;

            float apL = allpassFilters[0][h].processSample (noiseL);
            float notchL = bandpassFilters[0][h].processSample (noiseL);
            harmonicSumL += (apL - notchL) * gain;

            float apR = allpassFilters[1][h].processSample (noiseR);
            float notchR = bandpassFilters[1][h].processSample (noiseR);
            harmonicSumR += (apR - notchR) * gain;
        }

        leftChannel[sample] = harmonicSumL * globalGainCompensation * masterGainLinear * env;
        rightChannel[sample] = harmonicSumR * globalGainCompensation * masterGainLinear * env;
    }
}

bool OvertoneAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* OvertoneAudioProcessor::createEditor()
{
    return new OvertoneAudioProcessorEditor (*this);
}

void OvertoneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OvertoneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OvertoneAudioProcessor();
}
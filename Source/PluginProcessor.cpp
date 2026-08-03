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
    noiseSourceParam = apvts.getRawParameterValue ("noise_source");
    driveParam = apvts.getRawParameterValue ("drive");
    widthParam = apvts.getRawParameterValue ("stereo_width");
    reverbSizeParam = apvts.getRawParameterValue ("reverb_size");
    reverbMixParam = apvts.getRawParameterValue ("reverb_mix");
    shimmerParam = apvts.getRawParameterValue ("shimmer_amount");
    
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

    juce::StringArray sourceChoices {
        "Bosque",
        "Lluvia",
        "Arroyo",
        "VHS",
        "Vinilo",
        "Ruido Importado"
    };
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "noise_source", 1 },
        "Noise Source",
        sourceChoices,
        0
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "drive", 1 },
        "Drive",
        juce::NormalisableRange<float>(1.0f, 10.0f, 0.1f, 0.5f),
        1.5f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "master_gain", 1 },
        "Master Gain",
        juce::NormalisableRange<float>(-48.0f, 24.0f, 0.1f),
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

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "stereo_width", 1 },
        "Width",
        juce::NormalisableRange<float>(0.0f, 0.03f, 0.0001f, 0.5f),
        0.003f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "reverb_size", 1 },
        "Reverb Room Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "reverb_mix", 1 },
        "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.3f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "shimmer_amount", 1 },
        "Shimmer",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f
    ));

    return { params.begin(), params.end() };
}

bool OvertoneAudioProcessor::loadCustomNoiseFile (const juce::File& file)
{
    return customSamplePlayer.loadFromFile (file, currentSampleRate);
}

void OvertoneAudioProcessor::updateNoiseSourceSelection (int choice)
{
    if (choice == currentSourceChoice)
        return;
    
    currentSourceChoice = choice;

    switch (choice)
    {
        case 0:
            factorySamplePlayer.loadFromMemory (BinaryData::birds_wav, BinaryData::birds_wavSize, currentSampleRate);
            break;
        case 1:
            factorySamplePlayer.loadFromMemory (BinaryData::rain_wav, BinaryData::rain_wavSize, currentSampleRate);
            break;
        case 2:
            factorySamplePlayer.loadFromMemory (BinaryData::river_wav, BinaryData::river_wavSize, currentSampleRate);
            break;
        case 3:
            factorySamplePlayer.loadFromMemory (BinaryData::vhs_wav, BinaryData::vhs_wavSize, currentSampleRate);
            break;
        case 4:
            factorySamplePlayer.loadFromMemory (BinaryData::vinyl_wav, BinaryData::vinyl_wavSize, currentSampleRate);
            break;
        case 5:
            break;
    }
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

void OvertoneAudioProcessor::setCurrentProgram (int /* index */)
{

}

const juce::String OvertoneAudioProcessor::getProgramName (int /* index */)
{
    return {};
}

void OvertoneAudioProcessor::changeProgramName (int /* index */, const juce::String& /* newName */)
{

}

void OvertoneAudioProcessor::updateFilterCoefficients (float baseFrequencyHz, float currentQ, float detuneAmount)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;

    for (int i = 0; i < numHarmonics; ++i)
    {
        float harmonicBase = baseFrequencyHz * (i + 1);

        float freqL = harmonicBase * (1.0f + detuneAmount * 0.5f);
        float freqR = harmonicBase * (1.0f - detuneAmount * 0.5f);

        if (freqL < currentSampleRate * 0.49f && freqR < currentSampleRate * 0.49f)
        {
            auto apCoeffsL = juce::dsp::IIR::Coefficients<float>::makeAllPass (currentSampleRate, freqL, currentQ);
            auto notchCoeffsL = juce::dsp::IIR::Coefficients<float>::makeNotch (currentSampleRate, freqL, currentQ);

            allpassFilters[0][i].coefficients = apCoeffsL;
            bandpassFilters[0][i].coefficients = notchCoeffsL;

            auto apCoeffsR = juce::dsp::IIR::Coefficients<float>::makeAllPass (currentSampleRate, freqR, currentQ);
            auto notchCoeffsR = juce::dsp::IIR::Coefficients<float>::makeNotch (currentSampleRate, freqR, currentQ);

            allpassFilters[1][i].coefficients = apCoeffsR;
            bandpassFilters[1][i].coefficients = notchCoeffsR;
        }
    }

    currentBaseFreq = baseFrequencyHz;
    cachedQ = currentQ;
    cachedWidth = detuneAmount;
}

void OvertoneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());;

    reverb.prepare (spec);

    int shimmerBufferLength = static_cast<int>(sampleRate * 0.1);
    shimmerFeedbackBuffer.setSize (2, shimmerBufferLength);
    shimmerFeedbackBuffer.clear();
    shimmerWritePos = 0;

    adsr.setSampleRate (sampleRate);

    float currentQ = qParam->load();

    juce::dsp::ProcessSpec filterSpec = spec;
    filterSpec.numChannels = 1;

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
                allpassFilters[ch][i].prepare (filterSpec);

                bandpassFilters[ch][i].coefficients = notchCoeffs;
                bandpassFilters[ch][i].reset();
                bandpassFilters[ch][i].prepare (filterSpec);
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

    int sourceChoice = static_cast<int>(noiseSourceParam->load());
    updateNoiseSourceSelection (sourceChoice);

    float drive = driveParam->load();

    float targetQ = qParam->load();
    float targetWidth = widthParam->load();
    float reverbSize = reverbSizeParam->load();
    float reverbMix = reverbMixParam->load();
    float shimmerAmount = shimmerParam->load();

    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            int noteNumber = msg.getNoteNumber();
            float frequencyHz = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz (noteNumber));
            updateFilterCoefficients (frequencyHz, targetQ, targetWidth);
            adsr.noteOn();
        }
        else if (msg.isNoteOff())
            adsr.noteOff();
    }

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (std::abs (targetQ - cachedQ) > 0.01f || std::abs (targetWidth - cachedWidth) > 0.0001f)
        updateFilterCoefficients (currentBaseFreq, targetQ, targetWidth);
       

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

        float noiseL = 0.0f;
        float noiseR = 0.0f;

        if (sourceChoice >= 0 && sourceChoice <= 4)
            factorySamplePlayer.getNextSample (noiseL, noiseR);
        else if (sourceChoice == 5 && customSamplePlayer.hasSampleLoaded())
            customSamplePlayer.getNextSample (noiseL, noiseR);
        else
        {
            noiseL = random.nextFloat() * 2.0f - 1.0f;
            noiseR = random.nextFloat() * 2.0f - 1.0f;
        }

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

        float processedL = harmonicSumL * globalGainCompensation * env;
        float processedR = harmonicSumR * globalGainCompensation * env;

        processedL = std::tanh (processedL * drive);
        processedR = std::tanh (processedR * drive);

        leftChannel[sample] = processedL;
        rightChannel[sample] = processedR;
    }

    float effectiveShimmer = shimmerAmount * reverbMix;

    if (effectiveShimmer > 0.01f && shimmerFeedbackBuffer.getNumSamples() > 0)
    {
        int bufLen = shimmerFeedbackBuffer.getNumSamples();
        float halfLen = static_cast<float>(bufLen) * 0.5f;

        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            shimmerFeedbackBuffer.setSample (0, shimmerWritePos, leftChannel[s]);
            shimmerFeedbackBuffer.setSample (1, shimmerWritePos, rightChannel[s]);

            float pos1 = std::fmod (shimmerPhase, halfLen);
            float pos2 = std::fmod (shimmerPhase + (halfLen * 0.5f), halfLen);

            int idx1A = static_cast<int>(pos1) % bufLen;
            int idx1B = (idx1A + 1) % bufLen;
            float frac1 = pos1 - std::floor(pos1);

            int idx2A = static_cast<int>(pos2) % bufLen;
            int idx2B = (idx2A + 1) % bufLen;
            float frac2 = pos2 - std::floor(pos2);

            float shimL1 = shimmerFeedbackBuffer.getSample (0, idx1A) * (1.0f - frac1) + shimmerFeedbackBuffer.getSample (0, idx1B) * frac1;
            float shimR1 = shimmerFeedbackBuffer.getSample (1, idx1A) * (1.0f - frac1) + shimmerFeedbackBuffer.getSample (1, idx1B) * frac1;

            float shimL2 = shimmerFeedbackBuffer.getSample (0, idx2A) * (1.0f - frac2) + shimmerFeedbackBuffer.getSample (0, idx2B) * frac2;
            float shimR2 = shimmerFeedbackBuffer.getSample (1, idx2A) * (1.0f - frac2) + shimmerFeedbackBuffer.getSample (1, idx2B) * frac2;

            float w1 = 1.0f - std::abs ((pos1 / (halfLen * 0.5f)) - 1.0f);
            float w2 = 1.0f - std::abs ((pos2 / (halfLen * 0.5f)) - 1.0f);

            float outputShimmerL = (shimL1 * w1 + shimL2 * w2) * effectiveShimmer * 0.35f;
            float outputShimmerR = (shimR1 * w1 + shimR2 * w2) * effectiveShimmer * 0.35f;

            leftChannel[s]  += outputShimmerL;
            rightChannel[s] += outputShimmerR;

            shimmerWritePos = (shimmerWritePos + 1) % bufLen;
            shimmerPhase += 2.0f;
            if (shimmerPhase >= static_cast<float>(bufLen))
                shimmerPhase -= static_cast<float>(bufLen);
        }
    }

    if (reverbMix > 0.01f)
    {
        reverbParams.roomSize = reverbSize;
        reverbParams.damping = 0.3f;
        reverbParams.wetLevel = reverbMix;
        reverbParams.dryLevel = 1.0f - (reverbMix * 0.5f);
        reverbParams.width = 1.0f;
        reverbParams.freezeMode = 0.0f;
        reverb.setParameters (reverbParams);

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        reverb.process (context);
    }

    buffer.applyGain (masterGainLinear);
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
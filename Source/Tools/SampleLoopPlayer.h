#pragma once

#include <JuceHeader.h>

class SampleLoopPlayer
{
public:
    SampleLoopPlayer()
    {
        formatManager.registerBasicFormats();
    }

    bool loadFromFile (const juce::File& file, double targetSampleRate)
    {
        std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));

        if (reader != nullptr)
        {
            int maxAllowedSamples = static_cast<int>(targetSampleRate * 5.0);
            int samplesToRead = std::min (static_cast<int>(reader->lengthInSamples), maxAllowedSamples);

            sampleBuffer.setSize (static_cast<int>(reader->numChannels), samplesToRead);
            reader->read (&sampleBuffer, 0, samplesToRead, 0, true, true);

            double sumOfSquares = 0.0;
            int totalSamples = sampleBuffer.getNumSamples() * sampleBuffer.getNumChannels();

            for (int ch = 0; ch < sampleBuffer.getNumChannels(); ++ch)
            {
                const float* channelData = sampleBuffer.getReadPointer (ch);
                for (int s = 0; s < sampleBuffer.getNumSamples(); ++s)
                {
                    float sample = channelData[s];
                    sumOfSquares += static_cast<double>(sample * sample);
                }
            }

            float currentRMS = static_cast<float>(std::sqrt (sumOfSquares / totalSamples));

            const float targetRMS = 0.15f; 

            if (currentRMS > 0.0001f)
            {
                float normalizationMultiplier = targetRMS / currentRMS;

                normalizationMultiplier = std::min (normalizationMultiplier, 8.0f);

                sampleBuffer.applyGain (normalizationMultiplier);
            }

            playhead = 0;
            isLoaded = true;

            crossfadeSamples = static_cast<int>(targetSampleRate * 0.1);
            return true;
        }

        return false;
    }

    bool loadFromMemory (const void* data, size_t datasize, double targetSampleRate)
    {
        auto stream = std::make_unique<juce::MemoryInputStream> (data, datasize, false);
        std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (std::move (stream)));

        if (reader != nullptr)
        {
            int maxAllowedSamples = static_cast<int>(targetSampleRate * 5.0);
            int samplesToRead = std::min (static_cast<int>(reader->lengthInSamples), maxAllowedSamples);
            
            sampleBuffer.setSize (static_cast<int>(reader->numChannels), samplesToRead);
            reader->read (&sampleBuffer, 0, samplesToRead, 0, true, true);

            double sumOfSquares = 0.0;
            int totalSamples = sampleBuffer.getNumSamples() * sampleBuffer.getNumChannels();

            for (int ch = 0; ch < sampleBuffer.getNumChannels(); ++ch)
            {
                const float* channelData = sampleBuffer.getReadPointer (ch);
                for (int s = 0; s < sampleBuffer.getNumSamples(); ++s)
                {
                    float sample = channelData[s];
                    sumOfSquares += static_cast<double>(sample * sample);
                }
            }

            float currentRMS = static_cast<float>(std::sqrt (sumOfSquares / totalSamples));

            const float targetRMS = 0.15f; 

            if (currentRMS > 0.0001f)
            {
                float normalizationMultiplier = targetRMS / currentRMS;

                normalizationMultiplier = std::min (normalizationMultiplier, 8.0f);

                sampleBuffer.applyGain (normalizationMultiplier);
            }

            playhead = 0;
            isLoaded = true;

            crossfadeSamples = static_cast<int>(targetSampleRate * 0.1);
            return true;
        }
        
        return false;
    }

    void getNextSample (float& outL, float& outR)
    {
        int totalSamples = sampleBuffer.getNumSamples();

        if (!isLoaded || totalSamples == 0)
        {
            outL = 0.0f;
            outR = 0.0f;
            return;
        }

        int numChannels = sampleBuffer.getNumChannels();
        int currentReadIdx = playhead;

        float currentL = sampleBuffer.getSample (0, currentReadIdx);
        float currentR = (numChannels > 1) ? sampleBuffer.getSample (1, currentReadIdx) : currentL;

        int fadeStartIdx = totalSamples - crossfadeSamples;
        if (currentReadIdx >= fadeStartIdx && fadeStartIdx > 0)
        {
            float progress = static_cast<float>(currentReadIdx - fadeStartIdx) / static_cast<float>(crossfadeSamples);

            float fadeOutGain = std::cos (progress * juce::MathConstants<float>::halfPi);
            float fadeInGain = std::sin (progress * juce::MathConstants<float>::halfPi);

            int wrapIdx = currentReadIdx - fadeStartIdx;
            float wrapL = sampleBuffer.getSample (0, wrapIdx);
            float wrapR = (numChannels > 1) ? sampleBuffer.getSample (1, wrapIdx) : wrapL;

            outL = (currentL * fadeOutGain) + (wrapL * fadeInGain);
            outR = (currentR * fadeOutGain) + (wrapR * fadeInGain);
        }
        else
        {
            outL = currentL;
            outR = currentR;
        }

        playhead++;
        if (playhead >= totalSamples)
            playhead = 0;
    }

    bool hasSampleLoaded() const
    {
        return isLoaded;
    }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> sampleBuffer;

    int playhead = 0;
    int crossfadeSamples = 4410;
    bool isLoaded = false;
};
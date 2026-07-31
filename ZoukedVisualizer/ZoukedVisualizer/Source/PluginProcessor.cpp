/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace std;

//==============================================================================
ZoukedVisualizerAudioProcessor::ZoukedVisualizerAudioProcessor()
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
    /* FIFO 
    fifoBuffer.resize(fifoSize, 0.0f);
    */
   bufferSpecsFifoBuffer.resize(5, 0.0f);
   sampleRate = this->getSampleRate();
}

ZoukedVisualizerAudioProcessor::~ZoukedVisualizerAudioProcessor()
{
}

//==============================================================================
const juce::String ZoukedVisualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ZoukedVisualizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ZoukedVisualizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ZoukedVisualizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ZoukedVisualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ZoukedVisualizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ZoukedVisualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ZoukedVisualizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ZoukedVisualizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void ZoukedVisualizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ZoukedVisualizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    this->sampleRate = sampleRate;
}

void ZoukedVisualizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ZoukedVisualizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ZoukedVisualizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    const int numSamples = buffer.getNumSamples();
    const float* channelData = buffer.getReadPointer(0); // Only keeping the left channel for visualization

    const int fftSize = fastFourier.getSize();
    vector<float> fftData (2 * fftSize, 0.0f);
    const int samplesToCopy = jmin (numSamples, fftSize);

    fill (fftData.begin(), fftData.end(), 0.0f);
    copy_n (channelData, samplesToCopy, fftData.begin());

    const float peak = buffer.getMagnitude(0, numSamples);
    const float RMS = buffer.getRMSLevel(0, 0, numSamples);

    fastFourier.performFrequencyOnlyForwardTransform (fftData.data(), true);

    const auto dominantFrequencies = getTopFrequencies (fftData.data(), fftSize / 2 + 1);
    const std::array<float, 5> bufferSpecsSnapshot = { peak, RMS, dominantFrequencies[0], dominantFrequencies[1], dominantFrequencies[2] };

    int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
    bufferSpecsFifo.prepareToWrite (bufferSpecsFifoSize, start1, size1, start2, size2);

    if (size1 > 0)
        std::copy_n (bufferSpecsSnapshot.begin(), size1, bufferSpecsFifoBuffer.begin() + start1);

    if (size2 > 0)
        std::copy_n (bufferSpecsSnapshot.begin() + size1, size2, bufferSpecsFifoBuffer.begin() + start2);

    bufferSpecsFifo.finishedWrite (size1 + size2);

    }

//==============================================================================
bool ZoukedVisualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ZoukedVisualizerAudioProcessor::createEditor()
{
    return new ZoukedVisualizerAudioProcessorEditor (*this);
}

//==============================================================================
void ZoukedVisualizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ZoukedVisualizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZoukedVisualizerAudioProcessor();
}

float ZoukedVisualizerAudioProcessor::highestAmplitudeFreq(float* dft, const int dftSamples)
{
    if (dft == nullptr || dftSamples <= 0)
        return 0.0f;

    int maxIndice = 0;
    float maxValue = dft[0];

    for (int i = 1; i < dftSamples; ++i)
    {
        if (dft[i] > maxValue)
        {
            maxValue = dft[i];
            maxIndice = i;
        }
    }

    return static_cast<float> (maxIndice);
}

array<float, 3> ZoukedVisualizerAudioProcessor::getTopFrequencies(const float* spectrum, int spectrumSize) const
{
    array<float, 3> topValues = {{ 0.0f, 0.0f, 0.0f }};
    array<int, 3> topIndices = {{ 0, 0, 0 }};

    for (int i = 0; i < spectrumSize; ++i)
    {
        const float currentValue = spectrum[i];

        for (int j = 0; j < 3; ++j)
        {
            if (currentValue > topValues[j])
            {
                for (int k = 2; k > j; --k)
                {
                    topValues[k] = topValues[k - 1];
                    topIndices[k] = topIndices[k - 1];
                }

                topValues[j] = currentValue;
                topIndices[j] = i;
                break;
            }
        }
    }

    const float frequencyStep = static_cast<float> (sampleRate / static_cast<double> (fastFourier.getSize()));

    return {{ topIndices[0] * frequencyStep,
              topIndices[1] * frequencyStep,
              topIndices[2] * frequencyStep }};
}
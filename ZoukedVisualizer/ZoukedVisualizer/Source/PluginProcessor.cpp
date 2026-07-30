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
   bufferSpecs.resize(5, 0.0f);
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
    int numSamples = buffer.getNumSamples();
    const float* channelData = buffer.getReadPointer(0); // Only keeping the left channel for visualization
    const float doubledData[2*numSamples];
    for(int i = 0; i < 2*numSamples; i++)
    {i<numSamples ? doubleData[i] = channelData[i] : doubleData[i] = 0;}

    float peak = buffer.getMagnitude(0, numSamples);

    float RMS = buffer.getRMSLevel(0, 0, numSamples);

    fastFourier.performFrequencyOnlyForwardTransform(doubleData, true);

    float firstFreq = highestAmplitudeFreq(doubleData, numSamples);
    float secondFreq = highestAmplitudeFreq(doubleData, numSamples-1);
    float thirdFreq = highestAmplitudeFreq(doubleData, numSamples-2);

    bufferSpecs = {peak, RMS, firstFreq, secondFreq, thirdFreq};

    /* FIFO Writing data to the FIFO, to send it to the OpenGL component
    int start1, size1, start2, size2;
    audioFifo.prepareToWrite (numSamples, start1, size1, start2, size2);
 
    if (size1 > 0)
        copy(channelData, channelData + size1, fifoBuffer.begin() + start1);
 
    if (size2 > 0)
        copy(channelData + size1, channelData + size1 + size2, fifoBuffer.begin() + start2);

    audioFifo.finishedWrite (size1 + size2);
    */

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

float highestAmplitudeFreq(float* dft, const int dftSamples)
{
    int max = 0;
    int maxindice = 0;
    float dftbuffer[dftSamples - 1];
    for(int i = 0; i < dftSamples; i++)
    {
        if (dft[i]>max)
        {
            max = dft[i];
            maxindice = i;
        }
    }
    for(int j = 0, k = 0; j < dftSamples-1; j++)
    {
        if (j != maxindice)
        {
            k++;
            dftbuffer[j] = dft[k];
        }
    }
    dft = dftbuffer;
    return max;
}
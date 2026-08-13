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
   bufferSpecsFifoBuffer.resize(6, 0.0f); // has to be +1 of the number of wagons in the train
   sampleRate = this->getSampleRate();
   formatManager.registerBasicFormats();
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

    setLatencySamples(fourier.getLatencyInSamples());

    fourier.reset();

    if (wrapperType == wrapperType_Standalone) // Enabled only in standalone mode
    {
        File audioFile("/home/user/Musique/Sometimes.wav");
        auto* reader = formatManager.createReaderFor(audioFile);

        if (reader != nullptr)
        {
            readerSource.reset(new AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(), 0, nullptr, sampleRate);
            transportSource.start();
        }
        transportSource.prepareToPlay(samplesPerBlock, sampleRate);
    }
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
    // ==============================================================================
    if (wrapperType == AudioProcessor::wrapperType_Standalone) // Enabled only in standalone mode
    {
        AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
        transportSource.getNextAudioBlock(info); // overwrites buffer with file's audio
    }
    // ==============================================================================

    juce::ScopedNoDenormals noDenormals;
    bool bypassed = false;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const int numSamples = buffer.getNumSamples();
    const float* channelData = buffer.getReadPointer(0); // Only keeping the left channel for visualization

    float peak = -1.0f, RMS = -1.0f;
    array<float, 3> bandsAmplitude({ -1.0f, -1.0f, -1.0f });
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    // Perform FFT ========================
    for (int index = 0; index < numSamples; ++index) {
        float sample = channelData[index];

        bandsAmplitude = fourier.processSample(sample, bypassed);

        // if we do processing : channelData[index] = sample;
    }
    // ======================================

    peak = buffer.getMagnitude(0, numSamples);
    RMS = buffer.getRMSLevel(0, 0, numSamples);
    
    array<float, 5> bufferSpecsSnapshot = { peak, RMS, bandsAmplitude[0], bandsAmplitude[1], bandsAmplitude[2] };


    int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
    bufferSpecsFifo.prepareToWrite (5, start1, size1, start2, size2);

    if (size1 + size2 == 5)
    {
        if (size1 > 0)
            copy_n (bufferSpecsSnapshot.begin(), size1, bufferSpecsFifoBuffer.begin() + start1);

        if (size2 > 0)
            copy_n (bufferSpecsSnapshot.begin() + size1, size2, bufferSpecsFifoBuffer.begin() + start2);
    
    //cout << "#WROTE :";
    //for (int i = 0; i <5; i++ ){cout << bufferSpecsFifoBuffer[i] << "- ";} cout << endl;
    
    }
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
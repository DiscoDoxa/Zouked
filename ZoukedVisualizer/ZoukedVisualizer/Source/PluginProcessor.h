/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FFTProcessor.h"

using namespace juce;
using namespace std;

//==============================================================================
/**
*/
class ZoukedVisualizerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ZoukedVisualizerAudioProcessor();
    ~ZoukedVisualizerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AbstractFifo& getBufferSpecsFifo() { return bufferSpecsFifo; }
    vector<float>& getBufferSpecsFifoBuffer() { return bufferSpecsFifoBuffer; }
    // =============================================================================
    /* FIFO
    AbstractFifo& getAudioFifo() { return audioFifo; }
    vector<float>& getFifoBuffer() { return fifoBuffer; }
    */
    // =============================================================================
    
private:

    double sampleRate = 44100.0;

    // We need a separate FFTProcessor for each channel.
    FFTProcessor fourier { sampleRate };

    // =============================================================================
    /* FIFO
    static constexpr int fifoSize = 4096; 
    vector<float> fifoBuffer; // sized fifoSize, mono (or interleaved if you want stereo)
    AbstractFifo audioFifo { fifoSize }; // creates fifo of size fifoSize for left channel
    */
    vector<float> bufferSpecsFifoBuffer;
    AbstractFifo bufferSpecsFifo { 6 };
    // =============================================================================
    AudioFormatManager formatManager;
    unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZoukedVisualizerAudioProcessor)
};
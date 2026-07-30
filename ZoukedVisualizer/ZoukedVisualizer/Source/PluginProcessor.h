/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

    vector<float>& getbufferSpecs() { return bufferSpecs; }
    // =============================================================================
    /* FIFO
    AbstractFifo& getAudioFifo() { return audioFifo; }
    vector<float>& getFifoBuffer() { return fifoBuffer; }
    */
    // =============================================================================
    
private:

    vector<float> bufferSpecs;
    juce::dsp::FFT fastFourier { 11 };
    
    float highestAmplitudeFreq(float* dft, const int dftSamples);
    // =============================================================================
    /* FIFO
    static constexpr int fifoSize = 4096; 
    vector<float> fifoBuffer; // sized fifoSize, mono (or interleaved if you want stereo)
    AbstractFifo audioFifo { fifoSize }; // creates fifo of size fifoSize for left channel
    */
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZoukedVisualizerAudioProcessor)
};
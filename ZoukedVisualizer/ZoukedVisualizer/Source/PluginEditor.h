/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Include the OpenGLComponent.h file.
#include "OpenGLComponent.h"

//==============================================================================
/**
*/
class ZoukedVisualizerAudioProcessorEditor  : public juce::AudioProcessorEditor, public KeyListener
{
public:
    ZoukedVisualizerAudioProcessorEditor (ZoukedVisualizerAudioProcessor&);
    ~ZoukedVisualizerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool keyPressed(const KeyPress& key, Component *originatingComponent) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ZoukedVisualizerAudioProcessor& audioProcessor;

    // Create an instance of the OpenGLComponent.
    OpenGLComponent openGLComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZoukedVisualizerAudioProcessorEditor)
};

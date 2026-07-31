/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define initial_W 800
#define initial_H 600

//==============================================================================
ZoukedVisualizerAudioProcessorEditor::ZoukedVisualizerAudioProcessorEditor (ZoukedVisualizerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      openGLComponent(initial_W, initial_H, audioProcessor.getBufferSpecsFifo(), audioProcessor.getBufferSpecsFifoBuffer())
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (initial_W, initial_H);
    setResizable(true, true);
    openGLComponent.setBounds(0, 0, initial_W, initial_H);

    // Add the child Component and make it visible.
    addAndMakeVisible(openGLComponent);

    // for inputs
    setWantsKeyboardFocus(true);
    addKeyListener(this);
}

ZoukedVisualizerAudioProcessorEditor::~ZoukedVisualizerAudioProcessorEditor()
{
    removeKeyListener(this);
}

//==============================================================================
void ZoukedVisualizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ZoukedVisualizerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    openGLComponent.setBounds(getLocalBounds());

}

bool ZoukedVisualizerAudioProcessorEditor::keyPressed(const KeyPress& key, Component* originatingComponent)
{
    if (key == KeyPress('f'))
    {
        if (auto* peer = getPeer())
        {
            peer->setFullScreen(!peer->isFullScreen());
            return true;
        }
    }

    if (key == KeyPress::escapeKey)
    {
        JUCEApplication::getInstance()->systemRequestedQuit();
        return true;
    }

    return false;
}

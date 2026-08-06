/*
  ==============================================================================

    OpenGLComponent.h
    Created: 28 Jul 2026 9:36:40am
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;
using namespace std;
//==============================================================================
/*
*/
class OpenGLComponent  : public juce::Component, public OpenGLRenderer
{
public:
    OpenGLComponent(unsigned int width, unsigned int height, AbstractFifo& bufferSpecsFifoRef, vector<float>& bufferSpecsFifoBufferRef); /* FIFO AbstractFifo& audioFifoRef, vector<float>& fifoBufferRef); */
    ~OpenGLComponent() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    unsigned int WIDTH;
    unsigned int HEIGHT;

private:

    // Create an OpenGLContext for this Component.
    OpenGLContext openGLContext;

    struct Vertex
    {
        float position[2];
        float colour[4];
    };

    vector<Vertex> vertexBuffer;
    vector<unsigned int> indexBuffer;

    unsigned int vbo; // Vertex buffer object.
    unsigned int ibo; // Index buffer object.

    String vertexShader;
    String fragmentShader;

    String readFile(const String& filepath);

    unique_ptr<OpenGLShaderProgram> shaderProgram;

    Time timeTracker;
    float playTime = 0.0f;
    double startTime = 0.0;

    float mouvTime = 0.0f; // is named mouvTime but actually its a position reactin to bass input
    float old_mouvTime = 0.0f;
    
    bool usable_shader_program = false;

    // ===============================================================================
    vector<float> bufferSpecs;
    AbstractFifo& bufferSpecsFifo;
    vector<float>& bufferSpecsFifoBuffer;
    void updatebufferSpecs();
  
    /* FIFO
    vector<float> samples;
    AbstractFifo& audioFifo;
    vector<float>& fifoBuffer;
    void updateSamples();
    */

  
    // ===============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent)
};

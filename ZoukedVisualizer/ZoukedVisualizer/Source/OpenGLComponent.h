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
    OpenGLComponent(unsigned int width = 800, unsigned int height = 600);
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

    char *readFile(const char *filepath);
    char *concatenate(const char *filepath, const char *folderpath, char* fullPath);

    unique_ptr<OpenGLShaderProgram> shaderProgram;

    Time timeTracker;
    float playTime = 0.0f;
    double startTime = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent)
};

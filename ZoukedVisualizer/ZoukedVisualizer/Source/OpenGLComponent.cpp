/*
  ==============================================================================/home/user/Bureau/JUCE examples/same/OpenGLDemo/Source/DemoUtilities.h
  /home/user/Bureau/JUCE examples/same/OpenGLDemo/Source/Main.cpp
  /home/user/Bureau/JUCE examples/same/OpenGLDemo/Source/OpenGLDemo.h
  /home/user/Bureau/JUCE examples/same/OpenGLDemo/Source/WavefrontObjParser.h

    OpenGLComponent.cpp
    Created: 28 Jul 2026 9:36:40am
    Author:  user

  ==============================================================================
*/

#include <JuceHeader.h>
#include <algorithm>

#if defined (JUCE_MAC)
 #include <OpenGL/gl.h>
#else
 #include <GL/gl.h>
#endif

#include "OpenGLComponent.h"
#include <cmath>

using namespace juce;
using namespace std;
using namespace gl;

//==============================================================================
OpenGLComponent::OpenGLComponent(unsigned int width, unsigned int height, AbstractFifo& bufferSpecsFifoRef, vector<float>& bufferSpecsFifoBufferRef) /* FIFO AbstractFifo& audioFifoRef, vector<float>& fifoBufferRef) */
    : WIDTH(width), HEIGHT(height), bufferSpecsFifo(bufferSpecsFifoRef), bufferSpecsFifoBuffer(bufferSpecsFifoBufferRef)
{
    bufferSpecs.resize(5, 0.0f);
    /* FIFO samples.resize(fifoBufferRef.size()); */

    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    // Indicates that no part of this Component is transparent.
    setOpaque(true);

    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);

    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);

    // VSYNC: openGLContext.setSwapInterval(1);
}

OpenGLComponent::~OpenGLComponent()
{
    // Tell the context to stop using this Component.
    openGLContext.detach();
}

void OpenGLComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
}

void OpenGLComponent::resized()
{
    WIDTH = getWidth();
    HEIGHT = getHeight();
}

void OpenGLComponent::newOpenGLContextCreated()
{
    // 1. vertices (= sommets)
    // _______________________

    // Create 4 vertices each with a different colour.
    
    vertexBuffer = {
        // Vertex 0
        {
            { -1.0f, 1.0f },        // (-0.5, 0.5)
            { 1.f, 0.f, 0.f, 1.f }  // Red
        },
        // Vertex 1
        {
            { 1.0f, 1.0f },         // (0.5, 0.5)
            { 1.f, 1.0f, 0.f, 1.f } // Orange
        },
        // Vertex 2
        {
            { 1.0f, -1.0f },        // (0.5, -0.5)
            { 1.f, 1.f, 0.f, 1.f }  // Yellow
        },
        // Vertex 3
        {
            { -1.0f, -1.0f },       // (-0.5, -0.5)
            { 1.f, 0.f, 1.f, 1.f }  // Purple
        }
    };
    
    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer = {
        0, 1, 2,
        0, 2, 3
    };

    // 2. buffers (in the GPU for fast access)
    // _________________________________________

    // Generate 1 buffer, using our vbo variable to store its ID.
    openGLContext.extensions.glGenBuffers(1, &vbo);
    
    // Generate 1 more buffer, this time using our IBO variable.
    openGLContext.extensions.glGenBuffers(1, &ibo);

    // Bind the VBO.
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Send the vertices data.
    openGLContext.extensions.glBufferData(
        GL_ARRAY_BUFFER,                        // The type of data we're sending.           
        sizeof(Vertex) * vertexBuffer.size(),   // The size (in bytes) of the data.
        vertexBuffer.data(),                    // A pointer to the actual data.
        GL_STATIC_DRAW                          // How we want the buffer to be drawn.
    );
    
    // Bind the IBO.
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    
    // Send the indices data.
    openGLContext.extensions.glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * indexBuffer.size(),
        indexBuffer.data(),
        GL_STATIC_DRAW
    );

    // 3. shaders and shader program
    // _____________________________

    at_least_one_works = linkShaderPrograms();
    if(at_least_one_works) setCurrentShader("template");

}

void OpenGLComponent::renderOpenGL()
{
    if (at_least_one_works)
    {
    // -----
    // input
    // -----

    // ------------------------
    // update uniform variables
    // ------------------------

    glViewport(0, 0, WIDTH, HEIGHT);
    updateAndUseShaderProgram(currentShader);
    

    //cout << "|> " << (int)playTime << " s - W: " << WIDTH << "p - H: " << HEIGHT << '\r' << flush;
    //cout << "|> Peak: " << bufferSpecs[0] << " - RMS: " << bufferSpecs[1] << " - L: " << bufferSpecs[2] << " - M: " << bufferSpecs[3] << " - H: " << bufferSpecs[4] << '\r' << flush;
    // ----------------------------------
    // gl: rendering (write into buffers)
    // ----------------------------------


    // Clear the screen by filling it with black.
    OpenGLHelpers::clear(Colours::black);

    

    // Bind the VBO and IBO.
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
        0,              // The attribute's index (AKA location).
        2,              // How many values this attribute contains.
        GL_FLOAT,       // The attribute's type (float).
        GL_FALSE,       // Tells OpenGL NOT to normalise the values.
        sizeof(Vertex), // How many bytes to move to find the attribute with
                        // the same index in the next vertex.
        nullptr         // How many bytes to move from the start of this vertex
                        // to find this attribute (the default is 0 so we just
                        // pass nullptr here).
    );
    openGLContext.extensions.glEnableVertexAttribArray(0);
    
    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
        1,                              // This attribute has an index of 1
        4,                              // This time we have four values for the
                                        // attribute (r, g, b, a)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
                                        // position attribute in the Vertex
                                        // struct, so we need to skip over the
                                        // size of the position array to find
                                        // the start of this attribute.
    );
    openGLContext.extensions.glEnableVertexAttribArray(1);

    glDrawElements(
        GL_TRIANGLES,       // Tell OpenGL to render triangles.
        indexBuffer.size(), // How many indices we have.
        GL_UNSIGNED_INT,    // What type our indices are.
        nullptr             // We already gave OpenGL our indices so we don't
                            // need to pass that again here, so pass nullptr.
    );

    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
    }
}

void OpenGLComponent::openGLContextClosing()
{
    // Release the shader program.
    for (auto it = shaderPrograms.begin(); it != shaderPrograms.end(); ++it) 
    {
        it->second.reset(); //not release because dangling pointer issue
    }
    

    // free the VBO and IBO.
    openGLContext.extensions.glDeleteBuffers(1, &vbo);
    openGLContext.extensions.glDeleteBuffers(1, &ibo);
}

String OpenGLComponent::readFile(const String& filepath)
{
    File file(filepath);
    if (!file.existsAsFile())
    {
        DBG("Failed to open shader file: " << filepath);
        return {};
    }

    return file.loadFileAsString();
}

void OpenGLComponent::updatebufferSpecs()
{
    vector<float> bufferSpecsCache = bufferSpecs;
    const int numReady = bufferSpecsFifo.getNumReady();
    if (numReady >= 1)
    {
        //cout << "test" << endl;
        int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
        bufferSpecsFifo.prepareToRead (5, start1, size1, start2, size2);

        if (size1 > 0)
            copy_n (bufferSpecsFifoBuffer.begin() + start1,
                    size1,
                    bufferSpecs.begin());

        if (size2 > 0)
            copy_n (bufferSpecsFifoBuffer.begin() + start2,
                    size2,
                    bufferSpecs.begin() + size1);

        bufferSpecsFifo.finishedRead (size1 + size2);

        //cout << "#READ :";
        //for (int i = 0; i <5; i++ ){cout << bufferSpecs[i] << "- ";} cout << "size1: " << size1 << " size2: " << size2 << endl;
    }

    const int mass = 10;

    lowPos += cbrt(0.0005f * bufferSpecs[2] / mass);
    
    midPos += cbrt(0.0005f * bufferSpecs[3] / (mass*10.0f)); 

    highPos += cbrt(0.0005f * bufferSpecs[4]/ (mass*100.0f)); 
}

bool OpenGLComponent::linkShaderPrograms()
{
    bool at_least_one_shaderProgram_works = false;

    for (int s = 0; s < numShaders; s++)
    {
        unique_ptr<OpenGLShaderProgram> shaderProgram;
        String name = shaderNames[s];
        cout << "Linking " << name << "... ";
        bool success = linkShaderProgram(shaderProgram, name);
        if (success)
        {
            // tries to insert it, move because it is a unique_ptr
            shaderPrograms.try_emplace(name, move(shaderProgram));
            cout << " with success." << endl;
            at_least_one_shaderProgram_works = true;
        }
        else cout << " failed." << endl;
    }

    return at_least_one_shaderProgram_works;
}

bool OpenGLComponent::linkShaderProgram(unique_ptr<OpenGLShaderProgram>& shaderProgram, const String& shaderName)
{
    String vertexShader = readFile(shaderFolder + "vertexShader.glsl"); // always the same vertex Shader;
    String fragmentShader = readFile(shaderFolder + shaderName + ".glsl");

    // Create an instance of OpenGLShaderProgram
    shaderProgram = make_unique<OpenGLShaderProgram>(openGLContext);
    
    // Compile and link the shader.
    // Each of these methods will return false if something goes wrong so we'll
    // wrap them in an if statement
    if (shaderProgram->addVertexShader(vertexShader)
        && shaderProgram->addFragmentShader(fragmentShader)
        && shaderProgram->link())
    {
        // No compilation errors - set the shader program to be active
        shaderProgram->use();

         // 4. uniform variables initialization
        // ___________________________________

        startTime = timeTracker.getMillisecondCounterHiRes() * 0.001f;
        shaderProgram->setUniform("playTime", this->playTime);
        shaderProgram->setUniform("resolution", WIDTH, HEIGHT);
        shaderProgram->setUniform("lowPos", lowPos);
        shaderProgram->setUniform("midPos", midPos);
        shaderProgram->setUniform("highPos", highPos);
        shaderProgram->setUniform("peak", bufferSpecs[0]);
        shaderProgram->setUniform("RMS", bufferSpecs[1]);
        shaderProgram->setUniform("lowsAmp", bufferSpecs[2]);
        shaderProgram->setUniform("midsAmp", bufferSpecs[3]);
        shaderProgram->setUniform("highsAmp", bufferSpecs[4]);

        return true;
    }
    else
    {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        jassertfalse;

        shaderProgram.reset();
        return false;
    }
}

void OpenGLComponent::updateAndUseShaderProgram(OpenGLShaderProgram* shaderProgram)
{
    // Use the shader program.
    if (shaderProgram == nullptr) return;
    shaderProgram->use();

    playTime = timeTracker.getMillisecondCounterHiRes() * 0.001f - startTime;
    shaderProgram->setUniform("playTime", this->playTime);

    shaderProgram->setUniform("resolution", WIDTH, HEIGHT);

    updatebufferSpecs();
    shaderProgram->setUniform("lowPos", lowPos);
    shaderProgram->setUniform("midPos", midPos);
    shaderProgram->setUniform("highPos", highPos);
    shaderProgram->setUniform("peak", bufferSpecs[0]);
    shaderProgram->setUniform("RMS", bufferSpecs[1]);
    shaderProgram->setUniform("lowsAmp", bufferSpecs[2]);
    shaderProgram->setUniform("midsAmp", bufferSpecs[3]);
    shaderProgram->setUniform("highsAmp", bufferSpecs[4]);
}

bool OpenGLComponent::setCurrentShader(String shaderName)
{
    const auto it = shaderPrograms.find(shaderName);
    // .find() rather than [] to make sure no default value is created
     
    if (it == shaderPrograms.end())
    {
        cout << "Not assigned." << endl;
        return false;
    }

    cout << "Using " << shaderName << "." << endl;
    currentShader = it->second.get();
    return true;
}
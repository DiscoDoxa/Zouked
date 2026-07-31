#version 330 core
            
// The value that was output by the vertex shader.
// This MUST have the exact same name that we used in the vertex shader.

// IN -----------
in vec4 fragColor;

// OUT -----------
out vec4 fragShaded;

uniform float playTime;
uniform float peak;
uniform float RMS;
uniform float firstFreq;
uniform float secondFreq;
uniform float thirdFreq;

void main()
{
	// Set the fragment's colour to the attribute passed in by the
	// vertex shader.
	fragShaded = sin(playTime*peak)*fragColor;
}

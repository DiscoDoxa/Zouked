#version 330 core
            
// The value that was output by the vertex shader.
// This MUST have the exact same name that we used in the vertex shader.

// IN -----------
in vec4 fragColor;

// OUT -----------
out vec4 fragShaded;

uniform float playTime;

void main()
{
	// Set the fragment's colour to the attribute passed in by the
	// vertex shader.
	fragShaded = sin(playTime)*fragColor;
}

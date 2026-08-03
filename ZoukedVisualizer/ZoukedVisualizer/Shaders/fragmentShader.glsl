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
	fragShaded = vec4(0,0,0,1);
	float n1 = abs(1 - log(firstFreq)/log(20000));
	float n2 = abs(1 - log(secondFreq)/log(20000));
	float n3 = abs(1 - log(thirdFreq)/log(20000));
	
	fragShaded = (3*fragShaded + vec4(n1, n2, n3, 1))/4;
}

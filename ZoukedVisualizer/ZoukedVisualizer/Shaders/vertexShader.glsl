#version 330 core
            
// Input attributes.
in vec4 position;      
in vec4 sourceColor;

// Output to fragment shader.
out vec2 fragCoord;
out vec4 fragColor;

void main()
{

	// Set the position to the attribute we passed in.
	gl_Position = position;
	fragCoord = position.xy;

	// Set the frag colour to the attribute we passed in.
	// This value will be interpolated for us before the fragment
	// shader so the colours we defined ateach vertex will create a
	// gradient across the shape.
	fragColor = sourceColor;

}

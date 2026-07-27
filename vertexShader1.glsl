#version 330 core

// IN --------------------------------
layout (location = 0) in vec3 aPos; // Input (vec3)

// OUT -----------
// out vec2 fragCoord;

void main()
{

   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); // Output (vec4)

}; // Most simple vertex shader that does nothing

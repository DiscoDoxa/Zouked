#version 330 core

// IN --------------------------------
layout (location = 0) in vec2 aPos; // Input (vec3)

// OUT -----------
out vec2 fragCoord;

void main()
{

   fragCoord = vec2(aPos.x, aPos.y); // Output (vec4)
   gl_Position = vec4(aPos, 0.0, 1.0);

}; // Most simple vertex shader that does nothing

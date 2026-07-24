#version 330 core
layout (location = 0) in vec3 aPos; // Input (vec3)
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); // Output (vec4)
}; // Most simple vertex shader that does nothing

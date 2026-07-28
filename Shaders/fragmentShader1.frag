#version 330 core

// IN -----------
in vec2 fragCoord;

// OUT -----------
out vec4 fragColor;

// UNIFORM --------------
uniform vec4 ourColor;
uniform float playTime;

void main()
{
    fragColor = sin(playTime)*ourColor;
}
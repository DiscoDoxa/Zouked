#version 330 core

// IN -----------
in vec2 fragCoord;

// OUT -----------
out vec4 fragColor;

// UNIFORM --------------
uniform vec4 ourColor;

uniform vec3 iResolution;
uniform float playTime;

// uniform float iTimeDelta;
// uniform float iFrame;
// uniform float iChannelTime[4];
// uniform vec4 iMouse;
// uniform vec4 iDate;
// uniform float iSampleRate;
// uniform vec3 iChannelResolution[4];
// uniform samplerXX iChanneli;


void main()
{
    fragColor = ourColor;
}
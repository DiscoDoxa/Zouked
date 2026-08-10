#version 330 core

// IN ----------------
in vec4 fragColor;
in vec2 fragCoord;

// OUT ----------------
out vec4 fragShaded;

// UNIFORM ------------
uniform vec2 resolution;
uniform float playTime;
uniform float lowPos;
uniform float midPos;
uniform float highPos;
uniform float peak;
uniform float RMS;
uniform float lowsAmp;
uniform float midsAmp;
uniform float highsAmp;

void main()
{
   vec2 uv = fragCoord.xy;

   fragShaded = vec4(0.5 + 0.5*cos(playTime+uv.xyx+vec3(0,2,4)), 1.0);
}


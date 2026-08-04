#version 330 core

// IN ----------------
in vec4 fragColor;
in vec2 fragCoord;

// OUT ----------------
out vec4 fragShaded;

// UNIFORM ------------
uniform vec2 resolution;
uniform float playTime;
uniform float peak;
uniform float RMS;
uniform float lowsAmp;
uniform float midsAmp;
uniform float highsAmp;

void main()
{
	float n1 = lowsAmp;
	float n2 = midsAmp;
	float n3 = highsAmp;
	
	// From NSC to pixels
	vec2 uv = fragCoord.xy;
	uv.y *= resolution.y/resolution.x;
	
	if(mod(length(uv)*100,20) <= 2)
	{
		fragShaded = vec4(0,1,0,1);
	}
	else
	{
		fragShaded = vec4(1,1,1,1);
	}
}

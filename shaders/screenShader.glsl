-- VERTEX
#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoords;

out vec2 tex;

void main()
{
	tex = texCoords;
	gl_Position = vec4(pos, 0.0, 1.0);
}

-- FRAGMENT

//fragment shader adds a slight hue and some film grain

#version 330 core
out vec4 fragColor;
in vec2 tex;

uniform sampler2D screenTexture;
uniform float time;

float rand(vec2 co) 
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec2 center = vec2(0.5, 0.5);
	float dist = distance(tex, center);

	vec4 color = texture(screenTexture, tex);
	//aberration

	float redOffset = 0.06 * sin(time * 0.5) * dist * 2;
	float greenOffset = 0.04 * sin(time * 0.5) * dist * 2;
	float blueOffset = -0.04 * sin(time * 0.5) * dist * 2;
	vec2 texSize = textureSize(screenTexture, 0).xy;
	vec2 direction = tex - center;
	color.r = texture(screenTexture, tex + (direction * vec2(redOffset, 0))).r;
	color.g = texture(screenTexture, tex + (direction * vec2(greenOffset, 0))).g;
	color.b = texture(screenTexture, tex + (direction * vec2(blueOffset, 0))).b;
	
	vec3 tint = vec3(0.9+sin(time * 0.1) * 0.2, 0.9-sin(time * 0.1) * 0.2, 0.9);
	color.rgb *= tint;

	//film grain using time for randomness
	float grain = rand(tex * gl_FragCoord.xy + time) * 0.05;
	color.rgb += grain;

	//darken corners
	float radius = 0.5 + 0.05 * sin(time * 0.5);
	float edge = 0.3;
	float vignette = smoothstep(radius, radius+edge, dist);
	color.rgb *= 1.0 - 0.9 * vignette;
	
	color = round(color * 8.0) / 8.0;
	fragColor = color;
}

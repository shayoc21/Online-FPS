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
#version 330 core
out vec4 fragColor;
in vec2 tex;

uniform sampler2D screenTexture;

void main()
{
	vec4 color = texture(screenTexture, tex);

	vec3 purple = vec3(0.8, 0.0, 0.8);

	color.rgb *= purple;

	fragColor = color;

}

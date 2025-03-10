-- VERTEX
#version 330 core

layout (location=0) in vec3 pos;

out vec4 vertexColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(pos, 1.0f);
	vertexColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);
}

-- FRAGMENT
#version 330 core

out vec4 fragColor;

in vec4 vertexColor;

void main()
{
	fragColor = vertexColor;
}


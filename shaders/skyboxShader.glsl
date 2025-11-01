-- VERTEX
#version 330 core

layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = position;
	vec4 pos = projection * view * vec4(position, 1.0);
	gl_Position = pos.xyww;
}
-- FRAGMENT
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
}

-- VERTEX
#version 330 core

layout (location = 0) in vec3 pos;

void main()
{
	gl_Position = vec4(pos.x,pos.y,pos.z, 1.0);
}

-- FRAGMENT
#version 330 core

out vec4 fC;

void main()
{
	fC = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

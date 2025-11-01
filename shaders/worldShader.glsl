-- VERTEX
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;
layout (location = 2) in vec3 normal;

out vec2 texCoord;
out vec3 fNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);
  texCoord = textureCoord; 
  fNormal = mat3(model) * normal;
  fragPos = vec3(model * vec4(position, 1.0f));
}

-- FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 fNormal;
in vec3 fragPos;
uniform sampler2D tex;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
	float ambientStrength = 0.25;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(fNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * texture(tex, texCoord).xyz;
	FragColor = vec4(result, 1.0);	

}

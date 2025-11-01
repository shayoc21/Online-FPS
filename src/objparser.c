#define STB_IMAGE_IMPLEMENTATION
#include "objparser.h"

Mesh loadObj(const char* objPath)
{
	//currently limited to 10000 vertices
	//all vertices must be of format vvv tt nnn

	vec3 p[100000];
	uv t[100000];
	vec3 n[100000];
	int positionCounter = 0;
	int texCounter = 0;
	int normalCounter = 0;
	
	Vertex vertices[100000];
	int vertexCounter = 0;
	unsigned int indices[100000];
	int indexCounter = 0;
	
	FILE* objFile = fopen(objPath, "r");
	if (!objFile)
	{
		printf("Failed to open file: %s\n", objPath);
		return (Mesh) {0, 0};
	}

	char line[128];
	while(fgets(line, sizeof(line), objFile))
	{
		if (strncmp(line, "v ", 2) == 0)
		{
			sscanf(line, "v %f %f %f", &p[positionCounter].x, &p[positionCounter].y, &p[positionCounter].z);
			positionCounter++;
		}
		else if (strncmp(line, "vt ", 3) == 0)
		{
			sscanf(line, "vt %f %f", &t[texCounter].u, &t[texCounter].v);
			texCounter++;
		}
		else if (strncmp(line, "vn ", 3) == 0)
		{
			sscanf(line, "vn %f %f %f", &n[normalCounter].x, &n[normalCounter].y, &n[normalCounter].z);
			normalCounter++;
		}
		else if (strncmp(line, "f ", 2) == 0)
		{
			int positionIndex[3], uvIndex[3], normalIndex[3];
			sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
				&positionIndex[0], &uvIndex[0], &normalIndex[0],
				&positionIndex[1], &uvIndex[1], &normalIndex[1],
				&positionIndex[2], &uvIndex[2], &normalIndex[2]);
			for (int i = 0; i < 3; i++)
			{
				Vertex faceVertex;
				faceVertex.position = p[positionIndex[i]-1];
				faceVertex.texCoords = t[uvIndex[i]-1];
				faceVertex.normal = n[normalIndex[i]-1];
				vertices[vertexCounter] = faceVertex;
				indices[indexCounter] = positionIndex[i];
				vertexCounter++;
				indexCounter++;
			}
		}
	}
	fclose(objFile);
	
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCounter*sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCounter*sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return (Mesh) {VAO, vertexCounter};
}
			          
GLuint loadTexture(const char* texturePath)
{
	GLuint TEX;
	glGenTextures(1, &TEX);
	glBindTexture(GL_TEXTURE_2D, TEX);

	int w, h, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(texturePath, &w, &h, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture %s\n", texturePath);
	}
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	return TEX;
}

GLuint loadCubemap(const char* cubemapPrefix)
{
	GLuint TEX;
	glGenTextures(1, &TEX);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TEX);

	char texturePath[256];
	int w, h, nrChannels;
	stbi_set_flip_vertically_on_load(0);
	for (int i = 0; i < 6; i++)
	{
		snprintf(texturePath, sizeof(texturePath), "%s%d.jpg", cubemapPrefix, i+1);
		unsigned char* data = stbi_load(texturePath, &w, &h, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			printf("Failed to load texture %s\n", texturePath);
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return TEX;
}

GLuint loadShader(const char* shaderPath)
{
	GLuint SHADER;
	SHADER = glCreateProgram();

	shaderSource shaders = {0};
	parseShaders(&shaders, shaderPath);

	int compileResult;
	char infoLog[512];

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (char const* const*)&shaders.vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileResult);
	if (!compileResult)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile shader <vertex>; %s\n", infoLog);
	}
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (char const* const*)&shaders.fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileResult);
	if (!compileResult)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Failed to compile shader <fragment>; %s\n", infoLog);
	}
	glAttachShader(SHADER, vertexShader);
	glAttachShader(SHADER, fragmentShader);
	glLinkProgram(SHADER);
	glGetProgramiv(SHADER, GL_LINK_STATUS, &compileResult);
	if (!compileResult)
	{
		glGetProgramInfoLog(SHADER, 512, NULL, infoLog);
		printf("Failed to link shader; %s\n", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	destroyShaderSource(&shaders);
	return SHADER;
}

#ifndef _OBJPARSER
#define _OBJPARSER

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mathlibrary/maths.h"
#include "../dependencies/stb/stb_image.h"
#include "../shaders/shader.h"

struct Mesh
{
	GLuint VAO;
	GLuint vertexCount;
};
typedef struct Mesh Mesh;

struct uv
{
	float u, v;
};
typedef struct uv uv;

struct Vertex
{
	vec3 position;
	uv texCoords;
	vec3 normal;
};
typedef struct Vertex Vertex;

Mesh loadObj(const char* objPath);
GLuint loadTexture(const char* texturePath);
GLuint loadCubemap(const char* cubemapPrefix);
GLuint loadShader(const char* shaderPath);

#endif 

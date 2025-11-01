#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vec3
{
	float x, y, z;
};
typedef struct vec3 vec3;

struct uv
{
	float u, v;
};
typedef struct uv uv;

struct Vertex
{
	vec3 position;
	uv tex;
	vec3 normal;
};
typedef struct Vertex Vertex;


void loadOBJ(const char* objPath)
{
	vec3 p[10000];
	uv t[10000];
	vec3 n[10000];
	int positionCounter = 0;
	int texCounter = 0;
	int normalCounter = 0;
	
	Vertex vertices[10000];
	int vertexCounter = 0;
	unsigned int indices[10000];
	int indexCounter = 0;
	
	FILE* objFile = fopen(objPath, "r");
	if (!objFile)
	{
		printf("Failed to open file: %s\n", objPath);
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
				faceVertex.tex = t[uvIndex[i]-1];
				faceVertex.normal = n[normalIndex[i]-1];
				vertices[vertexCounter] = faceVertex;
				indices[indexCounter] = positionIndex[i]-1;
				vertexCounter++;
				indexCounter++;
			}
		}
	}
}

int main(int argc, char** argv)
{
	loadOBJ("cube.obj");
}

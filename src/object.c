#include "object.h"

void bindUniformMat4(Object* object, const char* uniformName, mat4 matrix)
{
	int matrixLocation = glGetUniformLocation(object->SHADER, uniformName);
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, matrix.m);
}

mat4 generateModelMatrix(vec3 position, quaternion rotation, vec3 scale)
{
	mat4 rot = rotationMat4(rotation);
	mat4 scalem = scaleMat4(scale);
	mat4 translate = translationMat4(position);

	return multiplyMat4(multiplyMat4(translate, rot), scalem);
}

void duplicateObject(Object* object, Object* objectTemplate, vec3 position, vec3 scale, quaternion rotation)
{
	//more memory efficient way of creating multiple objects of the same type
	//if object and objectTemplate are the same pointer it will just update the world attributes of that object
	//acts to avoid duplicate vaos, textures, shaders
	object->position = position;
	object->scale = scale;
	object->rotation = rotation;
	object->modelMatrix = generateModelMatrix(position, rotation, scale);

	object->VAO = objectTemplate->VAO;
	object->vertexCount = objectTemplate->vertexCount;
	object->TEXTURE = objectTemplate->TEXTURE;
	object->SHADER = objectTemplate->SHADER;
}
void initObject
(
	Object* object,
	vec3 position,
	vec3 scale, 
	quaternion rotation,
	const char* objectFile,
	const char* textureFile,
	int solid
)
{
	object->position = position;
	object->scale = scale;
	object->rotation = rotation;
	object->modelMatrix = generateModelMatrix(position, rotation, scale);

	Mesh objectMesh = loadObj(objectFile);
	object->VAO = objectMesh.VAO;
	object->vertexCount = objectMesh.vertexCount;

	object->TEXTURE = loadTexture(textureFile);
	object->SHADER = loadShader("./shaders/worldShader.glsl");
	
	if (solid)
	{
		char* objectSuffix = malloc(strlen(objectFile) - 8);
		//worst line of code ever. assumes all objs are in ./models/ directory.
		memmove(objectSuffix, objectFile + 9, strlen(objectFile) - 8);

		BSPNode* cMesh = generateCollisionMesh(objectSuffix);
		object->collisionMesh = cMesh;
		transformBSPTree(object->collisionMesh, object->modelMatrix);
		int tc = countFacesInTree(object->collisionMesh);
		printf(" for object %s, collision face count %d\n", objectSuffix, tc);
		object->solid = 1;
		free(objectSuffix);
	}
	else
	{
		object->collisionMesh = NULL;
		object->solid = 0;
	}
}

void initSkybox(Object* object, const char* texturePrefix)
{
	//the skybox doesnt use a model matrix, so that data is left NULL
	//i really hate c++ so much, this is my best effort at polymorphism in C
	
	object->vertexCount = 36;
	glGenVertexArrays(1, &object->VAO);
	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(object->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	object->TEXTURE = loadCubemap(texturePrefix);
	object->SHADER = loadShader("./shaders/skyboxShader.glsl");
}

void drawObject(Object* object)
{
	glDepthMask(GL_TRUE);

	glUseProgram(object->SHADER);
	glBindVertexArray(object->VAO);
	glBindTexture(GL_TEXTURE_2D, object->TEXTURE);
	
	bindUniformMat4(object, "model", object->modelMatrix);
	bindUniformMat4(object, "view", currentCamera->view);
	bindUniformMat4(object, "projection", currentCamera->projection);
	
	float lightDir[3] = {2000, -400, 3000};
	float lightColor[3] = {1.0, 1.0, 1.0};

	glUniform3fv(glGetUniformLocation(object->SHADER, "lightPos"), 1, lightDir);
	glUniform3fv(glGetUniformLocation(object->SHADER, "lightColor"), 1, lightColor);
	
	glDrawArrays(GL_TRIANGLES, 0, object->vertexCount);
	

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

mat4 clipMat4(mat4 matrix)
{
	matrix.m[3] = 0;
	matrix.m[7] = 0;
	matrix.m[11] = 0;
	matrix.m[12] = 0;
	matrix.m[13] = 0;
	matrix.m[14] = 0;
	matrix.m[15] = 0;
	return matrix;
}

void drawSkybox(Object* object)
{
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	
	glUseProgram(object->SHADER);
	glBindVertexArray(object->VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, object->TEXTURE);

	//remove transformation from view matrix;
	mat4 view = clipMat4(currentCamera->view);
	bindUniformMat4(object, "view", view);
	bindUniformMat4(object, "projection", currentCamera->projection);
	
	glDrawArrays(GL_TRIANGLES, 0, object->vertexCount);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void destroyObject(Object* object)
{
	destroyBSPTree(object->collisionMesh);
	return;
}

int isObjectSolid(Object* object)
{
	return object->solid;
}



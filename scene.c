#include "scene.h"

void addObjectToScene(Scene* scene, Object* object)
{
	Object* copiedObject = (Object*)malloc(sizeof(Object));
	*copiedObject = *object;

	scene->worldObjectCount++;
	scene->worldObjects = (Object**)realloc(scene->worldObjects, scene->worldObjectCount * sizeof(Object*));
	scene->worldObjects[scene->worldObjectCount-1] = copiedObject;
}

void initWorldmap(Scene* scene)
{
	Object worldObject;
	printf("		...Initializing Objects\n");
	initObject(&worldObject, vector(100.0f, 100.0f, 0.0f), vector(8.0f, 8.0f, 8.0f), quat(vector(0.0, 1.0, 0.0), 0.0), "./models/dust.obj", "./textures/dust.png", 1);
	addObjectToScene(scene, &worldObject);
	printf("			...Done\n");
	printf("		...Initializing Skybox\n");
	initSkybox(&scene->skybox, "./textures/skybox/day");
	printf("			...Done\n");
}

void initScene(Scene* scene)
{
	printf("	...Initializing Scene\n");
	scene->worldObjects = (Object**)malloc(sizeof(Object*));
	scene->worldObjectCount = 0;

	initWorldmap(scene);
	printf("		...Done\n");
}

void drawScene(Scene* scene)
{
	drawSkybox(&scene->skybox);
	for (int i = 0; i < scene->worldObjectCount; i++)
	{
		drawObject(scene->worldObjects[i]);
	}
}

void destroyScene(Scene* scene)
{
	for (int i = 0; i < scene->worldObjectCount; i++)
	{
		destroyObject(scene->worldObjects[i]);
		free(scene->worldObjects[i]);
	}
	destroyObject(&scene->skybox);
	free(scene->worldObjects);
}

vec3 triangleCentroid(Face face)
{
	vec3 sum = addVec3(addVec3(face.a, face.b), face.c);
	return scaleVec3(sum, 1.0f/3.0f);
}

int calculateSceneCollision(Scene* scene, vec3 origin, float radius)
{
	


float castRayThroughScene(Scene* scene, Ray ray)
{
	Collision shortestRayToFace = { 0, FLT_MAX, {0, 0}};
	for (int i = 0; i < scene->worldObjectCount; i++)
	{
		Object* obj = scene->worldObjects[i];
		if (obj->solid)
		{
			for (int j = 0; j < obj->collisionFaceCount; j++)
			{
				Face face = obj->collisionFaces[j];
				Collision rayToFace = mollerTrumboreRaycast(ray, face);
				if (rayToFace.status)
				{
					if (rayToFace.t < shortestRayToFace.t)
					{
						shortestRayToFace = rayToFace;
					}
				}
			}
		}
	}
	return shortestRayToFace.t;
}
		

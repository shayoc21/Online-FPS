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

	initObject(&worldObject, vector(100.0f, 100.0f, 0.0f), vector(1.0f, 1.0f, 1.0f), quat(vector(0.0, 1.0, 0.0), 0.0), "./models/castle.obj", "./textures/wall.png", 1);
	addObjectToScene(scene, &worldObject);
//	initObject(&worldObject, vector(100.0f, 100.0f, 0.0f), vector(8.0f, 8.0f, 8.0f), quat(vector(0.0, 1.0, 0.0), 0.0), "./models/dust.obj", "./textures/wall.png", 1);
//	addObjectToScene(scene, &worldObject);
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

float getShortestSceneCollision(Scene* scene, Ray ray)
{
	//this is assumed.
	vec3 playerPosition = ray.origin;
	float shortestDistance = FLT_MAX;

	//traverses the BSP collision tree of all solid objects in scene. raycasts through all scenes in the players leaf node.
	for (int i = 0; i < scene->worldObjectCount; i++)
	{
		Object* object = scene->worldObjects[i];
		if (object->solid == 0)
		{
			continue;
		}

		//else traverse BSP

		Face* playerFaces;
		int playerFaceCount = 0;
		traverseBSP(object->collisionMesh, playerPosition, &playerFaces, &playerFaceCount);
		for (int j = 0; j < playerFaceCount; j++)
		{
			Face f = playerFaces[j];
			/*printf("Face: %f %f %f\n"
				   "	  %f %f %f\n"
				   "	  %f %f %f\n", 
				   f.a.x,f.a.y,f.a.z,
				   f.b.x,f.b.y,f.b.z,
				   f.c.x,f.c.y,f.c.z);*/
			Collision faceCollision;
			faceCollision = mollerTrumboreRaycast(ray, playerFaces[j]);
			if (faceCollision.status && faceCollision.t < shortestDistance)
			{
				shortestDistance = faceCollision.t;
			}
		}
		free(playerFaces);
	}
	return shortestDistance;
}

	

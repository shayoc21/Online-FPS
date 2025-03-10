#ifndef _SCENE
#define _SCENE

#include "object.h"
#include "mathlibrary/maths.h"
#include "raycast.h"

struct Scene
{
	Object** worldObjects;
	int worldObjectCount;
	Object skybox;
};
typedef struct Scene Scene;

void initScene(Scene* scene);
void drawScene(Scene* scene);
void destroyScene(Scene* scene);
float getShortestSceneCollision(Scene* scene, Ray ray);

#endif

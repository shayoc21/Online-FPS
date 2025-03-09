#ifndef _RAYCAST
#define _RAYCAST

#include "mathlibrary/maths.h"
#include "objparser.h"

#include <math.h>

struct Collision
{
	int status;
	float t;
	vec2 barycentricCoordinates;
};
typedef struct Collision Collision;

struct Ray
{
	vec3 ray;
	vec3 origin;
};
typedef struct Ray Ray;

Collision mollerTrumboreRaycast(Ray ray, Face face);

#endif

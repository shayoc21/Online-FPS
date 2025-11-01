#ifndef _RAYCAST
#define _RAYCAST

#include "mathlibrary/maths.h"
#include "objparser.h"

#include <math.h>

struct Face
{
	vec3 a;
	vec3 b;
	vec3 c;
};
typedef struct Face Face;

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

struct Plane
{
	vec3 normal;
	float distance;
};
typedef struct Plane Plane;

struct PlaneCollision
{
	int status;
	float t;
	vec3 worldPosition;
};
typedef struct PlaneCollision PlaneCollision;

Collision mollerTrumboreRaycast(Ray ray, Face face);
PlaneCollision rayPlaneIntersection(Ray ray, Plane plane);

#endif

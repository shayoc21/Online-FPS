#ifndef _COLLISION
#define _COLLISION

#include "mathlibrary/maths.h"
#include "objparser.h"

struct Face
{
	vec3 a;
	vec3 b;
	vec3 c;
};
typedef struct Face Face;

struct Plane
{
	vec3 normal;
	float distance;
};
typedef struct Plane Plane;

struct BSPNode
{
	Plane splittingPlane;
	BSPNode* front;
	BSPNode* back;

	Face* faces;
	int faceCount;
};
typedef struct BSPNode BSPNode;

BSPNode* createCollisionMesh(const char* collisionFileSuffix);

#endif

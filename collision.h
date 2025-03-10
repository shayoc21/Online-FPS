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
	struct BSPNode* front;
	struct BSPNode* back;

	Face* faces;
	int faceCount;
};
typedef struct BSPNode BSPNode;

BSPNode* generateCollisionMesh(const char* collisionFileSuffix);
void transformBSPTree(BSPNode* node, mat4 modelMatrix);
void traverseBSP(BSPNode* node, vec3 position, Face** faceBuffer, int* faceCountBuffer);
void destroyBSPTree(BSPNode* node);
int countFacesInTree(BSPNode* node);

#endif

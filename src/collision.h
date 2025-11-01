#ifndef _COLLISION
#define _COLLISION

#include "mathlibrary/maths.h"
#include "objparser.h"
#include "raycast.h"

struct lineSeg
{
	vec3 a;
	vec3 b;
};
typedef struct lineSeg lineSeg;

struct BSPNode
{
	Plane splittingPlane;
	struct BSPNode* front;
	struct BSPNode* back;

	Face* faces;
	int faceCount;
};
typedef struct BSPNode BSPNode;

struct FaceCollision
{
	Face collidedFace;
	float distance;
	vec2 barycentricCoordinates;
	int didCollide;
};
typedef struct FaceCollision FaceCollision;

BSPNode* generateCollisionMesh(const char* collisionFileSuffix);
void transformBSPTree(BSPNode* node, mat4 modelMatrix);
void traverseBSP(BSPNode* node, vec3 position, Face** faceBuffer, int* faceCountBuffer);
void getCollisionFacesBSP(BSPNode* node, lineSeg colseg, Face** faceBuffer, int* faceCountBuffer);
void raycastBSP(BSPNode* node, Ray ray, Face** faceBuffer, int* faceCountBuffer);
void destroyBSPTree(BSPNode* node);
int countFacesInTree(BSPNode* node);
void FUCKTHISSHIT();

#endif

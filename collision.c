#include "collision.h"

Plane selectPlane(faces, faceCount)
{
	int middleFaceIndex = (faceCount-1)/2;
	Face planeFace = faces[middleFaceIndex];

	vec3 e1 = subtractVec3(planeFace.b - planeFace.a);
	vec3 e2 = subtractVec3(planeFace.c - planeFace.a);

	vec3 normal = normalizeVec3(crossVec3(e1, e2));

	float d = -(normal.x * planeFace.a.x + normal.y * planeFace.a.y + normal.z * planeFace.a.z);

	Plane plane;
	plane.normal = normal;
	plane.distance = d;
	return plane;
}

void partitionFaces(Face* faces, int faceCount, Plane plane, Face** frontFaces, Face** backFaces, int* frontCount, int* backCount)
{
	*frontCount = 0;
	*backCount = 0;

	float dot[faceCount];
	
	//count faces and calculate the dot product
	for ( int i = 0; i < faceCount; i++ )
	{
		dot[i] = getVec3Dot(faces[i].a, plane.normal);
		if (dot >= 0)
		{
			(*frontCount)++;
		}
		else
		{
			(*backCount)++;
		}
	}
	
	//allocate memory and store faces
	*frontFaces = (Face*)malloc(frontCount * sizeof(Face));
	*backFaces = (Face*)malloc(backCount * sizeof(Face));

	int fIndex = 0;
	int bIndex = 0;

	for ( int i = 0; i < faceCount; i++ )
	{
		float d = dot[i];
		if (d >= 0)
		{
			(*frontFaces)[fIndex++] = faces[i];
		}
		else
		{
			(*backFaces)[bIndex++] = faces[i];
		}
	}
}

BSPNode* generateBSPTree(Face* faces, int faceCount)
{
	//base case
	if (faceCount == 0) return NULL;

	BSPNode* node = (BSPNode*) malloc(sizeof(BSPNode));
	
	node->splittingPlane = selectPlane(faces, faceCount);
	Face* frontFaces;
	Face* backFaces;
	int frontCount, backCount;
	partitionFaces(faces, faceCount, node->splittingPlane, &frontFaces, &backFaces, &frontCount, &backCount);

	//is a leaf node
	if (frontCount == 0 && backCount == 0)
	{
		node->faces = faces;
		node->faceCount = faceCount;
		node->front = NULL;
		node->back = NULL;
		return node;
	}

	node->front = generateBSP(frontFaces, frontCount);
	node->back = generateBSP(backFaces, backCount);

	//is not a leaf node, and so has no faces
	node->faces = NULL;
	node->faceCount = 0;

	free(frontFaces);
	free(backFaces);

	return node;
}

BSPNode* createCollisionMesh(const char* collisionFileSuffix)
{
	Face* faces = (Face*)malloc(0);
	int faceCount = 0;
	vec3 vertices[100000];
	int vc = 0;

	char buffer[120];
	snprintf(buffer, sizeof(buffer), "%s%s", "./models/collision", fileSuffix);
	FILE* collisionFile = fopen(buffer, "r");
	if (!collisionFile)
	{
		printf("AAFailed to open file: %s\n", fileSuffix);
		return (CollisionMesh) { 0, 0};
	}

	char line[128];
	while(fgets(line, sizeof(line), collisionFile))
	{
		if (strncmp(line, "v ", 2) == 0)
		{
			sscanf(line, "v %f %f %f", &vertices[vc].x, &vertices[vc].y, &vertices[vc].z);
			vc++;
		}
		else if (strncmp(line, "f ", 2) == 0)
		{
			Face face = {0};
			int positions[3];
			sscanf(line, "f %d %d %d", &positions[0], &positions[1], &positions[2]);
			face.a = vertices[positions[0]-1];
			face.b = vertices[positions[1]-1];
			face.c = vertices[positions[2]-1];
			faceCount++;
			faces = (Face*)realloc(faces, sizeof(Face)*faceCount);
			faces[faceCount-1] = face;
		}
	}
	fclose(collisionFile);

	BSPNode* rootNode = generateBSPTree(faces, faceCount);
}

void transformFace(Face* face, mat4 matrix)
{		
	face.a = transformVec3(face.a, matrix);
	face.b = transformVec3(face.b, matrix);
	face.c = transformVec3(face.c, matrix);
}

void transformBSPTree(BSPNode* node, mat4 modelMatrix)
{
	if (node == NULL) return;

	for (int i = 0; i < node->faceCount; i++)
	{
		transformFace(&node->faces[i], modelMatrix);
	}

	transformBSPTree(node->left, modelMatrix);
	transformBSPTree(node->right, modelMatrix);
}


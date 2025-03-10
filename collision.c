#include "collision.h"


//Select plane for now just takes the middle face in faces, will add heuristics in my games optimization stage.

Plane selectPlane(Face* faces, int faceCount)
{
	int middleFaceIndex = (faceCount-1)/2;
	Face planeFace = faces[middleFaceIndex];

	vec3 e1 = subtractVec3(planeFace.b, planeFace.a);
	vec3 e2 = subtractVec3(planeFace.c, planeFace.a);

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

	*frontFaces = malloc(faceCount * sizeof(Face));
	*backFaces = malloc(faceCount * sizeof(Face));

	for (int i = 0; i < faceCount; i++)
	{
		Face face = faces[i];
		
		float da = plane.normal.x*face.a.x+plane.normal.y*face.a.y+plane.normal.z*face.a.z+plane.distance;
		float db = plane.normal.x*face.b.x+plane.normal.y*face.b.y+plane.normal.z*face.b.z+plane.distance;
		float dc = plane.normal.x*face.c.x+plane.normal.y*face.c.y+plane.normal.z*face.c.z+plane.distance;

		if (da >= 0 || db >= 0 || dc >= 0)
		{
			(*frontFaces)[(*frontCount)++] = face;
		}
		
		if (da < 0 || db < 0 || dc < 0)
		{
			(*backFaces)[(*backCount)++] = face;
		}
	}

	*frontFaces = realloc(*frontFaces, *frontCount * sizeof(Face));
	*backFaces = realloc(*backFaces, *backCount * sizeof(Face));
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
	if (frontCount == faceCount || backCount == faceCount)
	{
		node->faces = faces;
		node->faceCount = faceCount;
		node->front = NULL;
		node->back = NULL;
		free(frontFaces);
		free(backFaces);
		return node;
	}

	node->front = generateBSPTree(frontFaces, frontCount);
	node->back = generateBSPTree(backFaces, backCount);

	//is not a leaf node, and so has no faces
	node->faces = NULL;
	node->faceCount = 0;

	free(faces);

	return node;
}

BSPNode* generateCollisionMesh(const char* collisionFileSuffix)
{
	Face* faces = (Face*)malloc(0);
	int faceCount = 0;
	vec3 vertices[100000];
	int vc = 0;

	char buffer[120];
	snprintf(buffer, sizeof(buffer), "%s%s", "./models/collision", collisionFileSuffix);
	FILE* collisionFile = fopen(buffer, "r");
	if (!collisionFile)
	{
		printf("AAFailed to open file: %s\n", collisionFileSuffix);
		return NULL;
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
	
	printf("			...Generating BSP for Object %s\n", collisionFileSuffix);
	BSPNode* rootNode = generateBSPTree(faces, faceCount);
}

void transformFace(Face* face, mat4 matrix)
{		
	face->a = transformVec3(face->a, matrix);
	face->b = transformVec3(face->b, matrix);
	face->c = transformVec3(face->c, matrix);
}

void transformBSPTree(BSPNode* node, mat4 modelMatrix)
{
	if (node == NULL) return;
	
	for (int i = 0; i < node->faceCount; i++)
	{
		transformFace(&node->faces[i], modelMatrix);
	}

	transformBSPTree(node->front, modelMatrix);
	transformBSPTree(node->back, modelMatrix);
}

void traverseBSP(BSPNode* node, vec3 position, Face** faceBuffer, int* faceCountBuffer)
{
	if (!node) return;

	if (node->faceCount > 0)
	{
		//leaf node... allocate memory to faceBuffer and fill it
		*faceBuffer = (Face*)malloc(node->faceCount * sizeof(Face));
		for (int i = 0; i < node->faceCount; i++)
		{
			(*faceBuffer)[i] = node->faces[i];
		}
		*faceCountBuffer = node->faceCount;
		return;
	}

	Plane np = node->splittingPlane;
	float d = np.normal.x * position.x + np.normal.y * position.y + np.normal.z * position.z - np.distance;
	if (d >= 0)
	{
		traverseBSP(node->front, position, faceBuffer, faceCountBuffer);
	}
	else
	{
		traverseBSP(node->back, position, faceBuffer, faceCountBuffer);
	}
}

int countFacesInTree(BSPNode* node) {
    if (node == NULL) {
        return 0; // Base case: empty node
    }

    // Count faces in the current node
    int count = node->faceCount;

    // Recursively count faces in the front and back child nodes
    count += countFacesInTree(node->front);
    count += countFacesInTree(node->back);

    return count;
}

void destroyBSPTree(BSPNode* node)
{
	if (node == NULL) {
        return; // Base case: if the node is NULL, return
    }

    // Recursively destroy the front and back child nodes
    destroyBSPTree(node->front);
    destroyBSPTree(node->back);

    // Free the faces array in this node
    if (node->faces != NULL) {
        free(node->faces);
        node->faces = NULL; // Optional: set to NULL to avoid dangling pointers
    }

    // Free the current node
    free(node);
}




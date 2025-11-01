#include "collision.h"

float dot(vec3 a, vec3 b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

vec3 interpolate(vec3 a, vec3 b, float t)
{
	return (vec3)
	{
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y),
		a.z + t * (b.z - a.z)
	};
}

// returns whether or not the first face in out is front.
int splitTriangle(Face triangle, Plane plane, Face** out)
{

    float da = dot(triangle.a, plane.normal) + plane.distance;
    float db = dot(triangle.b, plane.normal) + plane.distance;
    float dc = dot(triangle.c, plane.normal) + plane.distance;

	if (da == 0 || db == 0 || dc == 0)
	{
		//printf("onplane\n");
	}

    int ina = da >= -0.002f, inb = db >= -0.002f, inc = dc >= -0.002f;
    int totalIn = ina + inb + inc;

	vec3 intersectionAB;
	vec3 intersectionBC;
	vec3 intersectionAC;
    int intersectionCount = 0;

    if (ina != inb) {
        float t = -da / (db - da);
        intersectionAB = interpolate(triangle.a, triangle.b, t);
		intersectionCount++;
    }
    if (ina != inc) {
		float t = -da / (dc - da);
		intersectionAC = interpolate(triangle.a, triangle.c, t);
		intersectionCount++;
	}
	if (inb != inc && intersectionCount < 2) {
		float t = -db / (dc - db);
		intersectionBC = interpolate(triangle.b, triangle.c, t);
	}

	if ((ina != inb && ina != inc) && (inb == inc)) 
	{
		//a is isolated by plane

		(*out)[0] = (Face) { triangle.a, intersectionAB, intersectionAC };
		(*out)[1] = (Face) { intersectionAB, triangle.b, triangle.c };
		(*out)[2] = (Face) { intersectionAC, intersectionAB, triangle.c };
		return ina;
	}
	else if ((inb != ina && inb != inc) && (ina == inc))
	{
		//b is isolated by plane

		(*out)[0] = (Face) { triangle.b, intersectionBC, intersectionAB };
		(*out)[1] = (Face) { intersectionBC, triangle.c, triangle.a };
		(*out)[2] = (Face) { intersectionAB, intersectionBC, triangle.a };
		return inb;
	}
	else if ((inc != ina && inc != inb) && (ina == inb))
	{	
		//c is isolated by plane 

		(*out)[0] = (Face) { triangle.c, intersectionAC, intersectionBC };
		(*out)[1] = (Face) { intersectionAC, triangle.a, triangle.b };
		(*out)[2] = (Face) { intersectionBC, intersectionAC, triangle.b };
		return inc;
	}
	return 0;
}

Plane selectPlane(Face* faces, int faceCount)
{
	printf("Selecting plane! faceCount : %d\n", faceCount);

	//heuristic finds the best splitting plane.
	float bestCost = FLT_MAX;
	Plane bestPlane = { {0,0,0}, 0 };
	int found = 0;
	const float epsilon = 0.002f;

	for (int i = 0; i < faceCount; i++)
	{
		Face candidateFace = faces[i];
		vec3 e1 = subtractVec3(candidateFace.b, candidateFace.a);
		vec3 e2 = subtractVec3(candidateFace.c, candidateFace.a);
		vec3 normal = normalizeVec3(crossVec3(e1, e2));
		float d = -dot(normal, candidateFace.a);
		Plane candidatePlane = { normal, d };

		int frontCount = 0, backCount = 0, splitCount = 0;

		for (int j = 0; j < faceCount; j++)
		{
			Face face = faces[j];
			float da = dot(face.a, candidatePlane.normal) + candidatePlane.distance;
			float db = dot(face.b, candidatePlane.normal) + candidatePlane.distance;
			float dc = dot(face.c, candidatePlane.normal) + candidatePlane.distance;
			
			int ina = (da >= -epsilon);
			int inb = (db >= -epsilon);
			int inc = (dc >= -epsilon);

			if (ina && inb && inc)
			{
				frontCount++;
			}
			else if (!ina && !inb && !inc)
			{
				backCount++;
			}
			else
			{
				splitCount++;
			}
		}

		if (frontCount == 0 || backCount == 0)
		{
			continue;
		}
		
		float cost = fabsf((float)frontCount - (float)backCount)+(float)splitCount * 10.0f;
		if (cost < bestCost)
		{
			bestCost = cost;
			bestPlane = candidatePlane;
			found = 1;
		}
	}

	if (!found)
	{
		// there is no splitting plane found. therefore the node is a leafnode and nullPlane is returned.
		return (Plane) { {0,0,0}, 0 };
	}
	printf("Done\n");
	return bestPlane;				
}
void partitionFaces(Face* faces, int faceCount, Plane plane, Face** frontFaces, Face** backFaces, int* frontCount, int* backCount)
{
	*frontCount = 0;
	*backCount = 0;

	*frontFaces = malloc(faceCount * 3 * sizeof(Face));
	*backFaces = malloc(faceCount * 3 * sizeof(Face));

	for (int i = 0; i < faceCount; i++)
	{
		Face face = faces[i];
		
		float da = plane.normal.x*face.a.x+plane.normal.y*face.a.y+plane.normal.z*face.a.z+plane.distance;
		float db = plane.normal.x*face.b.x+plane.normal.y*face.b.y+plane.normal.z*face.b.z+plane.distance;
		float dc = plane.normal.x*face.c.x+plane.normal.y*face.c.y+plane.normal.z*face.c.z+plane.distance;

		if (da >= -0.002f && db >= -0.002f && dc >= -0.002f)
		{
			//printf("A\n");
			(*frontFaces)[(*frontCount)++] = face;
		}
		
		else if (da <= 0.002f && db <= 0.002f && dc <= 0.002f)
		{
			//printf("B\n");
			(*backFaces)[(*backCount)++] = face;
		}

		else
		{
			//printf("C\n");
			//face is split by plane.
			Face* splitFaces = (Face*)malloc(sizeof(Face) * 3);
			int isSingleFaceFront = splitTriangle(face, plane, &splitFaces);
			if (isSingleFaceFront)
			{
				(*frontFaces)[(*frontCount)++] = splitFaces[0];
				(*backFaces)[(*backCount)++] = splitFaces[1];
				(*backFaces)[(*backCount)++] = splitFaces[2];
			}
			else
			{
				(*backFaces)[(*backCount)++] = splitFaces[0];
				(*frontFaces)[(*frontCount)++] = splitFaces[1];
				(*frontFaces)[(*frontCount)++] = splitFaces[2];
			}
			free(splitFaces);
		}
	}

	*frontFaces = realloc(*frontFaces, *frontCount * sizeof(Face));
	*backFaces = realloc(*backFaces, *backCount * sizeof(Face));
}

int planeEquals(Plane plane1, Plane plane2)
{
	return (plane1.normal.x == plane2.normal.x && plane1.normal.y == plane2.normal.y && plane1.normal.z == plane2.normal.z && plane1.distance == plane2.distance);
}

BSPNode* generateBSPTree(Face* faces, int faceCount)
{
	//base case
	if (faceCount == 0) return NULL;

	BSPNode* node = (BSPNode*) malloc(sizeof(BSPNode));
	
	node->splittingPlane = selectPlane(faces, faceCount);

	Plane nullPlane = { {0, 0, 0}, 0 };
	//is a leaf node -- none of the planes provide any splits.
	if (planeEquals(node->splittingPlane, nullPlane))
	{
		node->faces = faces;
		node->faceCount = faceCount;
		node->front = NULL;
		node->back = NULL;
		node->splittingPlane = nullPlane;
		printf("Going up!\n");
		return node;
	}
	
	Face* frontFaces;
	Face* backFaces;
	int frontCount, backCount;
	partitionFaces(faces, faceCount, node->splittingPlane, &frontFaces, &backFaces, &frontCount, &backCount);

	printf("Going front!\n");
	node->front = generateBSPTree(frontFaces, frontCount);
	printf("Going back!\n");
	node->back = generateBSPTree(backFaces, backCount);

	//is not a leaf node, and so has no faces
	node->faces = NULL;
	node->faceCount = 0;

	free(faces);

	return node;
}

BSPNode* generateCollisionMesh(const char* collisionFileSuffix)
{
	Face* faces = NULL;
	int faceCount = 0;
	int faceCap = 0;
	vec3* vertices = NULL;
	int vc = 0;
	int vertexCap = 0;

	char buffer[120];
	snprintf(buffer, sizeof(buffer), "%s%s", "./models/collision", collisionFileSuffix);
	FILE* collisionFile = fopen(buffer, "r");
	if (!collisionFile)
	{
		printf("Failed to open collision file: %s\n", collisionFileSuffix);
		return NULL;
	}

	char line[128];
	int c = 0;
	while(fgets(line, sizeof(line), collisionFile))
	{
		if (strncmp(line, "v ", 2) == 0)
		{
			if(vc>=vertexCap)
			{
				//exponential buffer growth
				vertexCap = vertexCap ? vertexCap*2 : 1024;
				vertices = realloc(vertices,sizeof(vec3)*vertexCap);
			}
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
			if(faceCount>=faceCap)
			{
				faceCap = faceCap ? faceCap*2 : 1024;
				faces = realloc(faces,sizeof(Face)*faceCap);
			}
			faces[faceCount++] = face;
		}
	}
	fclose(collisionFile);
	
	printf("			...Generating BSP for Object %s\n", collisionFileSuffix);
	BSPNode* rootNode = generateBSPTree(faces, faceCount);

	free(vertices);
	return rootNode;
}

void transformFace(Face* face, mat4 matrix)
{		
	face->a = transformVec3(face->a, matrix);
	face->b = transformVec3(face->b, matrix);
	face->c = transformVec3(face->c, matrix);
}

float determinantMat3(float m[9]) {
    return 
      m[0] * (m[4] * m[8] - m[5] * m[7]) -
      m[1] * (m[3] * m[8] - m[5] * m[6]) +
      m[2] * (m[3] * m[7] - m[4] * m[6]);
}

// Get 3x3 submatrix excluding row `excludeRow` and column `excludeCol`
void getSubmatrix(mat4 m, int excludeRow, int excludeCol, float sub[9]) {
    int subIndex = 0;
    for (int row = 0; row < 4; row++) {
        if (row == excludeRow) continue;
        for (int col = 0; col < 4; col++) {
            if (col == excludeCol) continue;
            sub[subIndex] = m.m[row * 4 + col];
            subIndex++;
        }
    }
}

mat4 inverseMat4(mat4 m) {
    mat4 inv;
    float det = 0.0f;

    // Compute matrix of minors and cofactors
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            // Get submatrix (3x3) excluding current row and column
            float sub[9];
            getSubmatrix(m, row, col, sub);

            // Compute minor and cofactor
            float minor = determinantMat3(sub);
            float cofactor = minor * ((row + col) % 2 == 0 ? 1.0f : -1.0f);

            // Transpose (adjugate) by storing at [col][row]
            inv.m[col * 4 + row] = cofactor;

            // Accumulate determinant using first row
            if (row == 0) {
                det += m.m[col] * cofactor; // m[col] = m.m[0*4 + col]
            }
        }
    }

    // Handle singular matrix
    if (fabsf(det) < 1e-8f) {
        mat4 identity = { .m = { 
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1 
        }};
        return identity;
    }

    // Divide adjugate by determinant
    float invDet = 1.0f / det;
    for (int i = 0; i < 16; i++) {
        inv.m[i] *= invDet;
    }

    return inv;
}

mat4 transpose(mat4 matrix)
{
	mat4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i+4*j] = matrix.m[j+4*i];
		}
	}
	return result;
}

void transformPlane(Plane* plane, mat4 matrix)
{
	mat4 invTranspose = transpose(inverseMat4(matrix));
	vec3 normal = plane->normal;
	plane->normal = transformVec3(normal, invTranspose);

	vec3 pointOnPlane = scaleVec3(normal, -plane->distance);
	vec3 transformedPoint = transformVec3(pointOnPlane, matrix);
	plane->distance = -dot(plane->normal, transformedPoint);
}

void transformBSPTree(BSPNode* node, mat4 modelMatrix)
{
	if (node == NULL) return;
	
	Plane nullPlane = {{0,0,0},0};
	if (!planeEquals(node->splittingPlane, nullPlane))
	{
		transformPlane(&node->splittingPlane, modelMatrix);
	}

	for (int i = 0; i < node->faceCount; i++)
	{
		transformFace(&node->faces[i], modelMatrix);
	}

	transformBSPTree(node->front, modelMatrix);
	transformBSPTree(node->back, modelMatrix);
}

//function to return list of faces around specific point
void traverseBSP(BSPNode* node, vec3 position, Face** faceBuffer, int* faceCountBuffer)
{
	if (!node) return;

	Plane nullPlane = { {0, 0, 0}, 0 };
	//is a leaf node -- none of the planes provide any splits.
	if (planeEquals(node->splittingPlane, nullPlane))
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
	float d = np.normal.x * position.x + np.normal.y * position.y + np.normal.z * position.z + np.distance;
	if (d >= -0.002f)
	{
		traverseBSP(node->front, position, faceBuffer, faceCountBuffer);
	}
	else
	{
		traverseBSP(node->back, position, faceBuffer, faceCountBuffer);
	}
}
Ray segmentToRay(lineSeg lseg)
{
    Ray r;
    r.origin = lseg.a;
    r.ray.x = lseg.b.x - lseg.a.x;
    r.ray.y = lseg.b.y - lseg.a.y;
    r.ray.z = lseg.b.z - lseg.a.z;
    return r;
}
//smarter, better version of "raycastBSP" specifically for collisions
//instead of naively just casting a ray through the whole bsp and getting every possible face it:
//	- Checks which side of the splitting plane a and b of lineSeg are on.
//	- If the are on the same side, traverses that side
//	- If on different sides, will split the line and traverse both sides with the new a and b
//
//This should catch the weird edge cases where a collision face is outside of the players node
void getCollisionFacesBSP(BSPNode* node, lineSeg colseg, Face** faceBuffer, int* faceCountBuffer)
{
	Plane nullPlane = { {0, 0, 0}, 0 };
	if (planeEquals(node->splittingPlane, nullPlane))
	{
		int currentCount = *faceCountBuffer;
		int newCount = currentCount + node->faceCount;
		*faceBuffer = (Face*)realloc(*faceBuffer, newCount * sizeof(Face));
		if (*faceBuffer == NULL) { *faceCountBuffer = 0; return; }
		for (int i = 0; i < node->faceCount; i++) 
		{
			(*faceBuffer)[currentCount + i] = node->faces[i];
		}
		*faceCountBuffer = newCount;
		return;
	}

	Plane np = node->splittingPlane;
	float da = dot(colseg.a, np.normal) + np.distance;
	float db = dot(colseg.b, np.normal) + np.distance;
	//different sides of the plane
	int daSide = (da >= -0.002f); int dbSide = (db >= -0.002f);
	if (daSide != dbSide) //different sides
	{
		Ray r = segmentToRay(colseg);
		PlaneCollision pcol = rayPlaneIntersection(r, np);
		lineSeg seg1 = {colseg.a, pcol.worldPosition};
		lineSeg seg2 = {pcol.worldPosition, colseg.b};
		BSPNode* node1 = daSide ? node->front : node->back;
		BSPNode* node2 = dbSide ? node->front : node->back;
		getCollisionFacesBSP(node1, seg1, faceBuffer, faceCountBuffer);
		getCollisionFacesBSP(node2, seg2, faceBuffer, faceCountBuffer);
	}
	else if (daSide & dbSide) //both in front
	{
		getCollisionFacesBSP(node->front, colseg, faceBuffer, faceCountBuffer);
	}
	else //both behind
	{
		getCollisionFacesBSP(node->back, colseg, faceBuffer, faceCountBuffer);
	}
	return;
}

//gets all faces in all partitions the ray intersects. 
void raycastBSP(BSPNode* node, Ray ray, Face** faceBuffer, int* faceCountBuffer)
{
	Plane nullPlane = { {0, 0, 0}, 0 };
	//is a leaf node -- none of the planes provide any splits.
	if (planeEquals(node->splittingPlane, nullPlane))
	{       
	// Append faces to the buffer
        size_t currentCount = *faceCountBuffer;
        size_t newCount = currentCount + node->faceCount;
        *faceBuffer = (Face*)realloc(*faceBuffer, newCount * sizeof(Face));
        if (*faceBuffer == NULL) {
            *faceCountBuffer = 0; // Handle allocation failure
            return;
        }
        for (int i = 0; i < node->faceCount; i++) {
            (*faceBuffer)[currentCount + i] = node->faces[i];
        }
        *faceCountBuffer = newCount;
        return;
	}
	
	float distance = dot(ray.origin, node->splittingPlane.normal) + node->splittingPlane.distance;
	float denominator = dot(ray.ray, node->splittingPlane.normal);
	
	BSPNode* nearChild;
	BSPNode* farChild;

	if (distance >= 0)
	{
		nearChild = node->front;
		farChild = node->back;
	}
	else
	{
		nearChild = node->back;
		farChild = node->front;
	}
	
	if (denominator == 0)
	{
		raycastBSP(nearChild, ray, faceBuffer, faceCountBuffer);
		return;
	}

	float t = -distance/denominator;

	if (t < 0)
	{
		raycastBSP(nearChild, ray, faceBuffer, faceCountBuffer);
		return;
	}	

	raycastBSP(nearChild, ray, faceBuffer, faceCountBuffer);

	Ray newRay;
	newRay.ray = ray.ray;
	newRay.origin = addVec3(ray.origin, scaleVec3(ray.ray, t));

	raycastBSP(farChild, newRay, faceBuffer, faceCountBuffer);
	return;
}

//function returns the closest face collision from a ray in the BSP tree.
/*void AraycastBSP(BSPNode* node, Ray ray, FaceCollision* closestCollision)
{
	Plane nullPlane = {0};
	if (planeEquals(nullPlane, node->splittingPlane))
	{
		//is a leaf node...
		for (int i = 0; i < node->faceCount; i++)
		{
			Face face = node->faces[i];
			Collision collision = mollerTrumboreRaycast(ray, face);
			c++;
			if (!collision.status || collision.t > closestCollision->distance)
			{
				continue;
			}
			else
			{
				//node contains a closer face
				closestCollision->distance = collision.t;
				closestCollision->collidedFace = face;
				closestCollision->didCollide = 1;
				closestCollision->barycentricCoordinates = collision.barycentricCoordinates;
			}
		}
		return;
	}

	float distance = dot(ray.origin, node->splittingPlane.normal) + node->splittingPlane.distance;
	float denominator = dot(ray.ray, node->splittingPlane.normal);
	
	BSPNode* nearChild;
	BSPNode* farChild;

	if (distance >= -0.002f)
	{
		nearChild = node->front;
		farChild = node->back;
	}
	else
	{
		nearChild = node->back;
		farChild = node->front;
	}
	
	if (denominator == 0)
	{
		raycastBSP(nearChild, ray, closestCollision);
		return;
	}

	float t = -distance/denominator;
	
	if (t <= 0.002f)
	{
		raycastBSP(nearChild, ray, closestCollision);
		return;
	}

	raycastBSP(nearChild, ray, closestCollision);

	Ray newRay;
	newRay.ray = ray.ray;
	newRay.origin = addVec3(ray.origin, scaleVec3(ray.ray, t + 0.002f));

	raycastBSP(farChild, newRay, closestCollision);
	return;
}*/

//function to cast ray, and return all faces around the ray
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




int raycastBSP(BSPNode* node, Ray ray, Face** faceBuffer, int* faceCountBuffer)
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
        return 0;
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


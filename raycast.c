#include "raycast.h"

Collision mollerTrumboreRaycast(Ray ray, Face face)
{
/*
	v0\
	  | \
	  |   \
	  |     \
	v1|_______\v2

	the equation for a parametric ray is:
		R(t) = O + tD
	where 
		O is the origin vector,
		t a scalar distance along the ray, 
		and D the direction vector
*/
	vec3 v0 = face.a;
	vec3 v1 = face.b;
	vec3 v2 = face.c;
	vec3 O = ray.origin;
	vec3 D = ray.ray;

	//compute edge vectors

	//THIS CAN BE PRECOMPUTED!!!
	vec3 e1 = {v1.x-v0.x,v1.y-v0.y,v1.z-v0.z};
	vec3 e2 = {v2.x-v0.x,v2.y-v0.y,v2.z-v0.z};
	
	//compute determinant
	vec3 p = crossVec3(D, e2);
	float det = getVec3Dot(e1, p);
	if (fabs(det) < 1e-12f && fabs(det) > -1e-12f)
	{
		return (Collision) { 0, 0, (vec2) { 0, 0} };
	}

	//barycentric coordinates
	vec3 T = {O.x-v0.x,O.y-v0.y,O.z-v0.z};
	vec3 Q = crossVec3(T, e1);
	float u = (getVec3Dot(T, p))/det;
	float v = (getVec3Dot(D, Q))/det;

	if (u < 0 || v < 0 || (u+v) > 1)
	{
		return (Collision) { 0, 0, (vec2) { 0, 0} };
	}
	float t = (getVec3Dot(e2, Q))/det;
	if (t < 0)
	{
		return (Collision) { 0, 0, (vec2) { 0, 0} };
	}
	return (Collision) { 1, t, (vec2) { u, v} };
}

PlaneCollision rayPlaneIntersection(Ray ray, Plane plane)
{
	PlaneCollision result = {0, 0.0f, {0.0f, 0.0f, 0.0f}};

	const float denominator = getVec3Dot(plane.normal, ray.ray);
	const float numerator = getVec3Dot(plane.normal, ray.origin) + plane.distance;

	if (fabsf(denominator) < 0.0002f)
	{
		if (fabsf(numerator) < 0.0002f)
		{
			result.status = 0;
			result.t = 0.0f;
			result.worldPosition = ray.origin;
		}
		return result;
	}

	const float t = -numerator / denominator;
	
	if (t < 0.0002f)
	{
		result.status = 0;
		result.t = 0.0f;
		result.worldPosition = ray.origin;
		return result;
	}

	result.status = 1;
	result.t = t;
	result.worldPosition = addVec3(ray.origin, scaleVec3(ray.ray, t));

	return result;
}

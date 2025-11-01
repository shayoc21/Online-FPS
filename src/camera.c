#include "camera.h"

Camera* currentCamera;

void initCamera(Camera* camera, vec3 position, vec3 direction, float fov)
{
	camera->position = position;
	camera->front = direction;
	camera->up = vector(0.0f, 1.0f, 0.0f);
	camera->right = crossVec3(camera->front, camera->up);
	camera->fov = fov;

	camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
	camera->projection = perspectiveMat4(camera->fov, 1280/720, 0.1f, 1000.0f);
}

vec3 rotateVector(quaternion q, vec3 vector)
{
	quaternion qI = {q.w, -q.x, -q.y, -q.z};
	quaternion v = {0.0f, vector.x, vector.y, vector.z};
	quaternion r = multiplyQuat(multiplyQuat(q, v), qI);
	return (vec3) {r.x, r.y, r.z};
}

void rotateCamera(Camera* camera, quaternion rotation)
{
	camera->front = rotateVector(rotation, camera->front);
	camera->right = rotateVector(rotation, camera->right);
	camera->up = rotateVector(rotation, camera->up);
	camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
}

void resetCamera(Camera* camera)
{
	camera->position = vector(-65,118,58);
	camera->front = vector(1,-0.115,0);
	camera->up = vector(0,1,0);
	camera->right = crossVec3(camera->front, camera->up);
	camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
	camera->projection = perspectiveMat4(camera->fov, 1280/720, 0.1f, 1000.0f);
}

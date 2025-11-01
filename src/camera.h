#ifndef _CAMERA
#define _CAMERA

#include "mathlibrary/maths.h"

struct Camera
{
	mat4 view;
	mat4 projection;
	vec3 position;
	vec3 front;
	vec3 right;
	vec3 up;
	float fov;
};
typedef struct Camera Camera;
extern Camera* currentCamera;

void initCamera(Camera* camera, vec3 position, vec3 direction, float fov);
void rotateCamera(Camera* camera, quaternion rotation);
vec3 rotateVector(quaternion q, vec3 vector);
void resetCamera(Camera* camera);

#endif

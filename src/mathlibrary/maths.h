#ifndef _MATHS
#define _MATHS

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>

#define PI 3.1415926535

struct vec3
{
	float x, y, z;
};
typedef struct vec3 vec3;

struct vec2
{
	float x, y;
};
typedef struct vec2 vec2;

struct mat4
{
	float m[16];
};
typedef struct mat4 mat4;

struct quaternion
{
	float w, x, y, z;
};
typedef struct quaternion quaternion;

void printMat4(const mat4 matrix);

vec3 vector(float x, float y, float z);
quaternion quat(vec3 axis, float rotation);

quaternion normalizeQuat(quaternion quat);
quaternion conjugateQuat(quaternion quat);
quaternion clampQuat(quaternion quat);
quaternion multiplyQuat(quaternion q1, quaternion q2);

mat4 identityMat4();
mat4 multiplyMat4(mat4 matrix1, mat4 matrix2);
mat4 translationMat4(vec3 translation);
mat4 rotationMat4(quaternion rotation);
mat4 scaleMat4(vec3 scale);

mat4 lookMat4(vec3 cameraPos, vec3 cameraTarget, vec3 cameraUp);
mat4 orthographicMat4(float left, float right, float bottom, float top, float near, float far);
mat4 perspectiveMat4(float fov, float aspectRatio, float near, float far);
mat4 generateModelMatrix(vec3 position, quaternion rotation, vec3 scale);

vec3 scaleVec3(vec3 vector, float scale);
vec3 subtractVec3(vec3 vector1, vec3 vector2);
vec3 addVec3(vec3 vector1, vec3 vector2);
vec3 normalizeVec3(vec3 vector);
vec3 crossVec3(vec3 vector1, vec3 vector2);
float getVec3Length(vec3 vector);
float getVec3Dot(vec3 vector1, vec3 vector2);
vec3 transformVec3(vec3 vector, mat4 matrix);

float radians(float degrees);

#endif

#include "maths.h"

vec3 transformVec3(vec3 vector, mat4 matrix)
{
	return (vec3)
	{
		matrix.m[0]*vector.x + matrix.m[1]*vector.y + matrix.m[2]*vector.z + matrix.m[3],
		matrix.m[4]*vector.x + matrix.m[5]*vector.y + matrix.m[6]*vector.z + matrix.m[7],
		matrix.m[8]*vector.x + matrix.m[9]*vector.y + matrix.m[10]*vector.z + matrix.m[11]
	};
}

vec3 scaleVec3(vec3 vector, float scale)
{
	return (vec3) {vector.x * scale, vector.y * scale, vector.z * scale};
}

vec3 vector(float x, float y, float z)
{
	vec3 v = {x, y, z};
	return v;
}

quaternion quat(vec3 axis, float rotation)
{
	axis = normalizeVec3(axis);
	float s = sin(radians(rotation)/2);
	float c = cos(radians(rotation)/2);

	quaternion returnQuat = {c, axis.x*s, axis.y*s, axis.z*s};
	return normalizeQuat(returnQuat);
}

quaternion clampQuat(quaternion quat)
{
	return (quaternion) {0.0f, quat.x, quat.y, quat.z};
}

quaternion normalizeQuat(quaternion quat)
{
	double magnitude = sqrt
	(
		quat.w*quat.w+
		quat.x*quat.x+
		quat.y*quat.y+
		quat.z*quat.z
	);
	if (magnitude == 0) return (quaternion){0.0f, 0.0f, 0.0f, 0.0f};
	quaternion returnQuat =
	{
		quat.w/magnitude,
		quat.x/magnitude,
		quat.y/magnitude,
		quat.z/magnitude
	};
	return returnQuat;
}

quaternion conjugateQuat(quaternion quat)
{
	return (quaternion){quat.w, -quat.x, -quat.y, -quat.z};
}	

quaternion multiplyQuat(quaternion q1, quaternion q2)
{
	quaternion returnQuat = 
	{
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
	};
	return returnQuat;
}

void printMat4(const mat4 matrix)
{
    printf("View:\n");
    for (int i = 0; i < 4; i++)  // Loop through rows
    {
        for (int j = 0; j < 4; j++)  // Loop through columns
        {
            printf("%6.2f ", matrix.m[i * 4 + j]);  // Access element at (i, j)
        }
        printf("\n");  // Newline after each row
    }
    printf("\n");
}

mat4 identityMat4()
{
  mat4 returnMatrix;
  static const float identityMatrix[16] =
  {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  memcpy(returnMatrix.m, identityMatrix, sizeof(identityMatrix));
  return returnMatrix;
}

mat4 multiplyMat4(mat4 matrix1, mat4 matrix2)
{
  mat4 returnMatrix;
  memset(returnMatrix.m, 0, sizeof(returnMatrix.m));
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      for (int k = 0; k < 4; k++)
      {
        returnMatrix.m[i*4+j] += matrix1.m[i*4+k] * matrix2.m[k*4+j];
      }
    }
  }
  return returnMatrix;
}

mat4 translationMat4(vec3 vector)
{
  mat4 returnMatrix = identityMat4();
  returnMatrix.m[ 3] = vector.x;
  returnMatrix.m[ 7] = vector.y;
  returnMatrix.m[11] = vector.z;
  return returnMatrix;
}

mat4 rotationMat4(quaternion quat)
{
	float w = quat.w;
	float x = quat.x;
	float x2 = x*x;
	float y = quat.y;
	float y2 = y*y;
	float z = quat.x;
	float z2 = z*z;

	mat4 returnMatrix;
	float mat[16] = 
	{
		1-2*y2-2*z2, 2*x*y-2*w*z, 2*x*z+2*w*y, 0,
		2*x*y+2*w*z, 1-2*x2-2*z2, 2*y*z-2*w*x, 0,
		2*x*z-2*w*y, 2*y*z+2*w*x, 1-2*x2-2*y2, 0,
		0		   ,		   0,			0, 1
	};
	memcpy(returnMatrix.m, mat, sizeof(mat));
	return returnMatrix;
}

mat4 scaleMat4(vec3 scale)
{
  	mat4 scaleMatrix;
  	float mat[16] =
  	{
		scale.x,       0,       0,     0,
		0	   , scale.y,       0,     0,
		0      ,       0, scale.z,     0,
		0      ,       0,       0,     1
	};
	memcpy(scaleMatrix.m, mat, sizeof(mat));
	return scaleMatrix;
}

mat4 lookMat4(vec3 cameraPos, vec3 cameraTarget, vec3 cameraUp)
{
  vec3 f = normalizeVec3(subtractVec3(cameraPos, cameraTarget));
  vec3 r = normalizeVec3(crossVec3(cameraUp, f));
  vec3 u = normalizeVec3(crossVec3(f, r));
  
  mat4 returnMatrix;
  float m1[16] =
  {
    r.x, r.y, r.z, 0,
    u.x, u.y, u.z, 0,
    f.x, f.y, f.z, 0,
    0  , 0  , 0  , 1
  };
  mat4 left;
  memcpy(left.m, m1, sizeof(m1));
  float m2[16] =
  {
    1, 0, 0, -cameraPos.x,
    0, 1, 0, -cameraPos.y,
    0, 0, 1, -cameraPos.z,
    0, 0, 0, 1
  };
  mat4 right;
  memcpy(right.m, m2, sizeof(m2));

  returnMatrix = multiplyMat4(left, right);
  return returnMatrix;
}

mat4 orthographicMat4(float l, float r, float b, float t, float n, float f)
{
  
  //SMALL PARAM NAMES TO ALLOW FOR MORE READABLE DEFINITION OF RETURNMATRIX
  //::PARAM NAME MEANINGS:://
  //  l - left plane of near view frustrum
  //  r - right plane ^^
  //  b - bottom plane
  //  t - top plane 
  //  n - distance to near plane
  //  f - distance to far plane
 
  mat4 returnMatrix;
  float orthoMatrix[16] =
  {
    2/(r-l), 0      , 0       , -(r+l)/(r-l),
    0      , 2/(t-b), 0       , -(t+b)/(t-b),
    0      , 0      , -2/(f-n), -(f+n)/(f-n),
    0      , 0      , 0       , 1
  };
  memcpy(returnMatrix.m, orthoMatrix, sizeof(orthoMatrix));
  return returnMatrix;
}

mat4 perspectiveMat4(float fov, float aspectRatio, float near, float far)
{
  float 
    tanf = tan(fov/2), 
    ar = aspectRatio,
    n = near,
    f = far;

  mat4 returnMatrix;
  float perspectiveMatrix[16] = 
  {
    1/(ar*tanf), 0     , 0          , 0,
    0          , 1/tanf, 0          , 0,
    0          , 0     , (f+n)/(n-f), 2*f*n/(n-f),
    0          , 0     , -1         , 0
  };
  memcpy(returnMatrix.m, perspectiveMatrix, sizeof(perspectiveMatrix));
  return returnMatrix;
}

vec3 subtractVec3(vec3 vector1, vec3 vector2)
{
  vec3 returnVector =
  {
    vector1.x - vector2.x,
    vector1.y - vector2.y,
    vector1.z - vector2.z
  };
  return returnVector;
}

vec3 addVec3(vec3 vector1, vec3 vector2)
{
	vec3 returnVector = 
	{
		vector1.x + vector2.x,
		vector1.y + vector2.y,
		vector1.z + vector2.z
	};
	return returnVector;
}

vec3 normalizeVec3(vec3 vector)
{
	float vectorLength = getVec3Length(vector);
	vec3 returnVector = 
	{
		vector.x/vectorLength,
		vector.y/vectorLength,
		vector.z/vectorLength
	};
	return returnVector;
}

vec3 crossVec3(vec3 vector1, vec3 vector2)
{
	vec3 returnVector = 
	{
		vector1.y*vector2.z - vector1.z*vector2.y,
		vector1.z*vector2.x - vector1.x*vector2.z,
		vector1.x*vector2.y - vector1.y*vector2.x
	};
	return returnVector;
}

float getVec3Length(vec3 vector)
{
	return sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

float getVec3Dot(vec3 vector1, vec3 vector2)
{
	return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z;
}

float radians(float degrees)
{
	return degrees * (PI/180);
}

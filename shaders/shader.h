#ifndef _SHADER
#define _SHADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct shaderSource
{
  char* vertexShaderSource;
  char* fragmentShaderSource;
};
typedef struct shaderSource shaderSource;

void parseShaders(shaderSource* shaderSource, const char* shaderSourceFilename);
void destroyShaderSource(shaderSource* shaderSource);

#endif


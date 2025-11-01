#include "shader.h"

void parseShaders(shaderSource* shaderSource, const char* shaderSourceFilename)
{
  FILE *shaderFile = fopen(shaderSourceFilename, "r");
  if (shaderFile == NULL)
  {
    fprintf(stderr, "SHADER SOURCE FILE PARSED NULL");
  }

  shaderSource->vertexShaderSource = ((void*) 0);
  shaderSource->fragmentShaderSource = ((void*) 0);
  
  char line[256];
  char** currentShaderPointer;

  while (fgets(line, 256, shaderFile))
  {
    if(line == NULL)
    {
      printf("LINE IS NULL\n");
    }
    if (!strcmp(line, "-- VERTEX\n"))
    {
      currentShaderPointer = &shaderSource->vertexShaderSource;
    }
    else if (!strcmp(line, "-- FRAGMENT\n"))
    {
      currentShaderPointer = &shaderSource->fragmentShaderSource;
    }
    else
    {
      if (*currentShaderPointer == NULL)
      {
        *currentShaderPointer = (char*)malloc(strlen(line) + 1);
        strcpy(*currentShaderPointer, line);
      }
      else
      {
        *currentShaderPointer = (char*)realloc(*currentShaderPointer, strlen(*currentShaderPointer) + strlen(line) + 2);
        strcat(*currentShaderPointer, line);
      }
    }
  }    
  fclose(shaderFile);
}

void destroyShaderSource(shaderSource* shaderSource)
{
  free(shaderSource->vertexShaderSource);
  free(shaderSource->fragmentShaderSource);
}











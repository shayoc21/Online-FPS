#ifndef _RENDER
#define _RENDER

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objparser.h"

#define GAMEHEIGHT 396
#define GAMEWIDTH 704

struct Renderer
{
	GLuint FBO;
	GLuint DBO;
	GLuint SCREENVAO;
	GLuint SCREENSHADER;
	GLuint SCREENTEXTURE;
};
typedef struct Renderer Renderer;

void initRenderer(Renderer* renderer);

#endif

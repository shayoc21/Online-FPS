#ifndef _GAME
#define _GAME

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "scene.h"
#include "camera.h"
#include "session.h"
#include "render.h"

struct Game
{
	GLFWwindow* window;
	Scene scene;
	Renderer renderer;
	Camera playerCamera;
};
typedef struct Game Game;

void initGame(Game* game);
void runGame(Game* game);
void closeGame(Game* game);

#endif

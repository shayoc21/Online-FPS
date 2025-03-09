#ifndef _GAME
#define _GAME

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "scene.h"
#include "camera.h"
#include "session.h"

struct Game
{
	GLFWwindow* window;
	Scene scene;
	//HUD HUD;
	Camera playerCamera;
};
typedef struct Game Game;

void initGame(Game* game);
void runGame(Game* game);
void closeGame(Game* game);

#endif

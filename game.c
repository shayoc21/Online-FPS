#include "game.h"

//private function declarations
void pollInputs(Game* game);
int isGameRunning(Game* game);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void initGame(Game* game)
{
	printf("Initializing Game...\n");

	initWindow(&game->window);
	initGL();

	initScene(&game->scene);
	//initHUD(&game->HUD);
	
	initCamera(&game->playerCamera, vector(-65.0f, 118.0f, 58.0f), vector(1.0f, -0.115f, 0.0f), 90);

	currentCamera = &game->playerCamera;

	glfwSetCursorPosCallback(game->window, mouseCallback);	

	printf("	...Game Initialized\n");
}

void runGame(Game* game)
{
	printf("Running Game...\n");
	int frameCount = 0;
	float previousTime = glfwGetTime();
	while (isGameRunning(game))
	{
	 	float currentTime = glfwGetTime();
		frameCount++;
		pollInputs(game);

		clearScreen();

		//draw screen
		drawScene(&game->scene);
		//drawHUD(&game->HUD);

		//swap buffers
		glfwSwapBuffers(game->window);

		currentCamera->position.y = 118;

		GLenum err = glGetError();
		while (err != GL_NO_ERROR)
		{
			printf("OpenGL Error: %d\n", err);
		}

		if (currentTime - previousTime >= 10.0)
		{
			printf("	...FPS : %f\n", (frameCount / 10.0f));
			frameCount = 0;
			previousTime = currentTime;
		}	
	}
}

void closeGame(Game* game)
{
	printf("Closing Game...\n");
	destroyScene(&game->scene);
	//destroyHUD(&game->hud);
	//destroyCamera(&game->playerCamera);
	glfwTerminate();
	printf("	...Game Closed\n");
}

// ===========Private function Definitions (disgusting code)===============//

int firstMouse = 1;
float lastX = 690;
float lastY = 360;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = 0;
	}

	float xoffset = -xpos+lastX;
	float yoffset = -ypos+lastY;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	quaternion xRotation = quat(vector(0,1,0), xoffset);
	quaternion yRotation = quat(currentCamera->right, yoffset);
	rotateCamera(currentCamera, xRotation);
	rotateCamera(currentCamera, yRotation);
}

//dont know where this function should go. it raycasts so it needs to include scene, this prevents circular dependencies

void movePlayer(Game* game, vec3 translation)
{
	Camera* camera = currentCamera;
	vec3 newPosition = addVec3(camera->position, translation);

	int collisionDetected = 0;
	float threshold = getVec3Length(translation) * 2;

	for (int i = 0; i < 2; i++)
	{
		float angle = (2 * PI / 4) * i;
		vec3 direction = { cos(angle), 0.0f, sin(angle) };

		Ray ray = { direction, newPosition };
		float distance = castRayThroughScene(&game->scene, ray, threshold);
		if (distance < threshold)
		{
			collisionDetected = 1;
			break;
		}
	}

	if (collisionDetected) return;

	camera->position = newPosition;
	camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
}

void pollInputs(Game* game)
{
	glfwPollEvents();
	float velocity = 0.2;
	if (glfwGetKey(game->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		velocity = 0.35;
	}
	if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(game->window, 1);
	}

	vec3 front = {currentCamera->front.x, 0, currentCamera->front.z};
	vec3 right = {currentCamera->right.x, 0, currentCamera->right.z};
	front = normalizeVec3(front);
	right = normalizeVec3(right);

	if (glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS)
	{
		vec3 movement = scaleVec3(front, velocity);
		movePlayer(game, movement);
	}
	if (glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS)
	{
		vec3 movement = scaleVec3(front, -velocity);
		movePlayer(game, movement);
	}
	if (glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS)
	{
		vec3 movement = scaleVec3(right, -velocity);
		movePlayer(game, movement);
	}
	if (glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS)
	{
		vec3 movement = scaleVec3(right, velocity);
		movePlayer(game, movement);
	}
	if (glfwGetKey(game->window, GLFW_KEY_P) == GLFW_PRESS)
	{
		printf("CAMERADIR : %f %f %f\n", currentCamera->front.x, currentCamera->front.y, currentCamera->front.z);
		printf("CAMERAPOS : %f %f %f\n", currentCamera->position.x, currentCamera->position.y, currentCamera->position.z);
	}
	if (glfwGetKey(game->window, GLFW_KEY_R) == GLFW_PRESS)
	{
		resetCamera(currentCamera);
	}
}

int isGameRunning(Game* game)
{
	return !glfwWindowShouldClose(game->window);
}


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

	Object* object = game->scene.worldObjects[0];
	Face* playerFaces = NULL;
	int playerFaceCount = 0;
	traverseBSP(object->collisionMesh, currentCamera->position, &playerFaces, &playerFaceCount);
	
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, playerFaceCount*sizeof(Face), playerFaces, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	GLuint SHADER = loadShader("shaders/debugRed.glsl");
	printf("s: %d\n", SHADER);

	int c = 0;
	while (isGameRunning(game))
	{
	 	float currentTime = glfwGetTime();
		frameCount++;
		pollInputs(game);

		clearScreen();

		//draw screen

	Face* playerFaces = NULL;
	int playerFaceCount = 0;
	traverseBSP(object->collisionMesh, currentCamera->position, &playerFaces, &playerFaceCount);
		glUseProgram(SHADER);
		glBindVertexArray(VAO);

	glBufferData(GL_ARRAY_BUFFER, playerFaceCount*sizeof(Face), playerFaces, GL_STATIC_DRAW);
		int pl = glGetUniformLocation(SHADER, "projection");
		int vl = glGetUniformLocation(SHADER, "view");
			
		glUniformMatrix4fv(pl, 1, GL_TRUE, currentCamera->projection.m);
		glUniformMatrix4fv(vl, 1, GL_TRUE, currentCamera->view.m);

		glDrawArrays(GL_TRIANGLES, 0, playerFaceCount*3);
		c++;

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
	vec3 newPosition = addVec3(currentCamera->position, translation);

	int collisionDetected = 0;
	float threshold = getVec3Length(translation) * 2;

	for (int i = 0; i < 8; i++)
	{
		float angle = (2 * PI / 8) * i;
		vec3 direction = { cos(angle), 0.0f, sin(angle) };

		Ray ray = { normalizeVec3(direction), newPosition };
		float distance = getShortestSceneCollision(&game->scene, ray);
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


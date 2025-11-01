#include "game.h"

//private function declarations
void pollInputs(Game* game);
int isGameRunning(Game* game);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void renderGame(Game* game, Renderer* renderer, float time);

void initGame(Game* game)
{
	printf("Initializing Game...\n");

	initWindow(&game->window);
	initGL();
	initRenderer(&game->renderer);

	initScene(&game->scene);
	
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

	float v[9] = 
	{
		0.0f, 0.0f, 0.0f,
		-0.01f, -0.01f, 0.0f,
		0.01f, -0.01f, 0.0f
	};

	GLuint VAOCROSSHAIR;
	glGenVertexArrays(1, &VAOCROSSHAIR);
	glBindVertexArray(VAOCROSSHAIR);

	GLuint VBOCROSSHAIR;
	glGenBuffers(1, &VBOCROSSHAIR);
	glBindBuffer(GL_ARRAY_BUFFER, VBOCROSSHAIR);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), v, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	GLuint SHADERCROSSHAIR = loadShader("shaders/crosshair.glsl");

	int c = 0;
	while (isGameRunning(game))
	{
	 	float currentTime = glfwGetTime();
		frameCount++;
		pollInputs(game);

		//draw screen
		renderGame(game, &game->renderer, currentTime);

		glDisable(GL_DEPTH_TEST);
		glUseProgram(SHADERCROSSHAIR);
		glBindVertexArray(VAOCROSSHAIR);
		glBindBuffer(GL_ARRAY_BUFFER, VBOCROSSHAIR);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glEnable(GL_DEPTH_TEST);


		//swap buffers
		glfwSwapBuffers(game->window);

		GLenum err = glGetError();
		while (err != GL_NO_ERROR)
		{
			printf("OpenGL Error: %d\n", err);
		}
		if (currentTime - previousTime >= 10.0)
		{
			for (int i = 0; i < 1; i++)
			{
				printf("	...FPS : %f\n", (frameCount / 10.0f));
			}
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
	int collisionDetected = 0;
	float threshold = getVec3Length(translation) * 2;
	
	lineSeg collisionSeg = {camera->position, addVec3(camera->position, translation)};
	//get possible collision faces
	Face* faces = (Face*)malloc(sizeof(Face));
	int faceCount = 0;
	Object* obj = game->scene.worldObjects[0];
	getCollisionFacesBSP(obj->collisionMesh, collisionSeg, &faces, &faceCount);
	//raycast against each face
	float shortestDistance = FLT_MAX;
	Ray ray = {normalizeVec3(translation), camera->position};
	for (int j = 0; j < faceCount; j++)
	{
		Collision c = mollerTrumboreRaycast(ray, faces[j]);
		if (c.status && c.t < threshold)
		{
			collisionDetected++;
			free(faces);
			return;
		}
	}
	free(faces);

	camera->position = addVec3(camera->position, translation);
	camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
}

void movePlayerHeight(Game* game, vec3 translation)
{
	Camera* camera = currentCamera;
	vec3 newPosition = camera->position;

	float threshold = getVec3Length(translation) * 2;

	Ray ray = { normalizeVec3(translation), newPosition };
	Face* faces = (Face*)malloc(sizeof(Face));
	int faceCount = 0;
	raycastBSP(game->scene.worldObjects[0]->collisionMesh, ray, &faces, &faceCount);
	for (int i = 0; i < faceCount; i++)
	{
		Collision c = mollerTrumboreRaycast(ray, faces[i]);
		if (c.status && c.t < threshold)
		{
			free(faces);
			return;
		}
	}
	camera->position = addVec3(camera->position, translation);
	camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
	free(faces);
	return;
}
	
int a = 1;
//used to deny inputs, by doing nothing when called.
void nothingFunction(GLFWwindow* window, double xpos, double ypos){return;}

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
	if (glfwGetKey(game->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		vec3 movement = scaleVec3(vector(0,1,0), -velocity);
		movePlayerHeight(game, movement);
	}
	if (glfwGetKey(game->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		vec3 movement = scaleVec3(vector(0,1,0), velocity);
		movePlayerHeight(game, movement);
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
	if (glfwGetKey(game->window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		if (a)
		{
			glfwSetInputMode(game->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(game->window, nothingFunction);	
			a = 0;
		}
		else
		{
			glfwSetInputMode(game->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(game->window, mouseCallback);	
			a = 1;
		}
	}
}

int isGameRunning(Game* game)
{
	return !glfwWindowShouldClose(game->window);
}

void renderGame(Game* game, Renderer* renderer, float time)
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->FBO);
	glViewport(0, 0, GAMEWIDTH, GAMEHEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene(&game->scene);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(renderer->SCREENSHADER);
	GLint timeloc = glGetUniformLocation(renderer->SCREENSHADER, "time");
	glUniform1f(timeloc, time);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderer->SCREENTEXTURE);
	glBindVertexArray(renderer->SCREENVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
	


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
	free(playerFaces);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	GLuint SHADER = loadShader("shaders/debugRed.glsl");
	printf("s: %d\n", SHADER);
	
	FaceCollision collidedFace = {0};
	collidedFace.distance = FLT_MAX;
	Ray playerRay = {normalizeVec3(currentCamera->front), currentCamera->position};
	raycastBSP(object->collisionMesh, playerRay, &collidedFace);

	GLuint VAOR;
	glGenVertexArrays(1, &VAOR);
	glBindVertexArray(VAOR);

	GLuint VBOR;
	glGenBuffers(1, &VBOR);
	glBindBuffer(GL_ARRAY_BUFFER, VBOR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Face), &collidedFace.collidedFace, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	GLuint SHADERR = loadShader("shaders/debugBlue.glsl");
	printf("sr: %d\n", SHADERR);
	
	float v[9] = 
	{
		0.0f, 0.0f, 0.0f,
		-0.1f, -0.1f, 0.0f,
		0.1f, -0.1f, 0.0f
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

		clearScreen();

		//draw screen

		Face* playerFaces = NULL;
		int playerFaceCount = 0;
		vec3 pos = currentCamera->position;
		traverseBSP(object->collisionMesh, pos, &playerFaces, &playerFaceCount);
		glUseProgram(SHADER);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, playerFaceCount*sizeof(Face), playerFaces, GL_STATIC_DRAW);
		free(playerFaces);

		int pl = glGetUniformLocation(SHADER, "projection");
		int vl = glGetUniformLocation(SHADER, "view");
			
		glUniformMatrix4fv(pl, 1, GL_TRUE, currentCamera->projection.m);
		glUniformMatrix4fv(vl, 1, GL_TRUE, currentCamera->view.m);

		glDrawArrays(GL_TRIANGLES, 0, playerFaceCount*3);

		FaceCollision collidedFace = {0};
		collidedFace.distance = FLT_MAX;
		FUCKTHISSHIT();
		Ray playerRay = {normalizeVec3(currentCamera->front), currentCamera->position};
		raycastBSP(object->collisionMesh, playerRay, &collidedFace);

		glUseProgram(SHADERR);
		glBindVertexArray(VAOR);
		glBindBuffer(GL_ARRAY_BUFFER, VBOR);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Face), &collidedFace.collidedFace, GL_STATIC_DRAW);

		int plr = glGetUniformLocation(SHADERR, "projection");
		int vlr = glGetUniformLocation(SHADERR, "view");

		glUniformMatrix4fv(plr, 1, GL_TRUE, currentCamera->projection.m);
		glUniformMatrix4fv(vlr, 1, GL_TRUE, currentCamera->view.m);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glDisable(GL_DEPTH_TEST);
		glUseProgram(SHADERCROSSHAIR);
		glBindVertexArray(VAOCROSSHAIR);
		glBindBuffer(GL_ARRAY_BUFFER, VBOCROSSHAIR);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glEnable(GL_DEPTH_TEST);

		drawScene(&game->scene);
		//drawHUD(&game->HUD);

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

void movePlayerHeight(Game* game, vec3 translation)
{
	Camera* camera = currentCamera;
	vec3 newPosition = addVec3(currentCamera->position, translation);

	float threshold = getVec3Length(translation) * 2;

	Ray ray = { normalizeVec3(translation), newPosition };
	float distance = getShortestSceneCollision(&game->scene, ray);
	if (distance < threshold)
	{
		return;
	}
	else
	{
		camera->position = newPosition;
		camera->view = lookMat4(camera->position, addVec3(camera->position, camera->front), camera->up);
	}
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
		FaceCollision f = {0};
		f.distance = FLT_MAX;
		Ray FUCKMYLIFE = {normalizeVec3(currentCamera->front), currentCamera->position};
		raycastBSP(game->scene.worldObjects[0]->collisionMesh, FUCKMYLIFE, &f);
		Face c = f.collidedFace;
		printf("Looked at face FUCK MY LIFE DUDE:\n"
			   "	%f %f %f\n	%f %f %f\n	%f %f %f\n",
				c.a.x,c.a.y,c.a.z,c.b.x,c.b.y,c.b.z,c.c.x,c.c.y,c.c.z);

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

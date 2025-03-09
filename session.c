#include "session.h"

void initWindow(GLFWwindow** window)
{
	printf("	...Initializing Window\n");
	if (!glfwInit())
	{
		printf("	...Failure to initialize GLFW, exiting (1)\n");
		exit(1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_POSITION_X, 640);
	glfwWindowHint(GLFW_POSITION_Y, 360);

	*window = glfwCreateWindow(1280, 720, "Shooter", NULL, NULL);
	if (!window)
	{
		printf("	...Failure to create window, exiting (1)\n");
		exit(1);
	}
	
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(*window);
	glfwSwapInterval(1);
	printf("		...Done\n");
}

void initGL()
{
	printf("	...Initializing OpenGL\n");
	GLenum glInitStatus = glewInit();
	if (glInitStatus != GLEW_OK)
	{
		printf("FAILED TO INITIALIZE GLEW\n");
	}
	glViewport(0, 0, 1280, 720);
	glEnable(GL_DEPTH_TEST);
	printf("		...Done\n");
}

void clearScreen()
{
	glClearColor(0.39f,0.58f,0.93f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

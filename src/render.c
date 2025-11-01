#include "render.h"

void initRenderer(Renderer* renderer)
{
	glGenFramebuffers(1, &renderer->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->FBO);

	// Create screen texture
	glGenTextures(1, &renderer->SCREENTEXTURE);
	glBindTexture(GL_TEXTURE_2D, renderer->SCREENTEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GAMEWIDTH, GAMEHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->SCREENTEXTURE, 0);

	// Create depth buffer
	glGenRenderbuffers(1, &renderer->DBO);
	glBindRenderbuffer(GL_RENDERBUFFER, renderer->DBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GAMEWIDTH, GAMEHEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderer->DBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Fullscreen Quad Setup
	float quadVertices[16] = {
	// Positions  // Tex Coords
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint VBO;
	glGenVertexArrays(1, &renderer->SCREENVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(renderer->SCREENVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	renderer->SCREENSHADER = loadShader("./shaders/screenShader.glsl");
}

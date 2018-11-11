
#include "GBuffer.hpp"

#include "../lumax.hpp"

GBuffer::GBuffer(){
	initFramebuffer();
}

void GBuffer::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glClearColor(0,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::unbind(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::copyDepth(){
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, 0, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void GBuffer::renderQuad(){
	quad->render();
}

void GBuffer::initFramebuffer(){
	// Framebuffer creation
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	std::cout << "Window size: " << lmx::getStaticWindowWidth() << "   " << lmx::getStaticWindowHeight() << std::endl;
	
	// Create texture for MRT rendering
	/*
	glGenTextures(1, &gWorldPos);
	glBindTexture(GL_TEXTURE_2D, gWorldPos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gWorldPos, 0);
	*/

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// MRT
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(3, attachments);

	// Create depth texture
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);
	
	// Check for completeness
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "ERROR: GBuffer could not be initialized" << std::endl;
		std::cout << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		assert(false);
	}

	// Make sure viewport is matching
	glViewport(0, 0, lmx::getStaticWindowWidth() * 2, lmx::getStaticWindowHeight() * 2);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Init quad
	static float vertices[] = {
		-1,-1,-1,1,1,1,1,-1
	};
	static float texCoords[] = {
		0,0,0,1,1,1,1,0
	};
	static int indices[] = {
		0,1,2,2,3,0
	};

	quad = new Model2D(vertices, 8, indices, 6, texCoords);
}

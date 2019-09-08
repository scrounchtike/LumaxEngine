
#include "RenderIntoTexture.hpp"

#include "../lumax.hpp"

RenderIntoTexture::RenderIntoTexture() : clear_color(Vec3(0,0,1))
{
	initFramebuffer();
}

void RenderIntoTexture::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void RenderIntoTexture::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderIntoTexture::setClearColor(const Vec3 &clear_color)
{
	this->clear_color = clear_color;
}

void RenderIntoTexture::clear()
{
	clearColor();
	clearDepth();
}

void RenderIntoTexture::clearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderIntoTexture::clearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderIntoTexture::blit()
{
	blitColor();
	copyDepth();
}

void RenderIntoTexture::blitColor()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, 0, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RenderIntoTexture::copyDepth()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, 0, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void RenderIntoTexture::initFramebuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Color attachment0 texture
	glGenTextures(1, &texture_output);
	glBindTexture(GL_TEXTURE_2D, texture_output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_output, 0);

	// Depth texture
	glGenTextures(1, &texture_depth);
	glBindTexture(GL_TEXTURE_2D, texture_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, lmx::getStaticWindowWidth()*2, lmx::getStaticWindowHeight()*2, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_depth, 0);

	// Check for completeness
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "ERROR: GBuffer could not be initialized" << std::endl;
		std::cout << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		assert(false);
	}

	// Make sure viewport is matching
	glViewport(0, 0, lmx::getStaticWindowWidth() * 2, lmx::getStaticWindowHeight() * 2);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

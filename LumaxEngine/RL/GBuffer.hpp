
#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include "Shader.hpp"
#include "../main.hpp"

#include "Model2D.hpp"

class GBuffer {
public:
	GBuffer();
	
	void bind();
	void unbind();
	void copyDepth();
	
	void renderQuad();

	GLuint getWorldPosTexture() { return gWorldPos; }
	GLuint getAlbedoTexture() { return gAlbedo; }
	GLuint getNormalTexture() { return gNormal; }
	GLuint getDepthTexture() { return gDepth; }
private:
	void initFramebuffer();
	GLuint fbo;

	GLuint gWorldPos;
	GLuint gAlbedo;
	GLuint gNormal;
	GLuint gDepth;

	Model2D* quad;
};

#endif

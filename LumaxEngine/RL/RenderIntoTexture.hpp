
#ifndef RENDER_INTO_TEXTURE_HPP
#define RENDER_INTO_TEXTURE_HPP

#include "Shader.hpp"
#include "Texture.hpp"

#include "../math.hpp"

class RenderIntoTexture {
public:
	RenderIntoTexture();

	void bind();
	void unbind();

	void setClearColor(const Vec3& clear_color);
	void clear();
	void clearColor();
	void clearDepth();

	void blit();
	void blitColor();
	void copyDepth();
	// TODO: Return a texture for color attachment 0
	Texture* resolve()
	{
		static Texture texture_resolve = Texture(texture_output);
		return &texture_resolve;
	}
private:
	void initFramebuffer();
	GLuint fbo;

	Vec3 clear_color;

	GLuint texture_output;
	// TODO: Ideally, depth would be done using a RenderBuffer object
	GLuint texture_depth;
};

#endif

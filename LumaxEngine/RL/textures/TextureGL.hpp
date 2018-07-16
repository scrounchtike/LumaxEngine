
#ifndef TEXTURE_GL_HPP
#define TEXTURE_GL_HPP

#include <string>
#include <cassert>

#include "../../RAL/UsingOpenGL.hpp"
#include "../../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

class TextureGL {
	friend class Texture;
public:
	TextureGL(GLuint id);
	TextureGL(float** data, unsigned int size, GLenum format = GL_RGBA, GLenum type = GL_FLOAT);
	~TextureGL();

	GLuint getTexture() { return textureID; }

	void bind() const;
private:
	bool initialize(float** data, unsigned int size, GLenum format, GLenum type);
	bool cleanUp();

	GLuint textureID;
};

#endif
#endif

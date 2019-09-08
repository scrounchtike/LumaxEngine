
#ifndef TEXTURE_GL_HPP
#define TEXTURE_GL_HPP

#include <string>
#include <cassert>

#include "../../RAL/UsingOpenGL.hpp"
#include "../../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

//
// 2D texture
// 

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

//
// Cubemap textures
//

class CubemapTextureGL {
public:
	friend class CubemapTexture;
private:
	CubemapTextureGL(GLuint id);
	CubemapTextureGL(float** data[6],
									 unsigned int size,
									 GLenum format = GL_RGBA,
									 GLenum type = GL_FLOAT);
	~CubemapTextureGL();

	GLuint getTexture() { return textureID; }

	void bind() const;
private:
	bool initialize(float** data[6],
									unsigned int size,
									GLenum format,
									GLenum type);
	bool cleanUp();
	
	GLuint textureID;
};

#endif
#endif

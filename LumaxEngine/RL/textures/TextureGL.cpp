
#include "TextureGL.hpp"
#include "../Texture.hpp"

#include "../../utils/stb_image.h"

#ifdef _USE_OPENGL

//
// 2D textures
//

TextureGL::TextureGL(GLuint id) : textureID(id)
{
}

TextureGL::TextureGL(float** data, unsigned int size, GLenum format, GLenum type)
{
	bool success = initialize(data, size, format, type);
	assert(success);
}

TextureGL::~TextureGL()
{
	bool success = cleanUp();
	assert(success);
}

void TextureGL::bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

bool TextureGL::initialize(float** data, unsigned int size, GLenum format, GLenum type)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, size, size, 0, format, type, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool TextureGL::cleanUp()
{
	return true; // TODO
}

//
// Cubemap Textures
//

CubemapTextureGL::CubemapTextureGL(GLuint id) : textureID(id)
{
}

CubemapTextureGL::CubemapTextureGL(float** data[6],
																	 unsigned int size,
																	 GLenum format,
																	 GLenum type)
{
	bool success = initialize(data, size, format, type);
	assert(success);
}

CubemapTextureGL::~CubemapTextureGL()
{
	bool success = cleanUp();
	assert(success);
}

void CubemapTextureGL::bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

bool CubemapTextureGL::initialize(float** data[6],
																	unsigned int size,
																	GLenum format,
																	GLenum type)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	
	for(GLuint i = 0; i < 6; ++i) // Assuming 6 faces for our cubemap
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, size, size, 0, format, type, data[i]);
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
	}
}

bool CubemapTextureGL::cleanUp()
{
	return true; // TODO
}

#endif

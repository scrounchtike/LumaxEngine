
#include "TextureGL.hpp"

#include "../../Utils/stb_image.h"

#ifdef _USE_OPENGL

TextureGL::TextureGL(GLuint id) : texture_ID(id) {

}

TextureGL::TextureGL(float** data, unsigned int size, GLenum format, GLenum type) {
	bool success = initialize(data, size, format, type);
	assert(success);
}

TextureGL::~TextureGL() {
	bool success = cleanUp();
	assert(success);
}

void TextureGL::bind() const {
	glBindTexture(GL_TEXTURE_2D, texture_ID);
}

bool TextureGL::initialize(float** data, unsigned int size, GLenum format, GLenum type) {
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, size, size, 0, format, type, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool TextureGL::cleanUp() {
	return true;
}

#endif
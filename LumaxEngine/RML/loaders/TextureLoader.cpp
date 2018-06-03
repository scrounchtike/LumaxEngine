
#include "TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../utils/stb_image.h"

#include "../../math/types.hpp"

#include "../ResourceManager.hpp"

Texture* TextureLoader::loadTexture(const std::string& filename){
#ifdef _USE_OPENGL
	return new Texture(loadTextureSTB(filename));
#elif defined _USE_DIRECTX11
	// TODO: DX11 textures support
#endif
}

#ifdef _USE_OPENGL

GLuint TextureLoader::loadTextureSTB(const std::string& filename) {
	int32 width, height, numComponents;
	uint8* data = stbi_load(filename.c_str(), &width, &height, &numComponents, 3);

	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return texID;
}

#elif defined _USE_DIRECTX11
#endif


#include "TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../utils/stb_image.h"

#include "../../math/types.hpp"

#include "../ResourceManager.hpp"

Texture* TextureLoader::loadTexture(const std::string& filename)
{
#ifdef _USE_OPENGL
	return new Texture(loadTextureSTB(filename));
#elif defined _USE_DIRECTX11
	// TODO: DX11 textures support
#endif
}

CubemapTexture* TextureLoader::loadCubemapTexture(const std::string& baseFolder,
																									const std::string filenames[6])
{
#ifdef _USE_OPENGL
	return new CubemapTexture(loadCubemapTextureSTB(baseFolder, filenames));
#endif
}

#ifdef _USE_OPENGL

GLuint TextureLoader::loadTextureSTB(const std::string& filename,
																		 unsigned depth)
{
	int32 width, height, numComponents;
	uint8* data = stbi_load(filename.c_str(), &width, &height, &numComponents, depth);

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

GLuint TextureLoader::loadCubemapTextureSTB(const std::string& baseFolder,
																						const std::string filenames[6],
																						unsigned depth)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	
	for(int i = 0; i < 6; ++i) // Assuming 6 face cubemaps
	{
		int32 width, height, numComponents;
		uint8* data = stbi_load((baseFolder + filenames[i]).c_str(), &width, &height, &numComponents, depth);

		GLenum format = (depth == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		stbi_image_free(data);
	}

	return textureID;
}

#elif defined _USE_DIRECTX11
#endif

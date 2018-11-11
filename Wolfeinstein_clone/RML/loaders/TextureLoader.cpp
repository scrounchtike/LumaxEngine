
#include "TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../utils/stb_image.h"

#include "../../math/types.hpp"

#include "../ResourceManager.hpp"
#include "../../lumax.hpp"

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
	uint8* data = stbi_load(filename.c_str(), &width, &height, &numComponents, 4);
	
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return texID;
}

GLuint TextureLoader::loadCubeTexture(const std::string *paths){
	GLuint texID;
	glGenTextures(1, &texID);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
  
	unsigned numPaths = 6;
	
	int width, height, numComponents;
	for(int i = 0; i < 6; ++i){
		std::string path = paths[i];
		std::cout << (lmx::getStaticResourceManager()->getResFolder() + path) << std::endl;
		unsigned char* data = stbi_load((lmx::getStaticResourceManager()->getResFolder() + path).c_str(), &width, &height, &numComponents, STBI_rgb_alpha);
		//TODO: Check that width and height match the input imageSize
    
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
  
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	

	return texID;
}

#elif defined _USE_DIRECTX11
#endif


#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <string>

#include "../../RL/Texture.hpp"

class TextureLoader {
	friend class ResourceManager;
private:
	static Texture* loadTexture(const std::string& filename);
	static CubemapTexture* loadCubemapTexture(const std::string& baseFolder, const std::string filenames[6]);
	
#ifdef _USE_OPENGL
	static GLuint loadTextureSTB(const std::string& filename,
															 unsigned depth = 3);
	static GLuint loadCubemapTextureSTB(const std::string& baseFolder,
																			const std::string filenames[6],
																			unsigned depth = 3);
#elif defined _USE_DIRECTX11
#endif
};

#endif

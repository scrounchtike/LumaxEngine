
#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <string>

#include "../../RL/Texture.hpp"

class TextureLoader {
	friend class ResourceManager;
private:
public:
	static Texture* loadTexture(const std::string& filename);
#ifdef _USE_OPENGL
	static GLuint loadTextureSTB(const std::string& filename);
	static GLuint loadCubeTexture(const std::string* paths);
#elif defined _USE_DIRECTX11
#endif
};

#endif

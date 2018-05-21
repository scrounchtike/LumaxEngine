
#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <string>

#include "../RL/textures/TextureGL.hpp"

class TextureLoader {
public:
#ifdef _USE_OPENGL
	static TextureGL* loadTextureSTB(const std::string& filename);
#elif defined _USE_DIRECTX11
	//TODO: DX11 Texture support
#endif
private:
};

#endif

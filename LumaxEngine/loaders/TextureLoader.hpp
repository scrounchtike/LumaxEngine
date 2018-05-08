
#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include "../utils/stb_image.h"
#include <string>

#include "../RL/TextureGL.hpp"

class TextureLoader {
public:
	static TextureGL* loadTextureSTB(const std::string& filename);
private:
};

#endif
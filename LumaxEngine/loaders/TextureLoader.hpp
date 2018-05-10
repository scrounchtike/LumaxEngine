
#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <string>

#include "../RL/TextureGL.hpp"

class TextureLoader {
public:
	static TextureGL* loadTextureSTB(const std::string& filename);
private:
};

#endif

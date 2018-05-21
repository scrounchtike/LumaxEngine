
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../RAL/buildDesc.hpp"

#include <string>

#ifdef _USE_OPENGL
class TextureGL;
#elif defined _USE_DIRECTX11
class TextureDX11;
#endif

class Texture {
public:
	Texture(const std::string& filename);
	Texture(const Texture& lhs);
	~Texture();

	void bind();
private:
	// Graphics API dependent Texture implementation
#ifdef _USE_OPENGL
	TextureGL* texture;
#elif defined _USE_DIRECTX11
	TextureDX11* texture;
#endif
};

#endif
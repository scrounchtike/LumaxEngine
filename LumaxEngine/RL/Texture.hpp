
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../RAL/buildDesc.hpp"

#include <string>

#ifdef _USE_OPENGL
//class TextureGL;
#include "textures/TextureGL.hpp"
#include "../RAL/UsingOpenGL.hpp"
#elif defined _USE_DIRECTX11
//class TextureDX11;
#include "textures/TextureDX11.hpp"
#endif

class Texture {
public:
#ifdef _USE_OPENGL
	Texture(GLuint texID);
#elif defined _USE_DIRECTX11
	// TODO: Make DX11 constructor
#endif
	Texture(const Texture& lhs);
	~Texture();

	unsigned getTextureID(){
#ifdef _USE_OPENGL
		return texture->getTexture();
#elif defined _USE_DIRECTX11
		return -1; // TODO: Make DX11 return unique ids
#endif
	}

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

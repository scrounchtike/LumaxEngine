
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
	
	unsigned getTextureID()
	{
		return texture->getTexture();
	}

	void bind() const;
private:
	// Graphics API dependent Texture implementation
#ifdef _USE_OPENGL
	TextureGL* texture;
#elif defined _USE_DIRECTX11
	TextureDX11* texture;
#endif
};

class CubemapTexture {
public:
#ifdef _USE_OPENGL
	CubemapTexture(GLuint texID);
#endif
	~CubemapTexture();

	unsigned getTextureID()
	{
		return cubemap->getTexture();
	}

	void bind() const;
private:
#ifdef _USE_OPENGL
	CubemapTextureGL* cubemap;
#endif
};

#endif

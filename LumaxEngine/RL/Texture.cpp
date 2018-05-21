
#include "Texture.hpp"

#include "../loaders/TextureLoader.hpp"

#ifdef _USE_OPENGL
#include "textures/TextureGL.hpp"
#elif defined _USE_DIRECTX11
#include "textures/TextureDX11.hpp"
#endif

Texture::Texture(const std::string& filename) {
#ifdef _USE_OPENGL
	texture = TextureLoader::loadTextureSTB(filename);
#elif defined _USE_DIRECTX11
	//texture = TextureLoader::loadTextureSTB(filename);
#endif
}

Texture::Texture(const Texture& lhs) {
#ifdef _USE_OPENGL
	texture = new TextureGL(*lhs.texture);
#elif defined _USE_DIRECTX11
	texture = new TextureDX11(*lhs.texture);
#endif
}

Texture::~Texture() {
	delete texture;
}

void Texture::bind() {
#ifdef _USE_OPENGL
	texture->bind();
#elif defined _USE_DIRECTX11
	// TODO: DX11 Textures
#endif
}
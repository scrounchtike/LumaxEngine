
#include "Texture.hpp"

#ifdef _USE_OPENGL
#include "textures/TextureGL.hpp"
#elif defined _USE_DIRECTX11
#include "textures/TextureDX11.hpp"
#endif

#ifdef _USE_OPENGL
Texture::Texture(GLuint texID) : texture(new TextureGL(texID)) {
}
#elif defined _USE_DIRECTX11
#endif

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


#include "Texture.hpp"

#ifdef _USE_OPENGL
#include "textures/TextureGL.hpp"
#elif defined _USE_DIRECTX11
#include "textures/TextureDX11.hpp"
#endif

//
// 2D textures
//

#ifdef _USE_OPENGL
Texture::Texture(GLuint texID) : texture(new TextureGL(texID))
{
}
#elif defined _USE_DIRECTX11
#endif

Texture::~Texture()
{
	delete texture;
}

void Texture::bind() const
{
#ifdef _USE_OPENGL
	texture->bind();
#elif defined _USE_DIRECTX11
	// TODO: DX11 Textures
#endif
}

//
// Cubemaps
//

#ifdef _USE_OPENGL
CubemapTexture::CubemapTexture(GLuint texID) : cubemap(new CubemapTextureGL(texID))
{
}
#endif

CubemapTexture::~CubemapTexture()
{
	delete cubemap;
}

void CubemapTexture::bind() const
{
#ifdef _USE_OPENGL
	cubemap->bind();
#endif
}


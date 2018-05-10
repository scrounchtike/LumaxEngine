
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "../math.hpp"

#include "TextureGL.hpp"
#include "ShaderGL.hpp"

#include "ShaderDX11.hpp"
#include "TextureDX11.hpp"

#ifdef _USE_OPENGL
typedef TextureGL Texture;
#elif defined _USE_DIRECTX11
typedef TextureDX11 Texture;
#endif

class Material {
public:
	Material(const Vec3& color);
	Material(Vec3* colors, int numColors);

	Material(Texture* texture);
	Material(Texture* texture, const Vec3& color, float blend = 0.5f);

	Vec3 getColor() const {
		return colors[colorIndex];
	}
	Texture* getTexture() const {
		return texture;
	}

#ifdef _USE_OPENGL
	void setShaderUniforms(ShaderGL* shader);
#elif defined _USE_DIRECTX11
	void setShaderUniforms(ShaderDX11* shader);
#endif
private:
	Texture* texture = nullptr;
	Vec3* colors;
public:
	int numColors = 1;
	int colorIndex = 0;
	float blend;
private:
	bool isColored = false;
};

#endif

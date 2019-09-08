
#include "Material.hpp"

#include "../main.hpp"

Material::Material() : ID(genID()) {}

Material::Material(const Vec3& color) : ID(genID()),
																				isColored(true),
																				colors(new Vec4[1])
{
	colors[0] = Vec4(color, 1.0);
}

Material::Material(const Vec4& color) : ID(genID()),
																				isColored(true),
																				colors(new Vec4[1]),
																				translucent(color.w != 1.0f)
{
	colors[0] = color;
}

Material::Material(Vec3* colors, int numColors) : ID(genID()),
																									isColored(true),
																									colors(new Vec4[numColors]),
																									numColors(numColors)
{
	for (int i = 0; i < numColors; ++i)
		this->colors[i] = Vec4(colors[i], 1.0);
}

Material::Material(Vec4* colors, int numColors) : ID(genID()),
																									isColored(true),
																									colors(new Vec4[numColors]),
																									numColors(numColors)
{
	for(int i = 0; i < numColors; ++i)
	{
		this->colors[i] = colors[i];
		if(colors[i].w != 1.0f) translucent = true;
	}
}

Material::Material(Texture* texture) : ID(genID()),
																			 isTextured(true)
{
	textures.insert(std::pair<std::string, Texture*>("texture0", texture));
}

Material::Material(Texture* texture, const Vec3& color, float blend)
	: ID(genID()),
		isTextured(true),
		isColored(true),
		isBlended(true),
		colors(new Vec4[1]),
		blend(blend)
{
	textures.insert(std::pair<std::string, Texture*>("texture0", texture));
	colors[0] = Vec4(color, 1.0);
}

Material::Material(Texture* texture, const Vec4& color, float blend)
	: ID(genID()),
		isTextured(true),
		isColored(true),
		isBlended(true),
		colors(new Vec4[1]),
		blend(blend),
		translucent(color.w != 1.0f)
{
	textures.insert(std::pair<std::string, Texture*>("texture0", texture));
	colors[0] = color;
}

#ifdef _USE_OPENGL
void Material::setShaderUniforms(Shader* shader) {
	if (isColored)
		shader->setUniform4f("color", colors[colorIndex]);
	if (hasTexture0()) {
		glEnable(GL_TEXTURE_2D);
		getTexture()->bind();
	}
}
#elif defined _USE_DIRECTX11
void Material::setShaderUniforms(Shader* shader) {
	if (isColored)
		shader->setUniform4f("color", colors[colorIndex]);
	if (hasTexture0()) {
		// TODO: DirectX 11 texturing
	}
}
#endif

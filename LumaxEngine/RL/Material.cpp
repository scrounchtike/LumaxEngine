
#include "Material.hpp"

Material::Material(const Vec3& color) : colors(new Vec3[1]), isColored(true) {
	colors[0] = color;
}

Material::Material(Vec3* colors, int numColors) : colors(new Vec3[numColors]), numColors(numColors), isColored(true) {
	for (int i = 0; i < numColors; ++i)
		this->colors[i] = colors[i];
}

Material::Material(Texture* texture) : texture(texture) {

}

Material::Material(Texture* texture, const Vec3& color, float blend)
	: texture(texture), colors(new Vec3[1]), blend(blend), isColored(true)
{
	colors[0] = color;
}

#ifdef _USE_OPENGL
void Material::setShaderUniforms(ShaderGL* shader) {
	if (isColored)
		shader->setUniform3f("color", colors[colorIndex]);
	if (texture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->getTexture());
	}
}
#elif defined _USE_DIRECTX11
void Material::setShaderUniforms(ShaderDX11* shader) {
	if (isColored)
		shader->setUniform3f("color", colors[colorIndex]);
	if (texture) {
		// TODO: DirectX 11 texturing
	}
}
#endif

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "../math.hpp"

#include "Texture.hpp"
#include "Shader.hpp"
#include "ShaderPipeline.hpp"

class Material {
public:
	const uint16 ID;

	Material();
	Material(const Vec3& color);
	Material(Vec3* colors, int numColors);
	
	Material(Texture* texture);
	Material(Texture* texture, const Vec3& color, float blend = 0.5f);

	void setColorIndex(int index){
		colorIndex = index;
	}
	void setColor(Vec3 color) {
		colors[0] = color;
	}
	void setColors(const std::vector<Vec3>& vcolors){
		numColors = vcolors.size();
		colors = new Vec3[numColors];
		for(int i = 0; i < numColors; ++i)
			colors[i] = vcolors[i];
	}
	Vec3 getColor() const {
		return colors[colorIndex];
	}
	Vec3* getColors(unsigned& size) const {
		size = numColors;
		return colors;
	}
	void setTexture(Texture* texture) {
		this->texture = texture;
	}
	Texture* getTexture() const {
		return texture;
	}

#ifdef _USE_OPENGL
	void setShaderUniforms(Shader* shader);
#elif defined _USE_DIRECTX11
	void setShaderUniforms(Shader* shader);
#endif

	bool isTextured = false;
	bool isColored = false;
	bool isBlended = false;
private:
	Texture* texture = nullptr;
	Vec3* colors;
public:
	int numColors = 1;
	int colorIndex = 0;
	float blend;
};

class MaterialPipeline{
public:
	const long ID; // Run-time type introspection
	
	MaterialPipeline(ShaderPipeline* pipeline, Material* material) : pipeline(pipeline), material(material), ID(rand()){
		
	}
	Material* getMaterial() const {
		return material;
	}
	ShaderPipeline* getPipeline() const {
		return pipeline;
	}
	void bind() const {
		pipeline->bind();
	}
private:
	ShaderPipeline* pipeline;
	Material* material;
};

struct PrintComponent {
	PrintComponent(std::string string) : string(string) {}
	std::string string;
};

#endif

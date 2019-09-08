
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
	Material(const Vec4& color);
	Material(Vec3* colors, int numColors);
	Material(Vec4* colors, int numColors);
	
	Material(Texture* texture);
	Material(Texture* texture, const Vec3& color, float blend = 0.5f);
	Material(Texture* texture, const Vec4& color, float blend = 0.5f);

	void setColorIndex(int index){
		colorIndex = index;
	}
	
	void setColor(Vec3 color)
	{
		isColored = true;
		colors = new Vec4[1];
		colors[0] = Vec4(color, 1.0);
	}
	
	void setColor(Vec4 color)
	{
		isColored = true;
		colors = new Vec4[1];
		colors[0] = color;
		if(colors[0].w != 1.0) translucent = true;
	}
	
	void setColors(const std::vector<Vec3>& vcolors)
	{
		isColored = true;
		numColors = vcolors.size();
		colors = new Vec4[numColors];
		for(int i = 0; i < numColors; ++i)
		{
			colors[i] = Vec4(vcolors[i], 1.0);
		}
	}
	
	void setColors(const std::vector<Vec4>& vcolors)
	{
		isColored = true;
		numColors = vcolors.size();
		colors = new Vec4[numColors];
		for(int i = 0; i < numColors; ++i)
		{
			colors[i] = vcolors[i];
			if(colors[i].w != 1.0) translucent = true;
		}
	}
	
	Vec4 getColor() const
	{
		return colors[colorIndex];
	}
	
	Vec4* getColors(unsigned& size) const
	{
		size = numColors;
		return colors;
	}
	
	bool hasTexture0()
	{
		return textures.find("texture0") != textures.end();
	}
	
	bool hasNormal0()
	{
		return textures.find("normal0") != textures.end();
	}
	
	void setTexture(Texture* texture, bool translucent = false)
	{
		isTextured = true;
		textures.insert(std::pair<std::string, Texture*>("texture0", texture));
		this->translucent = translucent;
	}
	
	Texture* getTexture() const
	{
	  if(textures.size() < 1)
			return nullptr;
		if(textures.size() > 1)
		{
			assert(!"Error: Tried to get texture when material contains multiple textures !");
			return nullptr;
		}
		return textures.begin()->second;
	}
	
	void setNormalTexture(Texture* texture)
	{
		isTextured = true;
		textures.insert(std::pair<std::string, Texture*>("normal0", texture));
	}
	
	Texture* getNormalTexture()
	{
		std::map<std::string, Texture*>::const_iterator it = textures.find("normal0");
		if(it != textures.end())
			return it->second;
		return nullptr;
	}
	
	void addTexture(const std::string& name, Texture* texture, bool translucent = false)
	{
		isTextured = true;
		std::map<std::string, Texture*>::iterator it = textures.find(name);
		if(it == textures.end())
		{
			textures.insert(std::pair<std::string, Texture*>(name, texture));
		}
		else
		{
			it->second = texture;
			// Should we also delete the texture ? (probably not .. no)
		}

		// If atleast one texture of material is translucent, then material
		// is translucent.
		if(translucent)
			this->translucent = true;
	}
	
	Texture* getTextureByName(const std::string& name)
	{
		std::map<std::string, Texture*>::iterator it = textures.find(name);
		if(it != textures.end())
			return it->second;
		return nullptr;
	}
	
	void setBlend(float blend)
	{
		isBlended = true;
		this->blend = blend;
	}
	
	float getBlend() const
	{
		return blend;
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
	std::map<std::string, Texture*> textures;
	Vec4* colors;
public:
	int numColors = 1;
	int colorIndex = 0;
	float blend;

	// This flag tells the renderer if the primitive using
	// this material has to be sorted first before being rendered
	// (Painter's algorithm)
	bool translucent = false;
};

class MaterialPipeline{
public:
	const long ID; // Run-time type introspection
	
	MaterialPipeline(ShaderPipeline* pipeline, Material* material) : ID(rand()), pipeline(pipeline), material(material){
		
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

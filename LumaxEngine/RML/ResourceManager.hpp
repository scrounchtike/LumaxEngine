
#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <fstream>
#include <string>
#include <map>
#include "../math/types.hpp"

class Model2D;
class Model3D;
class Material;
class ShaderPipeline;
class Shader;
class Animation;
class FullModel3D;
class Texture;

class ResourceManager {
public:
	ResourceManager(const std::string& resFile);

	// Add assets to manager
	void addModel2D(const std::string& name, Model2D* model);
	void addModel3D(const std::string& name, Model3D* model);
	void addModel3D(const std::string& name, const std::string& filename);
	void addTexture(const std::string& name, const std::string& filename);
	void addMaterial(const std::string& name, Material* material);

	void addShader(const std::string& name, const std::string& filename);
	void addVertexShader(const std::string& name, const std::string& filename);
	void addFragmentShader(const std::string& name, const std::string& filename);

	void addShaderPipeline(const std::string& name, ShaderPipeline* pipeline);

	// Get assets by name
	Shader* getShader(const std::string& filename);
	Shader* getVertexShader(const std::string& filename);
	Shader* getFragmentShader(const std::string& filename);

	ShaderPipeline* getShaderPipeline(const std::string& name);
	
	Model3D* getModel3D(const std::string& filename);
	Model3D* getAnimatedModel3D(const std::string& filename);
	Animation* getAnimation(const std::string& filename);
	FullModel3D* getFullModel3D(const std::string& filename);
	Model2D* getModel2D(const std::string& filename);
	Texture* getTexture(const std::string& filename);
	Material* getMaterial(const std::string& filename);
private:
	std::string getShaderFullName(const std::string& filename);
	
	std::map<std::string, Shader*> shaders;
	std::map<std::string, Shader*> vertexShaders;
	std::map<std::string, Shader*> fragmentShaders;

	std::map<std::string, ShaderPipeline*> shaderPipelines;
	
	std::map<std::string, Model3D*> models3D;
	std::map<std::string, FullModel3D*> fullModels3D;
	std::map<std::string, Model2D*> models2D;
	std::map<std::string, Texture*> textures;
	std::map<std::string, Material*> materials;

	std::string resFile;
};

#endif

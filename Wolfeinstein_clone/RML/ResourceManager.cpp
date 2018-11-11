
#include "ResourceManager.hpp"

#include "../RAL/Log.hpp"
#include "../RL/Shader.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/FullModel3D.hpp"
#include "../RL/Model2D.hpp"
#include "../RL/Texture.hpp"
#include "../RL/animations/Animation.hpp"
#include "../RL/Material.hpp"
#include "loaders/TextureLoader.hpp"
#include "loaders/ModelLoader.hpp"

#include "../RL/ShaderPipeline.hpp"

ResourceManager::ResourceManager(const std::string& resFile) : resFile(resFile) {

}

// Add assets to manager

void ResourceManager::addModel2D(const std::string& name, Model2D* model){
	models2D.insert(std::pair<std::string, Model2D*>(name, model));
}

void ResourceManager::addModel3D(const std::string& name, Model3D* model){
		models3D.insert(std::pair<std::string, Model3D*>(name, model));
}

void ResourceManager::addModel3D(const std::string& name, const std::string &filename){
	models3D.insert(std::pair<std::string, Model3D*>(name, ModelLoader::loadModel(resFile + "/models/" + filename)[0]));
}

void ResourceManager::addTexture(const std::string &name, const std::string &filename){
	textures.insert(std::pair<std::string, Texture*>(name, TextureLoader::loadTexture(resFile + "/textures/" + filename)));
}

void ResourceManager::addMaterial(const std::string &name, Material* material){
	materials.insert(std::pair<std::string, Material*>(name, material));
}

void ResourceManager::addShader(const std::string& name, const std::string &filename){
	shaders.insert(std::pair<std::string, Shader*>(name, new Shader(getShaderFullName(filename))));
}

void ResourceManager::addVertexShader(const std::string &name, const std::string &filename){
	shaders.insert(std::pair<std::string, Shader*>(name, new Shader(getShaderFullName(filename), GL_VERTEX_SHADER)));
}

void ResourceManager::addFragmentShader(const std::string &name, const std::string &filename){
	shaders.insert(std::pair<std::string, Shader*>(name, new Shader(getShaderFullName(filename), GL_FRAGMENT_SHADER)));
}

void ResourceManager::addShaderPipeline(const std::string &name, ShaderPipeline *pipeline){
	shaderPipelines.insert(std::pair<std::string, ShaderPipeline*>(name, pipeline));
}

// Get assets by name

Shader* ResourceManager::getShader(const std::string &filename){
	std::map<std::string, Shader*>::iterator it;
	assert(this);
	it = shaders.find(filename);
	if(it != shaders.end())
		return it->second;
	
  std::string fullName = getShaderFullName(filename);
	//Shader* shader = ShaderLoader::loadShader(fullName);
	
	Shader* shader = new Shader(fullName);
	shaders.insert(std::pair<std::string, Shader*>(filename, shader));
	return shader;
}

Shader* ResourceManager::getVertexShader(const std::string &filename){
	auto it = vertexShaders.find(filename);
	if(it != vertexShaders.end())
		return it->second;

	std::string fullName = getShaderFullName(filename);
	//Shader* shader = ShaderLoader::loadVertexShader(fullName);
	Shader* shader = new Shader(fullName, GL_VERTEX_SHADER);
	vertexShaders.insert(std::pair<std::string, Shader*>(filename, shader));
	return shader;
}

Shader* ResourceManager::getFragmentShader(const std::string &filename){
	auto it = fragmentShaders.find(filename);
	if(it != fragmentShaders.end())
		return it->second;

	std::string fullName = getShaderFullName(filename);
	//Shader* shader = ShaderLoader::loadFragmentShader(fullName);
	Shader* shader = new Shader(fullName, GL_FRAGMENT_SHADER);
	fragmentShaders.insert(std::pair<std::string, Shader*>(filename, shader));
	return shader;
}

ShaderPipeline* ResourceManager::getShaderPipeline(const std::string &name){
	auto it = shaderPipelines.find(name);
	if(it != shaderPipelines.end())
		return it->second;

	assert(false);
}

std::string ResourceManager::getShaderFullName(const std::string &filename){
	std::string APIFolder = "";
#ifdef _USE_OPENGL
	APIFolder = "GLSL";
#elif defined _USE_DIRECTX11
	APIFolder = "HLSL";
#endif
	std::string fullName = "shaders/" + APIFolder + "/" + filename;
	return fullName;
}

Model3D* ResourceManager::getModel3D(const std::string& filename){
	std::map<std::string, Model3D*>::iterator it = models3D.find(filename);
	if(it != models3D.end())
		return it->second;

	Model3D* model = ModelLoader::loadModel(resFile + "/models/" + filename)[0];
	models3D.insert(std::pair<std::string, Model3D*>(filename,model));
	return model;
}

Model3D* ResourceManager::getAnimatedModel3D(const std::string &filename){
	std::map<std::string, Model3D*>::iterator it = models3D.find(filename);
	if(it != models3D.end())
		return it->second;

	Model3D* model = ModelLoader::loadAnimatedModel(resFile + "/models/" + filename)[0];
	models3D.insert(std::pair<std::string, Model3D*>(filename,model));
	return model;
}

Animation* ResourceManager::getAnimation(const std::string& filename){
	return nullptr;
}

FullModel3D* ResourceManager::getFullModel3D(const std::string& filename){
	std::map<std::string, FullModel3D*>::iterator it = fullModels3D.find(filename);
	if(it != fullModels3D.end())
		return it->second;

	std::vector<Model3D*> models = ModelLoader::loadModel(resFile + "/models/" + filename);
	FullModel3D* fullmodel = new FullModel3D(models);
	fullModels3D.insert(std::pair<std::string, FullModel3D*>(filename, fullmodel));
	return fullmodel;
}

Model2D* ResourceManager::getModel2D(const std::string& filename){
	std::map<std::string, Model2D*>::iterator it = models2D.find(filename);
	if(it != models2D.end())
		return it->second;

	// No way to load a 2D model
	assert(false);
}

Texture* ResourceManager::getTexture(const std::string &filename){
	std::map<std::string, Texture*>::iterator it = textures.find(filename);
	if(it != textures.end())
		return it->second;

	Texture* texture = TextureLoader::loadTexture(resFile + "/textures/" + filename);
	textures.insert(std::pair<std::string, Texture*>(filename, texture));
	return texture;
}

 Material* ResourceManager::getMaterial(const std::string &filename){
	 auto it = materials.find(filename);
	 if(it != materials.end())
		 return it->second;

	 std::cerr << "Error: Material was not found in material map of Resource Manager" << std::endl;
	 assert(false);
 }

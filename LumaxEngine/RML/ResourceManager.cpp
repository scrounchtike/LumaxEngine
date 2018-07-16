
#include "ResourceManager.hpp"

ResourceManager::ResourceManager(const std::string& resFile) : resFile(resFile) {

}

Shader* ResourceManager::getShader(const std::string &filename){
	std::map<std::string, Shader*>::iterator it = shaders.find(filename);
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

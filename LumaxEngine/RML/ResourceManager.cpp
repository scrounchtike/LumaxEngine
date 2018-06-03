
#include "ResourceManager.hpp"

ResourceManager::ResourceManager(const std::string& resFile) : resFile(resFile) {

}

Shader* ResourceManager::getShader(const std::string &filename){
	std::map<std::string, Shader*>::iterator it = shaders.find(filename);
	if(it != shaders.end())
		return it->second;

	std::string APIFolder = "";
#ifdef _USE_OPENGL
	APIFolder = "GLSL";
#elif defined _USE_DIRECTX11
	APIFolder = "HLSL";
#endif
	std::string fullName = "shaders/" + APIFolder + "/" + filename;

	Shader* shader = ShaderLoader::loadShader(fullName);
	shaders.insert(std::pair<std::string, Shader*>(filename, shader));
	return shader;
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

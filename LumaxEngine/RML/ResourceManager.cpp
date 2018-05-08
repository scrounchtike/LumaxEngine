
#include "ResourceManager.hpp"

#include "../loaders/ShaderLoader.hpp"

ResourceManager::ResourceManager(const std::string& resFile) : resFile(resFile) {

}

Shader* ResourceManager::loadShader(const std::string& filename) {
	// Was shader already loaded?
	std::map<std::string, Shader*>::iterator it = shaders.find(filename);
	if (it != shaders.end())
		return it->second;

	// Else, load the shader
#ifdef _USE_DIRECTX11
	Shader* shader = ShaderLoader::loadShaderDX11("shaders/" + filename);
#elif defined _USE_OPENGL
	Shader* shader = ShaderLoader::loadShaderGL("shaders/" + filename);
#endif
	shaders.insert(std::pair<std::string, Shader*>(filename, shader));

	return shader;
}

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <fstream>
#include <string>
#include <map>

#include "../RAL/Log.hpp"

#ifdef _USE_DIRECTX11
#include "../RL/ShaderDX11.hpp"
#elif defined _USE_OPENGL
#include "../RL/ShaderGL.hpp"
#endif

class ResourceManager {
public:
	ResourceManager(const std::string& resFile);

	Shader* loadShader(const std::string& filename);
private:
	std::map<std::string, Shader*> shaders;

	std::string resFile;
};

#endif

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <fstream>
#include <string>
#include <map>

#include "../RAL/Log.hpp"
#include "../RL/Shader.hpp"

class ResourceManager {
public:
	ResourceManager(const std::string& resFile);

	Shader* loadShader(const std::string& filename);
private:
	std::map<std::string, Shader*> shaders;

	std::string resFile;
};

#endif
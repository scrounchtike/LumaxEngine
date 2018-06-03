
#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <fstream>
#include <string>
#include <map>

#include "../RAL/Log.hpp"
#include "../RL/Shader.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/FullModel3D.hpp"
#include "../RL/Model2D.hpp"
#include "../RL/Texture.hpp"

#include "loaders/ShaderLoader.hpp"
#include "loaders/TextureLoader.hpp"
#include "loaders/ModelLoader.hpp"

class ResourceManager {
public:
	ResourceManager(const std::string& resFile);

	Shader* getShader(const std::string& filename);
	Model3D* getModel3D(const std::string& filename);
	Model3D* getAnimatedModel3D(const std::string& filename);
	FullModel3D* getFullModel3D(const std::string& filename);
	Model2D* getModel2D(const std::string& filename);
	Texture* getTexture(const std::string& filename);
private:
	std::map<std::string, Shader*> shaders;
	std::map<std::string, Model3D*> models3D;
	std::map<std::string, FullModel3D*> fullModels3D;
	std::map<std::string, Model2D*> models2D;
	std::map<std::string, Texture*> textures;

	std::string resFile;
};

#endif

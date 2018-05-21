
#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>	
#include <string>
#include <vector>

#include "../RL/Model3D.hpp"

class ModelLoader {
public:
	static std::vector<Model3D*> loadModel(const std::string& filename);
private:
	static void processNode(aiNode* node, const aiScene* scene);
	static Model3D* processMesh(aiMesh* mesh, const aiScene* scene);

	static std::vector<Model3D*> models;
};

#endif
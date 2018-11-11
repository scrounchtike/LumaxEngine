
#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>	
#include <string>
#include <map>
#include <vector>

#include "../../RL/Model3D.hpp"
#include "../../RL/animations/Bone.hpp"
#include "../../RL/animations/Animation.hpp"
#include "../../RL/animations/Skeleton.hpp"

struct TrianglePolygon{
	Vec3 A, B, C;
	TrianglePolygon(Vec3 A, Vec3 B, Vec3 C) : A(A), B(B), C(C) {}
	Vec3& operator[](int index){
		if(index == 0)
			return A;
		if(index == 1)
			return B;
		return C;
	}
};

class ModelLoader {
public:
	friend class ResourceManager;

	static std::vector<TrianglePolygon*> loadModelData(const std::string& filename);
private:
	static std::vector<Model3D*> loadModel(const std::string& filename);
	// Load animated models
	static std::vector<Model3D*> loadAnimatedModel(const std::string& filename);


	static void processNodeData(aiNode* node, const aiScene* scene, std::vector<TrianglePolygon*>& polygons);
	static void processNode(aiNode* node, const aiScene* scene);

	static void processMeshData(aiMesh* mesh, const aiScene* scene, std::vector<TrianglePolygon*>& polygons);
	static Model3D* processMesh(aiMesh* mesh, const aiScene* scene);

	static void processSkeleton(aiNode* node, Bone* parent);

	static std::vector<Model3D*> models;

	// Animation temp data
	static std::map<std::string, std::pair<Bone*, unsigned> > boneMap;
};

#endif


#include "ModelLoader.hpp"

#include <iostream>
#include "../../RAL/Log.hpp"

#include "../ResourceManager.hpp"

std::vector<Model3D*> ModelLoader::models;

std::map<std::string, std::pair<Bone*, unsigned> > ModelLoader::boneMap;

std::vector<TrianglePolygon*> ModelLoader::loadModelData(const std::string& filename){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::println("Error: Could not import model with Assimp in loadModel function");
		Log::println("Assimp Error: " + std::string(importer.GetErrorString()));
		assert(false);
	}

	std::vector<TrianglePolygon*> polygons;
	processNodeData(scene->mRootNode, scene, polygons);
	return polygons;
}

std::vector<Model3D*> ModelLoader::loadModel(const std::string& filename) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::println("Error: Could not import model with Assimp in loadModel function");
		Log::println("Assimp Error: " + std::string(importer.GetErrorString()));
		assert(false);
	}

	models.clear();
	processNode(scene->mRootNode, scene);

	return models;
}

std::vector<Model3D*> ModelLoader::loadAnimatedModel(const std::string &filename){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	// Check if scene was loaded
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::println("Error: Could not import animated model with Assimp in loadAnimatedModel function");
		Log::println("Assimp error: " + std::string(importer.GetErrorString()));
		assert(false);
	}

	// Check that model has a skeleton
	// Huge assumption that model has only one mesh
	aiMesh* mesh = scene->mMeshes[0];
	if(!mesh->HasBones()){
		Log::println("Error: Tried to load animated model with no bones in LoadAnimatedModel");
		assert(false);
	}

	// Load Mesh data
	bool hasTexCoords = mesh->HasTextureCoords(0);
	bool hasNormals = mesh->HasNormals();
	bool hasFaces = mesh->HasFaces();
	bool hasTangents = mesh->HasTangentsAndBitangents();
	
	std::vector<float> vertices;
	vertices.resize(mesh->mNumVertices * 3);
	std::vector<int> indices;
	if(hasFaces)
		indices.resize(mesh->mNumFaces * 3);
	std::vector<float> texCoords;
	if(hasTexCoords)
		texCoords.resize(mesh->mNumVertices * 2);
	std::vector<float> normals;
	if(hasNormals)
		normals.resize(mesh->mNumVertices * 3);
	std::vector<float> tangents;
	if(hasTangents)
		tangents.resize(mesh->mNumVertices * 3);

	// Weights vertex data
	struct WeightInfo { // Max of 4 influences
		std::vector<float> boneIDs;
		std::vector<float> weights;
	};
	std::vector<WeightInfo> weights;
	weights.resize(mesh->mNumVertices);
	for(int i = 0; i < weights.size(); ++i){
		weights[i] = WeightInfo(); // Initialize weight info struct
		weights[i].boneIDs.assign(4, -1); // No bone ID by default
		weights[i].weights.assign(4, 0); // 0 weight by default
	}

	for(int i = 0; i < mesh->mNumVertices; ++i){
		vertices[i * 3 + 0] = mesh->mVertices[i].x;
		vertices[i * 3 + 1] = mesh->mVertices[i].y;
		vertices[i * 3 + 2] = mesh->mVertices[i].z;

		if(hasTexCoords){
			texCoords[i * 2 + 0] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}
		if(hasNormals){
			normals[i * 3 + 0] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}
		if(hasTangents){
			tangents[i * 3 + 0] = mesh->mTangents[i].x;
			tangents[i * 3 + 1] = mesh->mTangents[i].y;
			tangents[i * 3 + 2] = mesh->mTangents[i].z;
		}
	}
	if(hasFaces){
		for(int i = 0; i < mesh->mNumFaces; ++i){
			indices[i * 3 + 0] = mesh->mFaces[i].mIndices[2];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[0];
		}
	}

	// Data structures
	unsigned numBones = mesh->mNumBones;
	Skeleton* skeleton = new Skeleton(numBones);
	Animation* animation;

	for(int i = 0; i < numBones; ++i){
		// Load Bone Information
		aiBone* bone = mesh->mBones[i];
		std::string name = std::string(bone->mName.C_Str());
		aiMatrix4x4 _offsetMatrix = bone->mOffsetMatrix.Transpose();
		Mat4 localBoneSpaceTransform = Mat4(_offsetMatrix[0]);
		skeleton->bones[i] = new Bone(name);
		skeleton->bones[i]->index = i;
		skeleton->bones[i]->localBoneSpaceTransform = localBoneSpaceTransform;
		// Load bone into bone map
		boneMap.insert(std::pair<std::string, std::pair<Bone*, unsigned> >(name, std::pair<Bone*, unsigned>(skeleton->bones[i], i)));

		// Load weights
		for(int w = 0; w < bone->mNumWeights; ++w){
			aiVertexWeight vertexWeight = bone->mWeights[w];
			weights[vertexWeight.mVertexId].boneIDs.push_back(i);
			weights[vertexWeight.mVertexId].weights.push_back(vertexWeight.mWeight);
		}
	}

	// Set Skeleton root bone
	skeleton->root = skeleton->bones[0];
	Bone* root = skeleton->root;

	// Load Skeleton Hierarchy
	aiNode* rootNode = scene->mRootNode;
	for(int i = 0; i < rootNode->mNumChildren; ++i)
		processSkeleton(rootNode->mChildren[i], root);
	// Set Global Inverse Transform
	skeleton->globalInverseTransform = Mat4(rootNode->mTransformation.Inverse().Transpose()[0]);

	// Load animation
	// Assuming only one animation is present
	aiAnimation* _anim = scene->mAnimations[0];
	double duration = _anim->mDuration;
	double ticksPerSecond = _anim->mTicksPerSecond;

	// Check that numChannels matches number of bones
	assert(numBones == _anim->mNumChannels);

	// Initialize keyframes to number of keyframes
	unsigned numKeyframes = _anim->mChannels[0]->mNumPositionKeys;
	animation = new Animation(duration, ticksPerSecond, numBones);
	animation->keyframes.resize(numKeyframes);
	animation->timeStamps.resize(numKeyframes);
	for(int i = 0; i < numKeyframes; ++i)
		animation->keyframes[i] = Keyframe(numBones);

	// Load bone keyframes
	for(int i = 0; i < _anim->mNumChannels; ++i){
		aiNodeAnim* boneAnimation = _anim->mChannels[i];
		std::string boneName = std::string(boneAnimation->mNodeName.C_Str());
		std::map<std::string, std::pair<Bone*, unsigned> >::iterator it = boneMap.find(boneName);
		// Assert that we found a bone
		if(it == boneMap.end()){
			Log::println("Error: Bone could not be found in bone map during keyframe loading of node: " + boneName);
			assert(false);
		}

		// Assert that numKeyframes matches for bone and between position and rotation keyframes
		assert(numKeyframes == boneAnimation->mNumPositionKeys);
		assert(numKeyframes == boneAnimation->mNumRotationKeys);
		
		unsigned boneID = it->second.second;

		// Find positions and rotations for every keyframe
		for(int k = 0; k < numKeyframes; ++k){
			// Assert that timestamp matches between pos and rot
			double time = boneAnimation->mPositionKeys[k].mTime;
			assert(time == boneAnimation->mRotationKeys[k].mTime);
			
			// Position of the bone
			aiVector3D _pos = boneAnimation->mPositionKeys[k].mValue;
			Vec3 position = Vec3(_pos.x, _pos.y, _pos.z);
			
			// Rotation of the bone
			aiQuaternion _rot = boneAnimation->mRotationKeys[k].mValue;
			Quaternion rotation = Quaternion(_rot.x, _rot.y, _rot.z, _rot.w);
			// Add timestamp to global time stamps
			animation->timeStamps[k] = time;
			//Load bone transformation into keyframe
			animation->keyframes[k].boneTransformations[boneID] = BoneTransformation(position, rotation);
		}
	}
	
	// Create 3D Model
	Model3D* model;
	if(hasFaces){
		if(hasNormals)
			model = new Model3D(vertices, indices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3D(vertices, indices, texCoords);
		else
			model = new Model3D(vertices, indices);
	}else{
		if(hasNormals)
			model = new Model3D(vertices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3D(vertices, texCoords);
		else
			model = new Model3D(vertices);
	}
	// Check that at max 4 weights are assigned per vertex
	std::vector<float> boneIDs;
	boneIDs.resize(mesh->mNumVertices * 4);
	std::vector<float> skinWeights;
	weights.resize(mesh->mNumVertices * 4);
	for(int w = 0; w < weights.size(); ++w){
		WeightInfo weight = weights[w];
		assert(weight.boneIDs.size() <= 4);
		assert(weight.weights.size() <= 4);
		assert(weight.weights.size() == weight.boneIDs.size());
		unsigned size = weight.boneIDs.size();
		for(int i = 0; i < size; ++i){
			boneIDs[w * 4 + i] = weight.boneIDs[i];
			skinWeights[w * 4 + i] = weight.weights[i];
		}
		for(int i = size; i < 4; ++i){
			boneIDs[w * 4 + i] = 0;
			skinWeights[w * 4 + i] = 0;
		}
	}
	// Adding animation data to model
	model->addAnimationWeights(&boneIDs[0], &skinWeights[0], weights.size());

	std::vector<Model3D*> models;
	models.push_back(model);
	return models;
}

void ModelLoader::processNodeData(aiNode* node, const aiScene* scene, std::vector<TrianglePolygon*>& polygons){
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMeshData(mesh, scene, polygons);
	}
	for (int i = 0; i < node->mNumChildren; ++i) {
		processNodeData(node->mChildren[i], scene, polygons);
	}
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		models.push_back(processMesh(mesh, scene));
	}
	for (int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

void ModelLoader::processMeshData(aiMesh* mesh, const aiScene* scene, std::vector<TrianglePolygon*>& polygons){
	std::vector<float> vertices;
	vertices.resize(mesh->mNumVertices * 3);
	std::vector<int> indices;
	indices.resize(mesh->mNumFaces * 3);
	std::vector<float> texCoords;
	texCoords.resize(mesh->mNumVertices * 2);
	std::vector<float> normals;
	normals.resize(mesh->mNumVertices * 3);

	bool hasNormals, hasTexCoords, hasIndices = true;
	hasTexCoords = mesh->HasTextureCoords(0);
	hasNormals = mesh->HasNormals();

	//std::cout << "vertices data: " << std::endl;
	//std::cout << "num = " << mesh->mNumVertices << std::endl;
	
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		vertices[i * 3 + 0] = mesh->mVertices[i].x;
		vertices[i * 3 + 1] = mesh->mVertices[i].y;
		vertices[i * 3 + 2] = mesh->mVertices[i].z;
		//Vec3 vertex = Vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		//vertex.print();

		//if (hasTexCoords) {
		//	texCoords[i * 2 + 0] = mesh->mTextureCoords[0][i].x;
		//	texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		//}
		//if (hasNormals) {
		//	normals[i * 3 + 0] = mesh->mNormals[i].x;
		//	normals[i * 3 + 1] = mesh->mNormals[i].y;
		//	normals[i * 3 + 2] = mesh->mNormals[i].z;
		//}

		
	}

	//std::cout << std::endl << "print vertices: " << std::endl << std::endl;
	//for(int i = 0 ; i < vertices.size()/3; ++i){
	//	Vec3 vertex = Vec3(vertices[i*3+0], vertices[i*3+1], vertices[i*3+2]);
	//	vertex.print();
	//}
	
	std::cout << std::endl << "Connecting vertices: " << std::endl << std::endl;
	std::cout << "num faces = " << mesh->mNumFaces << std::endl;
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		Vec3 A = Vec3(vertices[mesh->mFaces[i].mIndices[2]*3+0], vertices[mesh->mFaces[i].mIndices[2]*3+1], vertices[mesh->mFaces[i].mIndices[2]*3+2]);
		Vec3 B = Vec3(vertices[mesh->mFaces[i].mIndices[1]*3+0], vertices[mesh->mFaces[i].mIndices[1]*3+1], vertices[mesh->mFaces[i].mIndices[1]*3+2]);
		Vec3 C = Vec3(vertices[mesh->mFaces[i].mIndices[0]*3+0], vertices[mesh->mFaces[i].mIndices[0]*3+1], vertices[mesh->mFaces[i].mIndices[0]*3+2]);
		//std::cout << "triangle: " << mesh->mFaces[i].mIndices[2] << " " << mesh->mFaces[i].mIndices[1] << " " << mesh->mFaces[i].mIndices[0] << std::endl;
		//A.print();
		//B.print();
		//C.print();
		polygons.push_back(new TrianglePolygon(A, B, C));
	}
}

Model3D* ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<float> vertices;
	vertices.resize(mesh->mNumVertices * 3);
	std::vector<int> indices;
	indices.resize(mesh->mNumFaces * 3);
	std::vector<float> texCoords;
	texCoords.resize(mesh->mNumVertices * 2);
	std::vector<float> normals;
	normals.resize(mesh->mNumVertices * 3);

	bool hasNormals, hasTexCoords, hasIndices = true;
	hasTexCoords = mesh->HasTextureCoords(0);
	hasNormals = mesh->HasNormals();

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		vertices[i * 3 + 0] = mesh->mVertices[i].x;
		vertices[i * 3 + 1] = mesh->mVertices[i].y;
		vertices[i * 3 + 2] = mesh->mVertices[i].z;

		if (hasTexCoords) {
			texCoords[i * 2 + 0] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}
		if (hasNormals) {
			normals[i * 3 + 0] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}
	}
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[2];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[0];
	}

	Model3D* model;
	if(hasIndices){
		if(hasNormals)
			model = new Model3D(vertices, indices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3D(vertices, indices, texCoords);
		else
			model = new Model3D(vertices, indices);
	}else{
		if(hasNormals)
			model = new Model3D(vertices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3D(vertices, texCoords);
		else
			model = new Model3D(vertices);
	}
	return model;
}

void ModelLoader::processSkeleton(aiNode *node, Bone* parent){
	// Load bone into hierarchy
	std::string name = std::string(node->mName.C_Str());
	// Check that the bone corresponds to a bone name
	std::map<std::string, std::pair<Bone*, unsigned> >::iterator it = boneMap.find(name);
	if(it == boneMap.end())
		return; // No name found = not a bone

	// We found a matching bone
	Bone* bone = it->second.first;

	// Load parent space transformation matrix
	aiMatrix4x4 _transform = node->mTransformation.Transpose();
	Mat4 parentSpaceTransform = Mat4(_transform[0]);
	bone->parentSpaceTransform = parentSpaceTransform;

	// Add bone as child to parent bone
	parent->children.push_back(bone);
	
	// Load children of bone
	for(int i = 0; i < node->mNumChildren; ++i)
		processSkeleton(node->mChildren[i], bone); // Recursive loading of nodes
}

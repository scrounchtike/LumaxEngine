
#include "ModelLoader.hpp"

#include <iostream>
#include "../RAL/Log.hpp"

std::vector<Model3D*> ModelLoader::models;

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

void ModelLoader::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		models.push_back(processMesh(mesh, scene));
	}
	for (int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

#ifdef _USE_OPENGL
Model3DGL* ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
#elif defined _USE_DIRECTX11
Model3DDX11* ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
#endif
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

#ifdef _USE_OPENGL
	Model3DGL* model;
	if(hasIndices){
		if(hasNormals)
			model = new Model3DGL(vertices, indices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3DGL(vertices, indices, texCoords);
		else
			model = new Model3DGL(vertices, indices);
	}else{
		if(hasNormals)
			model = new Model3DGL(vertices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3DGL(vertices, texCoords);
		else
			model = new Model3DGL(vertices);
	}
#elif defined _USE_DIRECTX11
	Model3DDX11* model;
	if(hasIndices){
		if(hasNormals)
			model = new Model3DDX11(vertices, indices, texCoords, normals);
		else if(hasTexCoords)
			model = new Model3DDX11(vertices, indices, texCoords);
		else
			model = new Model3DGL(vertices, indices);
	}else{
		if(hasNormals)
			model = new Model3DDX11(vertices, texCoords, normals);
		else if(hasTexCoords)
			model = newModel3DDX11(vertices, texCoords);
		else
			model = new Model3DDX11(vertices);
	}
#endif
	return model;
}

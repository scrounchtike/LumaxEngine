
#include "Model3D.hpp"

#ifdef _USE_OPENGL
#include "models3D/Model3DGL.hpp"
#elif defined _USE_DIRECTX11
#include "models3D/Model3DDX11.hpp"
#endif

Model3D::Model3D(const float* vertices, int numVertices) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const float* texCoords) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, texCoords);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, texCoords);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const float* texCoords, const float* normals) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, texCoords, normals);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, texCoords, normals);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, texCoords, normals, tangents);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, texCoords, normals, tangents);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const int* indices, int numIndices) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, indices, numIndices);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, indices, numIndices);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, indices, numIndices, texCoords);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, indices, numIndices, texCoords);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals)
	: ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, indices, numIndices, texCoords, normals);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, indices, numIndices, texCoords, normals);
#endif
}

Model3D::Model3D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(vertices, numVertices, indices, numIndices, texCoords, normals, tangents);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(vertices, numVertices, indices, numIndices, texCoords, normals, tangents);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<float>& texCoords) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &texCoords[0]);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &texCoords[0]);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &texCoords[0], &normals[0]);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &texCoords[0], &normals[0]);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &texCoords[0], &normals[0], &tangents[0]);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &texCoords[0], &normals[0], &tangents[0]);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<int>& indices) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &indices[0], indices.size());
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &indices[0], indices.size());
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &indices[0], indices.size(), &texCoords[0]);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &indices[0], indices.size(), &texCoords[0]);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &indices[0], indices.size(), &texCoords[0], &normals[0]);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &indices[0], indices.size(), &texCoords[0], &normals[0]);
#endif
}

Model3D::Model3D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents) : ID(genID()){
#ifdef _USE_OPENGL
	model = new Model3DGL(&vertices[0], vertices.size()/3, &indices[0], indices.size(), &texCoords[0], &normals[0], &tangents[0]);
#elif defined _USE_DIRECTX11
	model = new Model3DDX11(&vertices[0], vertices.size()/3, &indices[0], indices.size(), &texCoords[0], &normals[0], &tangents[0]);
#endif
}

Model3D::~Model3D() {
	delete model;
}

void Model3D::addAnimationWeights(const float *boneIDs, const float *weights, unsigned int numWeights){
#ifdef _USE_OPENGL
	model->addAnimationWeights(boneIDs, weights, numWeights);
#elif defined _USE_DIRECTX11
	// TODO: Support animation for DX11 3D models
#endif
}

bool Model3D::isIndexed() const { return model->isIndexed; }
bool Model3D::isTextured() const { return model->isTextured; }
bool Model3D::hasNormals() const { return model->hasNormals; }
bool Model3D::hasTangents() const { return model->hasTangents; }
bool Model3D::isAnimated() const { return model->isAnimated; }
unsigned int Model3D::getNumVertices() const { return model->numVertices; }
unsigned int Model3D::getNumIndices() const { return model->numIndices; }

void Model3D::update() {
	model->update();
}

void Model3D::render() const {
	model->render();
}

void Model3D::bindForRender() const {
	model->bindForRender();
}

void Model3D::renderBuffersOnly() const {
	model->renderBuffersOnly();
}

void Model3D::unbindForRender() const {
	model->unbindForRender();
}

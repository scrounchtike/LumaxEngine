
#include "Model2D.hpp"

#ifdef _USE_OPENGL
#include "models2D/Model2DGL.hpp"
#elif defined _USE_DIRECTX11
#include "models2D/Model2DDX11.hpp"
#endif

Model2D::Model2D(const float* vertices, int numVertices) {
#ifdef _USE_OPENGL
	model = new Model2DGL(vertices, numVertices);
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(vertices, numVertices);
#endif
}

Model2D::Model2D(const float* vertices, int numVertices, const float* texCoords) {
#ifdef _USE_OPENGL
	model = new Model2DGL(vertices, numVertices, texCoords);
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(vertices, numVertices, texCoords);
#endif
}

Model2D::Model2D(const float* vertices, int numVertices, const int* indices, int numIndices) {
#ifdef _USE_OPENGL
	model = new Model2DGL(vertices, numVertices, indices, numIndices);
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(vertices, numVertices, indices, numIndices);
#endif
}

Model2D::Model2D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) {
#ifdef _USE_OPENGL
	model = new Model2DGL(vertices, numVertices, indices, numIndices, texCoords);
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(vertices, numVertices, indices, numIndices, texCoords);
#endif
}

Model2D::Model2D(const std::vector<float>& vertices) {
#ifdef _USE_OPENGL
	model = new Model2DGL(&vertices[0], vertices.size());
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(&vertices[0], vertices.size());
#endif
}

Model2D::Model2D(const std::vector<float>& vertices, const std::vector<float>& texCoords) {
#ifdef _USE_OPENGL
	model = new Model2DGL(&vertices[0], vertices.size(), &texCoords[0]);
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(&vertices[0], vertices.size(), &texCoords[0]);
#endif
}

Model2D::Model2D(const std::vector<float>& vertices, const std::vector<int>& indices) {
#ifdef _USE_OPENGL
	model = new Model2DGL(&vertices[0], vertices.size(), &indices[0], indices.size());
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(&vertices[0], vertices.size(), &indices[0], indices.size());
#endif
}

Model2D::Model2D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords) {
#ifdef _USE_OPENGL
	model = new Model2DGL(&vertices[0], vertices.size(), &indices[0], indices.size(), &texCoords[0]);
#elif defined _USE_DIRECTX11
	model = new Model2DDX11(&vertices[0], vertices.size(), &indices[0], indices.size(), &texCoords[0]);
#endif
}

Model2D::~Model2D() {
	delete model;
}

bool Model2D::isIndexed() const { return model->isIndexed; }
bool Model2D::isTextured() const { return model->isTextured; }
unsigned int Model2D::getNumVertices() const { return model->numVertices; }
unsigned int Model2D::getNumIndices() const { return model->numIndices; }

void Model2D::update() {
	model->update();
}

void Model2D::render() const {
	model->render();
}

void Model2D::bindForRender() const {
	model->bindForRender();
}

void Model2D::renderBuffersOnly() const {
	model->renderBuffersOnly();
}

void Model2D::unbindForRender() const {
	model->unbindForRender();
}

#ifndef MODEL_2D_HPP
#define MODEL_2D_HPP

#include <vector>

#include "../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL
//class Model2DGL;
#include "models2D/Model2DGL.hpp"
#elif defined _USE_DIRECTX11
//class Model2DDX11;
#include "models2D/Model2DDX11.hpp"
#endif

class Model2D {
public:
	Model2D(){}
	Model2D(const float* vertices, int numVertices);
	Model2D(const float* vertices, int numVertices, const float* texCoords);
	Model2D(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model2D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);

	Model2D(const std::vector<float>& vertices);
	Model2D(const std::vector<float>& vertices, const std::vector<float>& texCoords);
	Model2D(const std::vector<float>& vertices, const std::vector<int>& indices);
	Model2D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords);

	~Model2D();

	bool isIndexed() const;
	bool isTextured() const;
	unsigned int getNumVertices() const;
	unsigned int getNumIndices() const;

	void update();
	void render() const;
	void renderInstanced(unsigned int count) const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;

	unsigned getGeometryID() const {
#ifdef _USE_OPENGL
		return model->vaoID;
#elif defined _USE_DIRECTX11
		return -1; // TODO: Unique geometry id for DX11 meshes
#endif
	}
private:
	// API dependent Model2D implementation
#ifdef _USE_OPENGL
	Model2DGL* model;
#elif defined _USE_DIRECTX11
	Model2DDX11* model;
#endif
};

#endif

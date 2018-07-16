
#ifndef MODEL_3D_HPP
#define MODEL_3D_HPP

#include <vector>

#include "../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL
//class Model3DGL;
#include "models3D/Model3DGL.hpp"
#elif defined _USE_DIRECTX11
//class Model3DDX11;
#include "models3D/Model3DDX11.hpp"
#endif

class Model3D {
public:
	Model3D(const float* vertices, int numVertices);
	Model3D(const float* vertices, int numVertices, const float* texCoords);
	Model3D(const float* vertices, int numVertices, const float* texCoords, const float* normals);
	Model3D(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents);
	Model3D(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model3D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);
	Model3D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals);
	Model3D(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents);

	Model3D(const std::vector<float>& vertices);
	Model3D(const std::vector<float>& vertices, const std::vector<float>& texCoords);
	Model3D(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals);
	Model3D(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents);
	Model3D(const std::vector<float>& vertices, const std::vector<int>& indices);
	Model3D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords);
	Model3D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals);
	Model3D(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents);

	~Model3D();

	// Animation support
	void addAnimationWeights(const float* boneIDs, const float* weights, unsigned numWeights);

	bool isIndexed() const;
	bool isTextured() const;
	bool hasNormals() const;
	bool hasTangents() const;
	bool isAnimated() const;
	unsigned int getNumVertices() const;
	unsigned int getNumIndices() const;

	void update();
	void render() const;

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
	// Model3D API specific implementation
#ifdef _USE_OPENGL
	Model3DGL* model;
#elif defined _USE_DIRECTX11
	Model3DDX11* model;
#endif
};

#endif

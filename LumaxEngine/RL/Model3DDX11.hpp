
#ifndef MODEL_3D_DX11_HPP
#define MODEL_3D_DX11_HPP

#include <vector>

#include "../RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11

#include "../RAL/UsingDX11.hpp"
#include "../RAL/RenderingContextDX11.hpp"

class Model3DDX11 {
public:
	Model3DDX11(const float* vertices, int numVertices);
	Model3DDX11(const float* vertices, int numVertices, const float* texCoords);
	Model3DDX11(const float* vertices, int numVertices, const float* texCoords, const float* normals);
	Model3DDX11(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents);
	Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);
	Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals);
	Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents);

	Model3DDX11(const std::vector<float>& vertices);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<float>& texCoords);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals);
	Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents);

	~Model3DDX11();

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	RenderingContextDX11* dx11;

	void initialize(const float* vertices, int numVertices);
	void initialize(const float* vertices, int numVertices, const float* texCoords);
	void initialize(const float* vertices, int numVertices, const float* texCoords, const float* normals);
	void initialize(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents);

	void cleanUp();

	bool isTextured, isIndexed, hasNormals, hasTangents;
	unsigned int numVertices, numIndices;
	unsigned int strideSize;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* texCoordBuffer;
	ID3D11Buffer* normalBuffer;
	ID3D11Buffer* tangentBuffer;

	void createBuffer(const float* data, int numVertices, ID3D11Buffer** buffer);
	void createIndexBuffer(const int* indices, int numIndices);
};

#endif
#endif
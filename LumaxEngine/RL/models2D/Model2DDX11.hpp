
#ifndef MODEL_2D_DX11_HPP
#define MODEL_2D_DX11_HPP

#include "../../RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11

#include <vector>

#include "../../RAL/UsingDX11.hpp"
#include "../../RAL/RenderingContextDX11.hpp"

class Model2DDX11 {
public:
	int strideSize;
	bool isIndexed = false;
	bool isTextured = false;
	unsigned int numVertices, numIndices;

	Model2DDX11(const float* vertices, int numVertices);
	Model2DDX11(const float* vertices, int numVertices, const float* texCoords);
	Model2DDX11(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model2DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoord);

	~Model2DDX11();

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	void interleaveData(std::vector<float>& result, const float* vertices, int numVertices, const float* texCoords);
	void createInterleavedBuffer(const float* vertexData, int numVertices, int strideSize);
	void createIndexBuffer(const int* indices, int numIndices);

	// Rendering Context
	RenderingContextDX11* dx11;

	// Buffers
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* texCoordBuffer;
	ID3D11Buffer* indexBuffer;
};

#endif
#endif
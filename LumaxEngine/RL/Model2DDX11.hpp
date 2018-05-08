
#ifndef MODEL_2D_DX11_HPP
#define MODEL_2D_DX11_HPP

#include "../RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11

#include <vector>

#include "Model2D.hpp"
#include "../RAL/UsingDX11.hpp"
#include "../RAL/RenderingContextDX11.hpp"

class Model2DDX11 : public Model2D {
public:
	Model2DDX11(float* vertices, int numVertices);
	Model2DDX11(float* vertices, int numVertices, float* texCoords);
	Model2DDX11(float* vertices, int numVertices, int* indices, int numIndices);
	Model2DDX11(float* vertices, int numVertices, int* indices, int numIndices, float* texCoord);

	Model2DDX11(std::vector<float>& vertices);
	Model2DDX11(std::vector<float>& vertices, std::vector<float>& texCoords);
	Model2DDX11(std::vector<float>& vertices, std::vector<int>& indices);
	Model2DDX11(std::vector<float>& vertices, std::vector<int>& indices, std::vector<float>& texCoords);

	~Model2DDX11();

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	void initialize(float* vertices, int numVertices);
	void initialize(float* vertices, int numVertices, float* texCoords);
	void initialize(float* vertices, int numVertices, int* indices, int numIndices);
	void initialize(float* vertices, int numVertices, int* indices, int numIndices, float* texCoords);

	void cleanUp();

	void interleaveData(std::vector<float>& result, float* vertices, int numVertices, float* texCoords);
	void createInterleavedBuffer(float* vertexData, int numVertices, int strideSize);
	void createIndexBuffer(int* indices, int numIndices);

	// Rendering Context
	RenderingContextDX11* dx11;

	// State variables
	int strideSize;
	bool isIndexed = false;
	bool isTextured = false;
	unsigned int numVertices, numIndices;

	// Buffers
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* texCoordBuffer;
	ID3D11Buffer* indexBuffer;
};

#endif
#endif

#include "Model2DDX11.hpp"

#ifdef _USE_DIRECTX11

#include "../../main.hpp"

Model2DDX11::Model2DDX11(const float* vertices, int numVertices)
	: isIndexed(false), isTextured(false), numVertices(numVertices), numIndices(0), dx11(getStaticRenderingContextDX11()), strideSize(2)
{
	createInterleavedBuffer(vertices, numVertices, strideSize);
}

Model2DDX11::Model2DDX11(const float* vertices, int numVertices, const float* texCoords) 
	: isIndexed(false), isTextured(true), numVertices(numVertices), numIndices(0), dx11(getStaticRenderingContextDX11()), strideSize(4) 
{
	std::vector<float> vertexData;
	interleaveData(vertexData, vertices, numVertices, texCoords);
	createInterleavedBuffer(&vertexData[0], numVertices, strideSize);
}

Model2DDX11::Model2DDX11(const float* vertices, int numVertices, const int* indices, int numIndices) 
	: isIndexed(true), isTextured(false), numVertices(numVertices), numIndices(0), dx11(getStaticRenderingContextDX11()), strideSize(2) 
{
	createInterleavedBuffer(vertices, numVertices, strideSize);
	createIndexBuffer(indices, numIndices);
}

Model2DDX11::Model2DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) 
	: isIndexed(true), isTextured(true), numVertices(numVertices), numIndices(0), dx11(getStaticRenderingContextDX11()), strideSize(4) 
{
	std::vector<float> vertexData;
	interleaveData(vertexData, vertices, numVertices, texCoords);
	createInterleavedBuffer(&vertexData[0], numVertices, strideSize);
	createIndexBuffer(indices, numIndices);
}

Model2DDX11::~Model2DDX11() { 
	if (vertexBuffer) {
		vertexBuffer->Release();
		vertexBuffer = 0;
	}
	if (texCoordBuffer) {
		texCoordBuffer->Release();
		texCoordBuffer = 0;
	}
	if (indexBuffer) {
		indexBuffer->Release();
		indexBuffer = 0;
	}
}

void Model2DDX11::update() {

}

void Model2DDX11::render() const {
	unsigned int stride = sizeof(float) * strideSize;
	unsigned int offset = 0;

	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	if (isIndexed)
		dx11->getDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (isIndexed)
		dx11->getDeviceContext()->DrawIndexed(numIndices, 0, 0);
	else
		dx11->getDeviceContext()->Draw(numVertices, 0);
}

void Model2DDX11::bindForRender() const {
	unsigned int stride = sizeof(float) * strideSize;
	unsigned int offset;

	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	if (isIndexed)
		dx11->getDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model2DDX11::renderBuffersOnly() const {
	if (isIndexed)
		dx11->getDeviceContext()->DrawIndexed(numIndices, 0, 0);
	else
		dx11->getDeviceContext()->Draw(numVertices, 0);
}

void Model2DDX11::unbindForRender() const {
	// Does nothing in DX11
	// Added for interface completeness
}

void Model2DDX11::interleaveData(std::vector<float>& result, const float* vertices, int numVertices, const float* texCoords) {
	for (int i = 0; i < numVertices; ++i) {
		result.push_back(vertices[i * 2 + 0]);
		result.push_back(vertices[i * 2 + 1]);
		result.push_back(texCoords[i * 2 + 0]);
		result.push_back(texCoords[i * 2 + 1]);
	}
}

void Model2DDX11::createInterleavedBuffer(const float* vertexData, int numVertices, int strideSize) {
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = sizeof(float) * strideSize * numVertices;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT result = dx11->getDevice()->CreateBuffer(&vertexDesc, &data, &vertexBuffer);
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to create 2D interleaved vertex buffer");
		assert(false);
		return;
	}
}

void Model2DDX11::createIndexBuffer(const int* indices, int numIndices) {
	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(indexDesc));
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(int) * numIndices;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT result = dx11->getDevice()->CreateBuffer(&indexDesc, &indexData, &indexBuffer);
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to create 2D index buffer");
		assert(false);
		return;
	}
}

#endif
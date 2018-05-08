
#include "Model3DDX11.hpp"

#include "../main.hpp"

#ifdef _USE_DIRECTX11

Model3DDX11::Model3DDX11(const float* vertices, int numVertices) : isTextured(false), isIndexed(false), hasNormals(false), hasTangents(false) {
	initialize(vertices, numVertices);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const float* texCoords) : isTextured(true), isIndexed(false), hasNormals(false), hasTangents(false) {
	initialize(vertices, numVertices, texCoords);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const float* texCoords, const float* normals) : isTextured(true), isIndexed(false), hasNormals(true), hasTangents(false) {
	initialize(vertices, numVertices, texCoords, normals);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents) : isTextured(true), isIndexed(false), hasNormals(true), hasTangents(true) {
	initialize(vertices, numVertices, texCoords, normals, tangents);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices) : isTextured(false), isIndexed(true), hasNormals(false), hasTangents(false) {
	initialize(vertices, numVertices, indices, numIndices);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) : isTextured(true), isIndexed(true), hasNormals(false), hasTangents(false) {
	initialize(vertices, numVertices, indices, numIndices, texCoords);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals) : isTextured(true), isIndexed(true), hasNormals(true), hasTangents(false) {
	initialize(vertices, numVertices, indices, numIndices, texCoords, normals);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents) : isTextured(true), isIndexed(true), hasNormals(true), hasTangents(true) {
	initialize(vertices, numVertices, indices, numIndices, texCoords, normals, tangents);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices) : isTextured(false), isIndexed(false), hasNormals(false), hasTangents(false) {
	initialize(&vertices[0], vertices.size() / 3);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<float>& texCoords) : isTextured(true), isIndexed(false), hasNormals(false), hasTangents(false) {
	initialize(&vertices[0], vertices.size() / 3, &texCoords[0]);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals) : isTextured(true), isIndexed(false), hasNormals(true), hasTangents(false) {
	initialize(&vertices[0], vertices.size() / 3, &texCoords[0], &normals[0]);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents) : isTextured(true), isIndexed(false), hasNormals(true), hasTangents(true) {
	initialize(&vertices[0], vertices.size() / 3, &texCoords[0], &normals[0], &tangents[0]);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices) : isTextured(false), isIndexed(true), hasNormals(false), hasTangents(false) {
	initialize(&vertices[0], vertices.size() / 3, &indices[0], indices.size());
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords) : isTextured(true), isIndexed(true), hasNormals(false), hasTangents(false) {
	initialize(&vertices[0], vertices.size() / 3, &indices[0], indices.size(), &texCoords[0]);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals) : isTextured(true), isIndexed(true), hasNormals(true), hasTangents(false) {
	initialize(&vertices[0], vertices.size() / 3, &indices[0], indices.size(), &texCoords[0], &normals[0]);
}

Model3DDX11::Model3DDX11(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents) : isTextured(true), isIndexed(true), hasNormals(true), hasTangents(true) {
	initialize(&vertices[0], vertices.size() / 3, &indices[0], indices.size(), &texCoords[0], &normals[0], &tangents[0]);
}

Model3DDX11::~Model3DDX11() {

}

void Model3DDX11::update() {

}

void Model3DDX11::render() const {
	unsigned int strides[] = { 3, 2 };
	static unsigned int offsets[] = { 0, 3 };
	ID3D11Buffer* buffers[2] = { vertexBuffer, texCoordBuffer };
	dx11->getDeviceContext()->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	if(isIndexed)
		dx11->getDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (isIndexed)
		dx11->getDeviceContext()->DrawIndexed(numIndices, 0, 0);
	else
		dx11->getDeviceContext()->Draw(numVertices, 0);
}

void Model3DDX11::bindForRender() const {
	unsigned int strides[] = { 3, 2 };
	static unsigned int offsets[] = { 0, 3 };
	ID3D11Buffer* buffers[2] = { vertexBuffer, texCoordBuffer };
	dx11->getDeviceContext()->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	if (isIndexed)
		dx11->getDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model3DDX11::renderBuffersOnly() const {
	if (isIndexed)
		dx11->getDeviceContext()->DrawIndexed(numIndices, 0, 0);
	else
		dx11->getDeviceContext()->Draw(numVertices, 0);
}

void Model3DDX11::unbindForRender() const {
	// Added for interface integrity with OpenGL Model3D
}

void Model3DDX11::initialize(const float* vertices, int numVertices) {
	dx11 = getStaticRenderingContextDX11();
	this->numVertices = numVertices;
	strideSize = 3 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const float* texCoords) {
	dx11 = getStaticRenderingContextDX11();
	this->numVertices = numVertices;
	strideSize = 5 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const float* texCoords, const float* normals) {
	dx11 = getStaticRenderingContextDX11();
	this->numVertices = numVertices;
	strideSize = 8 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents) {
	dx11 = getStaticRenderingContextDX11();
	this->numVertices = numVertices;
	strideSize = 11 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
	createBuffer(tangents, numVertices * 3, &tangentBuffer);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const int* indices, int numIndices) {
	dx11 = getStaticRenderingContextDX11();
	this->numIndices = numIndices;
	strideSize = 3 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) {
	dx11 = getStaticRenderingContextDX11();
	this->numIndices = numIndices;
	strideSize = 5 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals) {
	dx11 = getStaticRenderingContextDX11();
	this->numIndices = numIndices;
	strideSize = 8 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
}

void Model3DDX11::initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents) {
	dx11 = getStaticRenderingContextDX11();
	this->numIndices = numIndices;
	strideSize = 11 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
	createBuffer(tangents, numVertices * 3, &tangentBuffer);
}

void Model3DDX11::cleanUp() {

}

void Model3DDX11::createBuffer(const float* data, int numData, ID3D11Buffer** buffer) {
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(float) * numData;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA resource;
	resource.pSysMem = data;
	resource.SysMemPitch = 0;
	resource.SysMemSlicePitch = 0;

	HRESULT result = dx11->getDevice()->CreateBuffer(&bufferDesc, &resource, buffer);
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to initialize DX11 3D model with vertex info");
		assert(false);
		return;
	}
}

void Model3DDX11::createIndexBuffer(const int* indices, int numIndices) {
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
		OutputDebugString("Error: Failed to initialize index buffer during DX11 3D model creation");
		assert(false);
		return;
	}
}

#endif
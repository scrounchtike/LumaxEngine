
#include "Model3DDX11.hpp"

#include "../../main.hpp"

#ifdef _USE_DIRECTX11

Model3DDX11::Model3DDX11(const float* vertices, int numVertices) 
	: numVertices(numVertices), numIndices(0), isTextured(false), isIndexed(false), hasNormals(false), hasTangents(false) 
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 3 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const float* texCoords) 
	: numVertices(numVertices), numIndices(0), isTextured(true), isIndexed(false), hasNormals(false), hasTangents(false) 
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 5 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const float* texCoords, const float* normals)
	: numVertices(numVertices), numIndices(0), isTextured(true), isIndexed(false), hasNormals(true), hasTangents(false)
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 8 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents)
	: numVertices(numVertices), numIndices(0), isTextured(true), isIndexed(false), hasNormals(true), hasTangents(true) 
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 11 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
	createBuffer(tangents, numVertices * 3, &tangentBuffer);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices)
	: numVertices(numVertices), numIndices(numIndices), isTextured(false), isIndexed(true), hasNormals(false), hasTangents(false)
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 3 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) 
	: numVertices(numVertices), numIndices(numIndices), isTextured(true), isIndexed(true), hasNormals(false), hasTangents(false)
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 5 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals) 
	: numVertices(numVertices), numIndices(numIndices), isTextured(true), isIndexed(true), hasNormals(true), hasTangents(false)
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 8 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
}

Model3DDX11::Model3DDX11(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents)
	: numVertices(numVertices), numIndices(numIndices), isTextured(true), isIndexed(true), hasNormals(true), hasTangents(true)
{
	dx11 = getStaticRenderingContextDX11();
	strideSize = 11 * sizeof(float);
	createBuffer(vertices, numVertices * 3, &vertexBuffer);
	createIndexBuffer(indices, numIndices);
	createBuffer(texCoords, numVertices * 2, &texCoordBuffer);
	createBuffer(normals, numVertices * 3, &normalBuffer);
	createBuffer(tangents, numVertices * 3, &tangentBuffer);
}

Model3DDX11::~Model3DDX11() {
	// TODO: Cleanup buffers and resources
}

void Model3DDX11::update() {

}

void Model3DDX11::render() const {
	unsigned int strides[] = { 0 };
	static unsigned int offsets[] = { 0 };
	ID3D11Buffer* buffers[1] = { vertexBuffer };
	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, buffers, strides, offsets);
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

#include "RendererDX11.hpp"

#ifdef _USE_DIRECTX11

#include <cassert>
#include "../../loaders/ShaderLoader.hpp"

RendererDX11::RendererDX11(Camera* camera) : camera(camera) {
	// Get Rendering Context
	dx11 = getStaticRenderingContextDX11();

	bool success = initializeGeometries();
	success &= initializeShaders();
	assert(success);
}

RendererDX11::~RendererDX11() {
	bool success = cleanUpGeometries();
	success &= cleanUpShaders();
	assert(success);
}

/*
* Rendering of individual primitives
*/

void RendererDX11::renderPoint2D(const Point2D& point) const {
	shaderPoint2D->setUniform2f("position", point.position);
	shaderPoint2D->setUniform3f("color", point.color);
	shaderPoint2D->bind();

	static unsigned int stride = 2 * sizeof(float);
	static unsigned int offset = 0;

	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDpoint2D, &stride, &offset);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dx11->getDeviceContext()->Draw(6, 0);
}

void RendererDX11::renderLine2D(const Line2D& line) const {
	shaderLine2D->setUniform2f("start", line.A);
	shaderLine2D->setUniform2f("end", line.B);
	shaderLine2D->setUniform3f("color", line.color);
	shaderLine2D->bind();

	static unsigned int stride = 2 * sizeof(float);
	static unsigned int offset = 0;

	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDpoint2D, &stride, &offset);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	dx11->getDeviceContext()->Draw(2, 0);
}

void RendererDX11::renderSprite2D(const Sprite2D& sprite) const {
	shaderSprite2D->setUniform2f("position", sprite.position);
	shaderSprite2D->setUniform2f("extents", sprite.extents);
	shaderSprite2D->setUniform3f("color", sprite.color);
	shaderSprite2D->bind();

	static unsigned int stride = 2 * sizeof(float);
	static unsigned int offset = 0;

	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDsprite2D, &stride, &offset);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dx11->getDeviceContext()->Draw(6, 0);
}

void RendererDX11::renderPoint3D(const Point3D& point) const {
	shaderPoint3D->setUniformMatrix4f("view", camera->getViewMatrix());
	shaderPoint3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderPoint3D->setUniform3f("position", point.position);
	shaderPoint3D->setUniform3f("color", point.color);
	shaderPoint3D->bind();

	static unsigned int stride = 3 * sizeof(float);
	static unsigned int offset = 0;

	dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDsprite3D, &stride, &offset);
	dx11->getDeviceContext()->IASetIndexBuffer(indexIDpoint3D, DXGI_FORMAT_R32_UINT, 0);
	dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dx11->getDeviceContext()->DrawIndexed(32, 0, 0);
}

void RendererDX11::renderLine3D(const Line3D& line) const {

}

void RendererDX11::renderSprite3D(const Sprite3D& sprite) const {

}

void RendererDX11::renderMesh2D(const Mesh2D& mesh2D) const {
	mesh2D.render();
}

void RendererDX11::renderMesh3D(const Mesh3D& mesh3D) const {
	mesh3D.render(camera);
}

void RendererDX11::renderLightedMesh3D(const Mesh3D& mesh3D, const LightingDescription& lights) const {

}

void RendererDX11::renderAABB(const Mesh3D& aabb) const {

}

void RendererDX11::renderSphere(const Mesh3D& sphere) const {

}

void RendererDX11::renderPlane(const Mesh3D& plane) const {

}

void RendererDX11::renderOBB(const Mesh3D& obb) const {

}

void RendererDX11::renderRay(const Mesh3D& ray) const {

}

void RendererDX11::renderLine(const Mesh3D& line) const {

}

/*
* Rendering of primitive vectors
*/

void RendererDX11::renderPoints2D(const std::vector<Point2D*>& points2D) const {
	for (Point2D* point : points2D) {
		shaderPoint2D->setUniform2f("position", point->position);
		shaderPoint2D->setUniform3f("color", point->color);
		shaderPoint2D->bind();

		static unsigned int stride = 2 * sizeof(float);
		static unsigned int offset = 0;

		dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDpoint2D, &stride, &offset);
		dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		dx11->getDeviceContext()->Draw(6, 0);
	}
}

void RendererDX11::renderLines2D(const std::vector<Line2D*>& lines2D) const {
	for (Line2D* line : lines2D) {
		shaderLine2D->setUniform2f("start", line->A);
		shaderLine2D->setUniform2f("end", line->B);
		shaderLine2D->setUniform3f("color", line->color);
		shaderLine2D->bind();

		static unsigned int stride = 2 * sizeof(float);
		static unsigned int offset = 0;

		dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDline2D, &stride, &offset);
		dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		dx11->getDeviceContext()->Draw(2, 0);
	}
}

void RendererDX11::renderSprites2D(const std::vector<Sprite2D*>& sprites2D) const {
	for (Sprite2D* sprite : sprites2D) {
		shaderSprite2D->setUniform2f("position", sprite->position);
		shaderSprite2D->setUniform2f("extents", sprite->extents);
		shaderSprite2D->setUniform3f("color", sprite->color);
		shaderSprite2D->bind();

		static unsigned int stride = 2 * sizeof(float);
		static unsigned int offset = 0;

		dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDsprite2D, &stride, &offset);
		dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		dx11->getDeviceContext()->Draw(6, 0);
	}
}

void RendererDX11::renderPoints3D(const std::vector<Point3D*>& points3D) const {
	shaderPoint3D->setUniformMatrix4f("view", camera->getViewMatrix());
	shaderPoint3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	for (Point3D* point : points3D) {
		shaderPoint3D->setUniform3f("position", point->position);
		shaderPoint3D->setUniform3f("color", point->color);
		shaderPoint3D->bind();

		static unsigned int stride = 3 * sizeof(float);
		static unsigned int offset = 0;

		dx11->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexIDpoint3D, &stride, &offset);
		dx11->getDeviceContext()->IASetIndexBuffer(indexIDpoint3D, DXGI_FORMAT_R32_UINT, 0);
		dx11->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		dx11->getDeviceContext()->DrawIndexed(36, 0, 0);
		//dx11->getDeviceContext()->Draw(24, 0);
	}
}

void RendererDX11::renderLines3D(const std::vector<Line3D*>& lines3D) const {

}

void RendererDX11::renderSprites3D(const std::vector<Sprite3D*>& sprites3D) const {

}

void RendererDX11::renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const {
	for (Mesh2D* mesh : meshes2D)
		mesh->render();
}

void RendererDX11::renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const {
	assert(camera);
	for (Mesh3D* mesh : meshes3D) {
		mesh->shader->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		mesh->shader->setUniformMatrix4f("view", camera->getViewMatrix());
		if (mesh->transform)
			mesh->shader->setUniformMatrix4f("transform", *mesh->transform->getTransformation());
		else
			mesh->shader->setUniformMatrix4f("transform", Mat4().initIdentity());
		mesh->material->setShaderUniforms(mesh->shader);
		mesh->shader->prepareUniforms();

		mesh->fullModel->render();
	}
}

void RendererDX11::renderLightedMeshes3D(const std::vector<Mesh3D*>& meshes3D, const LightingDescription& lights) const {

}

void RendererDX11::renderAABBs(const std::vector<Mesh3D*>& aabbs) const {

}

void RendererDX11::renderSpheres(const std::vector<Mesh3D*>& spheres) const {

}

void RendererDX11::renderPlanes(const std::vector<Mesh3D*>& planes) const {

}

void RendererDX11::renderOBBs(const std::vector<Mesh3D*>& obbs) const {

}

void RendererDX11::renderRays(const std::vector<Mesh3D*>& rays) const {

}

void RendererDX11::renderLines(const std::vector<Mesh3D*>& lines) const {

}

bool RendererDX11::initializeGeometries() {
	// Point2D initialization
	{
		const float sizePoint = 0.025;
		float ar = 600.0f / 800.0f;
		float offset = sizePoint / 2.0f;
		float verticesPoints2D[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };

		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.ByteWidth = sizeof(float) * 12;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = verticesPoints2D;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT result = dx11->getDevice()->CreateBuffer(&vertexDesc, &data, &vertexIDpoint2D);
		if (FAILED(result)) {
			OutputDebugString("Error: Failed to create 2D point vertex buffer in RendererDX11 initialization");
			assert(false);
			return false;
		}
	}

	// Line 2D initialization
	{
		float verticesLines2D[4] = { 0,0,1,1 };

		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.ByteWidth = sizeof(float) * 4;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = verticesLines2D;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT result = dx11->getDevice()->CreateBuffer(&vertexDesc, &data, &vertexIDline2D);
		if (FAILED(result)) {
			OutputDebugString("Error: Failed to create 2D line vertex buffer in RendererDX11 initialization");
			assert(false);
			return false;
		}
	}

	// Sprite 2D initialization
	{
		const float sizeSprite = 0.5f;
		float ar = getStaticWindowHeight() / getStaticWindowWidth();
		float offset = sizeSprite / 2.0;
		float verticesSprites2D[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };
		float texCoordsSprites2D[12] = { 0,0,0,1,1,1,1,1,1,0,0,0 };

		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.ByteWidth = sizeof(float) * 12;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_BUFFER_DESC texCoordDesc;
		ZeroMemory(&texCoordDesc, sizeof(texCoordDesc));
		texCoordDesc.Usage = D3D11_USAGE_DEFAULT;
		texCoordDesc.ByteWidth = sizeof(float) * 12;
		texCoordDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = verticesSprites2D;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		D3D11_SUBRESOURCE_DATA texCoordData;
		texCoordData.pSysMem = texCoordsSprites2D;
		texCoordData.SysMemPitch = 0;
		texCoordData.SysMemSlicePitch = 0;

		HRESULT result = dx11->getDevice()->CreateBuffer(&vertexDesc, &vertexData, &vertexIDsprite2D);
		if (FAILED(result)) {
			OutputDebugString("Error: Failed to create 2D sprite vertex buffer in RendererDX11 initialization");
			assert(false);
			return false;
		}
		result = dx11->getDevice()->CreateBuffer(&texCoordDesc, &texCoordData, &texCoordIDsprite2D);
		if (FAILED(result)) {
			OutputDebugString("Error: Failed to create 2D sprite texCoord buffer in RendererDX11 initialization");
			assert(false);
			return false;
		}
	}

	// Point3D initialization
	{
		const float pointSize = 0.25f;
		float verticesPoints3D[24] = { -pointSize, pointSize, pointSize,
										pointSize, pointSize, pointSize,
										pointSize, -pointSize, pointSize,
										-pointSize, -pointSize, pointSize,
										-pointSize, pointSize, -pointSize,
										pointSize, pointSize, -pointSize,
										pointSize, -pointSize, -pointSize,
										-pointSize, -pointSize, -pointSize };
		int indicesPoints3D[36] = { 0,1,2,2,3,0,
									1,5,6,6,2,1,
									4,5,1,1,0,4,
									4,0,3,3,7,4,
									6,7,3,3,2,6,
									5,4,7,7,6,5 };
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.ByteWidth = sizeof(float) * 24;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_DEFAULT;
		indexDesc.ByteWidth = sizeof(int) * 36;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = verticesPoints3D;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indicesPoints3D;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		HRESULT result = dx11->getDevice()->CreateBuffer(&vertexDesc, &vertexData, &vertexIDpoint3D);
		if (FAILED(result)) {
			OutputDebugString("Error: Failed to create 3D point vertex buffer in RendererDX11 initialization");
			assert(false);
			return false;
		}
		result = dx11->getDevice()->CreateBuffer(&indexDesc, &indexData, &indexIDpoint3D);
		if (FAILED(result)) {
			OutputDebugString("Error: Failed to create 3D point index buffer in RendererDX11 initialization");
			assert(false);
			return false;
		}
	}

	return true;
}

bool RendererDX11::initializeShaders() {
	shaderPoint2D = ShaderLoader::loadShaderDX11("shaders/HLSL/renderer/shader2Dpoint");
	shaderLine2D = ShaderLoader::loadShaderDX11("shaders/HLSL/renderer/shader2Dline");
	shaderSprite2D = ShaderLoader::loadShaderDX11("shaders/HLSL/renderer/shader2Dsprite");

	shaderPoint3D = ShaderLoader::loadShaderDX11("shaders/HLSL/renderer/shader3Dpoint");
	//shaderLine3D = ShaderLoader::loadShaderDX11("shaders/HLSL/renderer/shader3Dline");
	//shaderSprite3D = ShaderLoader::loadShaderDX11("shaders/HLSL/renderer/shader3Dsprite");

	return true;
}

bool RendererDX11::cleanUpGeometries() {
	return true;
}

bool RendererDX11::cleanUpShaders() {
	delete shaderPoint2D;
	delete shaderLine2D;
	delete shaderSprite2D;
	
	return true;
}

#endif

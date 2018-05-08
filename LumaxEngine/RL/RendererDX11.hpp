
#ifndef RENDERER_DX11_HPP
#define RENDERER_DX11_HPP

#include "Renderer.hpp"

#ifdef _USE_DIRECTX11

#include "ShaderDX11.hpp"
#include "Camera.hpp"
#include "../RAL/UsingDX11.hpp"
#include "../main.hpp"

class RendererDX11 {
public:
	RendererDX11(Camera* camera);
	~RendererDX11();

	void renderPoint2D(const Point2D& point) const;
	void renderLine2D(const Line2D& line) const;
	void renderSprite2D(const Sprite2D& sprite) const;
	void renderPoint3D(const Point3D& point) const;
	void renderLine3D(const Line3D& line) const;
	void renderSprite3D(const Sprite3D& sprite) const;
	
	void renderMesh2D(const Mesh2D& mesh2D) const;
	void renderMesh3D(const Mesh3D& mesh3D) const;

	void renderPoints2D(const std::vector<Point2D*>& points2D) const;
	void renderLines2D(const std::vector<Line2D*>& lines2D) const;
	void renderSprites2D(const std::vector<Sprite2D*>& sprites2D) const;
	void renderPoints3D(const std::vector<Point3D*>& points3D) const;
	void renderLines3D(const std::vector<Line3D*>& lines3D) const;
	void renderSprites3D(const std::vector<Sprite3D*>& sprites3D) const;

	void renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const;
	void renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const;

	void setCamera(Camera* camera) { this->camera = camera; }
	Camera* getCamera() { return camera; }
private:
	bool initializeGeometries();
	bool initializeShaders();

	bool cleanUpGeometries();
	bool cleanUpShaders();

	// DX11 Rendering Context
	RenderingContextDX11* dx11;

	// Geometries
	ID3D11Buffer* vertexIDpoint2D;
	ID3D11Buffer* vertexIDline2D;
	ID3D11Buffer* vertexIDsprite2D;
	ID3D11Buffer* texCoordIDsprite2D;

	ID3D11Buffer* vertexIDpoint3D;
	ID3D11Buffer* indexIDpoint3D;
	ID3D11Buffer* vertexIDline3D;
	ID3D11Buffer* vertexIDsprite3D;
	ID3D11Buffer* texCoordIDsprite3D;

	// Shaders
	ShaderDX11* shaderPoint2D;
	ShaderDX11* shaderLine2D;
	ShaderDX11* shaderSprite2D;

	ShaderDX11* shaderPoint3D;
	ShaderDX11* shaderLine3D;
	ShaderDX11* shaderSprite3D;

	// Camera
	Camera* camera;
};

#endif
#endif
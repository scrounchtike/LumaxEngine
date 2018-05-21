
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "RenderPrimitives.hpp"
#include "../RAL/buildDesc.hpp"

#include "Mesh2D.hpp"
#include "Mesh3D.hpp"

#ifdef _USE_OPENGL
class RendererGL;
#elif defined _USE_DIRECTX11
class RendererDX11;
#endif

#include "Camera.hpp"
#include "Shader.hpp"

#include <vector>

struct LightingDescription;

class Renderer {
public:
	Renderer(Camera* camera);
	~Renderer();

	// Individual rendering methods
	void renderPoint2D(const Point2D& point) const;
	void renderLine2D(const Line2D& line) const;
	void renderSprite2D(const Sprite2D& model) const;

	void renderPoint3D(const Point3D& point) const;
	void renderLine3D(const Line3D& line) const;
	void renderSprite3D(const Sprite3D& sprite) const;

	void renderMesh2D(const Mesh2D& mesh2D) const;
	void renderMesh3D(const Mesh3D& mesh3D) const;
	void renderLightedMesh3D(const Mesh3D& mesh3D) const;

	void renderAABB(const Mesh3D& aabb) const;
	void renderSphere(const Mesh3D& sphere) const;
	void renderPlane(const Mesh3D& plane) const;
	void renderOBB(const Mesh3D& obb) const;
	void renderRay(const Mesh3D& ray) const;
	void renderLine(const Mesh3D& line) const;

	// Vector rendering methods (more efficient state reuse)
	void renderPoints2D(const std::vector<Point2D*>& points) const;
	void renderLines2D(const std::vector<Line2D*>& lines) const;
	void renderSprites2D(const std::vector<Sprite2D*>& sprites) const;

	void renderPoints3D(const std::vector<Point3D*>& points) const;
	void renderLines3D(const std::vector<Line3D*>& lines) const;
	void renderSprites3D(const std::vector<Sprite3D*>& sprites) const;

	void renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const;
	void renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const;
	void renderLightedMeshes3D(const std::vector<Mesh3D*>& meshes3D, const LightingDescription& lights) const;

	void renderAABBs(const std::vector<Mesh3D*>& aabbs) const;
	void renderSpheres(const std::vector<Mesh3D*>& spheres) const;
	void renderPlanes(const std::vector<Mesh3D*>& planes) const;
	void renderOBBs(const std::vector<Mesh3D*>& obbs) const;
	void renderRays(const std::vector<Mesh3D*>& rays) const;
	void renderLines(const std::vector<Mesh3D*>& lines) const;
private:
	// Graphics API dependent Renderer implementation
#ifdef _USE_OPENGL
	RendererGL* renderer;
#elif defined _USE_DIRECTX11
	RendererDX11* renderer;
#endif
};

#endif
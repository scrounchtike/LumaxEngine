
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Model2D.hpp"
#include "RenderPrimitives.hpp"
#include "../RAL/buildDesc.hpp"

#include "Mesh2D.hpp"
#include "Mesh3D.hpp"

#include <vector>

class Renderer {
public:
	virtual void renderPoint2D(const Point2D& point) const = 0;
	virtual void renderLine2D(const Line2D& line) const = 0;
	virtual void renderSprite2D(const Sprite2D& model) const = 0;
	virtual void renderMesh2D(const Mesh2D& mesh2D) const = 0;
	//virtual void renderMesh3D(const Mesh3D& mesh3D) const = 0;

	virtual void renderPoints2D(const std::vector<Point2D*>& points) const = 0;
	virtual void renderLines2D(const std::vector<Line2D*>& lines) const = 0;
	virtual void renderSprites2D(const std::vector<Sprite2D*>& sprites) const = 0;
	virtual void renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const = 0;
	//virtual void renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const = 0;
private:
};

#endif
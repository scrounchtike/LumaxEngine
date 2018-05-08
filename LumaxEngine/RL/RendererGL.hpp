
#ifndef RENDERER_GL_HPP
#define RENDERER_GL_HPP

#include "Renderer.hpp"

#ifdef _USE_OPENGL

#include "ShaderGL.hpp"
#include "../RAL/UsingOpenGL.hpp"
#include "Camera.hpp"

#include "../PL/PhysicsPrimitives.hpp"

class RendererGL {
public:
	RendererGL(Camera* camera);
	~RendererGL();

	void renderPoint2D(const Point2D& point) const;
	void renderLine2D(const Line2D& line) const;
	void renderSprite2D(const Sprite2D& sprite) const;
	void renderPoint3D(const Point3D& point) const;
	void renderLine3D(const Line3D& line) const;
	void renderSprite3D(const Sprite3D& sprite) const;

	void renderMesh2D(const Mesh2D& mesh2D) const;
	void renderMesh3D(const Mesh3D& mesh3D) const;

	void renderAABB(const Mesh3D& aabb) const;
	void renderSphere(const Mesh3D& sphere) const;
	void renderPlane(const Mesh3D& plane) const;
	void renderOBB(const Mesh3D& obb) const;
	void renderRay(const Mesh3D& ray) const;
	void renderLine(const Mesh3D& line) const;

	void renderPoints2D(const std::vector<Point2D*>& points) const;
	void renderLines2D(const std::vector<Line2D*>& lines) const;
	void renderSprites2D(const std::vector<Sprite2D*>& sprites) const;
	void renderPoints3D(const std::vector<Point3D*>& points) const;
	void renderLines3D(const std::vector<Line3D*>& lines) const;
	void renderSprites3D(const std::vector<Sprite3D*>& sprites) const;

	void renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const;
	void renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const;

	void renderAABBs(const std::vector<Mesh3D*>& aabbs) const;
	void renderSpheres(const std::vector<Mesh3D*>& spheres) const;
	void renderPlanes(const std::vector<Mesh3D*>& planes) const;
	void renderOBBs(const std::vector<Mesh3D*>& obbs) const;
	void renderRays(const std::vector<Mesh3D*>& rays) const;
	void renderLines(const std::vector<Mesh3D*>& lines) const;

	void setCamera(Camera* camera) { this->camera = camera; }
	Camera* getCamera() const { return camera; }
private:
	bool initializeGeometries();
	bool initializeShaders();

	bool cleanUpGeometries();
	bool cleanUpShaders();

	// Geometries
	GLuint vaoIDpoint2D;
	GLuint vboIDpoint2D;

	GLuint vaoIDpoint3D;
	GLuint vboIDpoint3D;
	GLuint iboIDpoint3D;

	GLuint vaoIDcube3D;
	GLuint vboIDcube3D;
	GLuint iboIDcube3D;

	GLuint vaoIDline2D;
	GLuint vboIDline2D;

	GLuint vaoIDline3D;
	GLuint vboIDline3D;

	GLuint vaoIDsprite2D;
	GLuint vboIDsprite2D;
	GLuint vbotIDsprite2D;

	GLuint vaoIDsprite3D;
	GLuint vboIDsprite3D;
	GLuint vbotIDsprite3D;

	GLuint vaoIDsphere3D;
	GLuint vboIDsphere3D;
	GLuint iboIDsphere3D;
	GLuint vbotIDsphere3D;
	unsigned int numIndicesSphere;

	GLuint vaoIDplane3D;
	GLuint vboIDplane3D;
	GLuint vbotIDplane3D;

	// Shaders
	ShaderGL* shaderPoint2D;
	ShaderGL* shaderLine2D;
	ShaderGL* shaderSprite2D;
	ShaderGL* shaderPoint3D;
	ShaderGL* shaderLine3D;
	ShaderGL* shaderSprite3D;

	ShaderGL* shaderAABB3D;
	ShaderGL* shaderSphere3D;
	ShaderGL* shaderPlane3D;
	ShaderGL* shaderOBB3D;

	Camera* camera;
};

#endif
#endif
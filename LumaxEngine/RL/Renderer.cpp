
#include "Renderer.hpp"

#include "../GL/Level.hpp"

#ifdef _USE_OPENGL
#include "renderers/RendererGL.hpp"
#elif defined _USE_DIRECTX11
#include "renderers/RendererDX11.hpp"
#endif

Renderer::Renderer(Camera* camera) {
#ifdef _USE_OPENGL
	renderer = new RendererGL(camera);
#elif defined _USE_DIRECTX11
	renderer = new RendererDX11(camera);
#endif
}

Renderer::~Renderer() {
	delete renderer;
}

// Individual rendering methods
void Renderer::renderPoint2D(const Point2D& point) const {
	renderer->shaderPoint2D->bind();
	renderer->shaderPoint2D->setUniform2f("position", point.position);
	renderer->shaderPoint2D->setUniform3f("color", point.color);
	renderer->shaderPoint2D->prepareUniforms();

	renderer->renderPoint2D(point);
}

void Renderer::renderLine2D(const Line2D& line) const {
	renderer->shaderLine2D->bind();
	renderer->shaderLine2D->setUniform2f("A", line.A);
	renderer->shaderLine2D->setUniform2f("B", line.B);
	renderer->shaderLine2D->setUniform3f("color", line.color);
	renderer->shaderLine2D->prepareUniforms();

	renderer->renderLine2D(line);
}

void Renderer::renderSprite2D(const Sprite2D& sprite) const {
	renderer->shaderSprite2D->bind();
	renderer->shaderSprite2D->setUniform2f("position", sprite.position);
	renderer->shaderSprite2D->setUniform2f("extents", sprite.extents);
	renderer->shaderSprite2D->setUniform3f("color", sprite.color);
	renderer->shaderSprite2D->prepareUniforms();

	renderer->renderSprite2D(sprite);
}

void Renderer::renderPoint3D(const Point3D& point) const {
	renderer->shaderPoint3D->bind();
	renderer->shaderPoint3D->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderPoint3D->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	renderer->shaderPoint3D->setUniform3f("position", point.position);
	renderer->shaderPoint3D->setUniform3f("color", point.color);
	renderer->shaderPoint3D->prepareUniforms();
}

void Renderer::renderLine3D(const Line3D& line) const {
	renderer->shaderLine3D->bind();
	renderer->shaderLine3D->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderLine3D->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	renderer->shaderLine3D->setUniform3f("start", line.start);
	renderer->shaderLine3D->setUniform3f("end", line.end);
	renderer->shaderLine3D->setUniform3f("color", line.color);
	renderer->shaderLine3D->prepareUniforms();
}

void Renderer::renderSprite3D(const Sprite3D& sprite) const {
	assert(false);
}

void Renderer::renderMesh2D(const Mesh2D& mesh2D) const {
	renderer->renderMesh2D(mesh2D);
}

void Renderer::renderMesh3D(const Mesh3D& mesh3D) const {
	renderer->renderMesh3D(mesh3D);
}

void Renderer::renderLightedMesh3D(const Mesh3D& mesh3D) const {
	//renderer->renderLightedMesh3D(mesh3D);
}

void Renderer::renderAABB(const Mesh3D& aabb) const {
	renderer->shaderAABB->bind();
	renderer->shaderAABB->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderAABB->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	renderer->shaderAABB->setUniform3f("position", ((AABB*)(aabb.physics))->position);
	renderer->shaderAABB->setUniform3f("extents", ((AABB*)(aabb.physics))->extents);
	aabb.material->setShaderUniforms(renderer->shaderAABB);
	renderer->shaderAABB->prepareUniforms();

	renderer->renderAABB(aabb);
}

void Renderer::renderSphere(const Mesh3D& sphere) const {
	renderer->shaderSphere->bind();
	renderer->shaderSphere->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderSphere->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	renderer->shaderSphere->setUniform3f("position", ((Sphere*)(sphere.physics))->position);
	renderer->shaderSphere->setUniform1f("radius", ((Sphere*)(sphere.physics))->radius);
	sphere.material->setShaderUniforms(renderer->shaderSphere);
	renderer->shaderSphere->prepareUniforms();

	renderer->renderSphere(sphere);
}

void Renderer::renderPlane(const Mesh3D& plane) const {
	renderer->shaderPlane->bind();
	renderer->shaderPlane->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderPlane->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	if (plane.material->getTexture())
		renderer->shaderPlane->setUniform1f("isTextured", 1.0);
	else
		renderer->shaderPlane->setUniform1f("isTextured", 0.0);
	Vec3 bitangent = -cross(((Plane*)plane.physics)->tangent, ((Plane*)plane.physics)->normal);
	Mat4 rotation = Mat4().initSpecialRotation2(((Plane*)plane.physics)->tangent, bitangent, ((Plane*)plane.physics)->normal);
	renderer->shaderPlane->setUniformMatrix4f("rotation", rotation);
	renderer->shaderPlane->setUniform3f("position", ((Plane*)plane.physics)->position);
	renderer->shaderPlane->setUniform1f("scale", ((Plane*)plane.physics)->renderingScale);
	plane.material->setShaderUniforms(renderer->shaderPlane);
	renderer->shaderPlane->prepareUniforms();

	renderer->renderPlane(plane);
}

void Renderer::renderOBB(const Mesh3D& obb) const {
	renderer->shaderOBB->bind();
	renderer->shaderOBB->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderOBB->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	OBB* phys = (OBB*)obb.physics;
	Vec3 u2 = cross(phys->u[0], phys->u[1]);
	Mat4 translation = Mat4().initTranslation(phys->position);
	Mat4 rotation = Mat4().initSpecialRotation2(phys->u[0], phys->u[1], u2);
	Mat4 scale = Mat4().initScale(phys->e);
	Mat4 transform = translation.mul(rotation.mul(scale));
	renderer->shaderOBB->setUniformMatrix4f("transform", transform);
	obb.material->setShaderUniforms(renderer->shaderOBB);
	renderer->shaderOBB->prepareUniforms();

	renderer->renderOBB(obb);
}

void Renderer::renderRay(const Mesh3D& ray) const {
	renderer->shaderLine3D->bind();
	renderer->shaderLine3D->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderLine3D->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	renderer->shaderLine3D->setUniform3f("start", ((Ray*)ray.physics)->position);
	renderer->shaderLine3D->setUniform3f("start", ((Ray*)ray.physics)->position + ((Ray*)ray.physics)->direction * 10000.0f);
	ray.material->setShaderUniforms(renderer->shaderLine3D);
	renderer->shaderLine3D->prepareUniforms();

	renderer->renderRay(ray);
}

void Renderer::renderLine(const Mesh3D& line) const {
	renderer->shaderLine3D->bind();
	renderer->shaderLine3D->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	renderer->shaderLine3D->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	renderer->shaderLine3D->setUniform3f("start", ((Line*)line.physics)->start);
	renderer->shaderLine3D->setUniform3f("start", ((Line*)line.physics)->end);
	line.material->setShaderUniforms(renderer->shaderLine3D);
	renderer->shaderLine3D->prepareUniforms();

	renderer->renderLine(line);
}

// Vector rendering methods (more efficient state reuse)
void Renderer::renderPoints2D(const std::vector<Point2D*>& points) const {
	renderer->renderPoints2D(points);
}

void Renderer::renderLines2D(const std::vector<Line2D*>& lines) const {
	renderer->renderLines2D(lines);
}

void Renderer::renderSprites2D(const std::vector<Sprite2D*>& sprites) const {
	renderer->renderSprites2D(sprites);
}

void Renderer::renderPoints3D(const std::vector<Point3D*>& points) const {
	renderer->renderPoints3D(points);
}

void Renderer::renderLines3D(const std::vector<Line3D*>& lines) const {
	renderer->renderLines3D(lines);
}

void Renderer::renderSprites3D(const std::vector<Sprite3D*>& sprites) const {
	renderer->renderSprites3D(sprites);
}

void Renderer::renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const {
	renderer->renderMeshes2D(meshes2D);
}

void Renderer::renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const {
	renderer->renderMeshes3D(meshes3D);
}

void Renderer::renderLightedMeshes3D(const std::vector<Mesh3D*>& meshes3D, const LightingDescription& lights) const{
	renderer->renderLightedMeshes3D(meshes3D, lights);
}

void Renderer::renderAABBs(const std::vector<Mesh3D*>& aabbs) const {
	renderer->renderAABBs(aabbs);
}

void Renderer::renderSpheres(const std::vector<Mesh3D*>& spheres) const {
	renderer->renderSpheres(spheres);
}

void Renderer::renderPlanes(const std::vector<Mesh3D*>& planes) const {
	renderer->renderPlanes(planes);
}

void Renderer::renderOBBs(const std::vector<Mesh3D*>& obbs) const {
	renderer->renderOBBs(obbs);
}

void Renderer::renderRays(const std::vector<Mesh3D*>& rays) const {
	renderer->renderRays(rays);
}

void Renderer::renderLines(const std::vector<Mesh3D*>& lines) const {
	renderer->renderLines(lines);
}
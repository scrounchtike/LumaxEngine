
#include "Renderer.hpp"

#include "../GL/Level.hpp"

#ifdef _USE_OPENGL
#include "renderers/RendererGL.hpp"
#elif defined _USE_DIRECTX11
#include "renderers/RendererDX11.hpp"
#endif

Renderer::Renderer(Player* player) {
#ifdef _USE_OPENGL
	renderer = new RendererGL(player->getCamera());
#elif defined _USE_DIRECTX11
	renderer = new RendererDX11(player->getCamera());
#endif
}

Renderer::~Renderer() {
	delete renderer;
}

// Individual rendering methods
void Renderer::renderPoint2D(const Point2D& point) const {
	shader2Dpoint->bind();
	shader2Dpoint->setUniform2f("position", point.position);
	shader2Dpoint->setUniform3f("color", point.color);
	shader2Dpoint->prepareUniforms();

	point2D->render();
}

void Renderer::renderLine2D(const Line2D& line) const {
	shader2Dline->bind();
	shader2Dline->setUniform2f("A", line.A);
	shader2Dline->setUniform2f("B", line.B);
	shader2Dline->setUniform3f("color", line.color);
	shader2Dline->prepareUniforms();

	line2D->render();
}

void Renderer::renderSprite2D(const Sprite2D& sprite) const {
	shader2Dsprite->bind();
	shader2Dsprite->setUniform2f("position", sprite.position);
	shader2Dsprite->setUniform2f("extents", sprite.extents);
	shader2Dsprite->setUniform3f("color", sprite.color);
	shader2Dsprite->prepareUniforms();

	sprite2D->render();
}

void Renderer::renderPoint3D(const Point3D& point) const {
	shader3Dpoint->bind();
	shader3Dpoint->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shader3Dpoint->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	shader3Dpoint->setUniform3f("position", point.position);
	shader3Dpoint->setUniform3f("color", point.color);
	shader3Dpoint->prepareUniforms();

	//renderer->renderPoint3D();
	cube->render();
}

void Renderer::renderLine3D(const Line3D& line) const {
	shader3Dline->bind();
	shader3Dline->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shader3Dline->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	shader3Dline->setUniform3f("start", line.start);
	shader3Dline->setUniform3f("end", line.end);
	shader3Dline->setUniform3f("color", line.color);
	shader3Dline->prepareUniforms();

	renderer->renderLine3D();
}

void Renderer::renderSprite3D(const Sprite3D& sprite) const {
	assert(false);
}

void Renderer::renderAABB(const Mesh3D& aabb) const {
	shaderAABB->bind();
	shaderAABB->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shaderAABB->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	shaderAABB->setUniform3f("position", ((AABB*)(aabb.physics))->position);
	shaderAABB->setUniform3f("extents", ((AABB*)(aabb.physics))->extents);
	aabb.material->setShaderUniforms(shaderAABB);
	shaderAABB->prepareUniforms();

	cube->render();
}

void Renderer::renderSphere(const Mesh3D& sphere) const {
	shaderSphere->bind();
	shaderSphere->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shaderSphere->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	shaderSphere->setUniform3f("position", ((Sphere*)(sphere.physics))->position);
	shaderSphere->setUniform1f("radius", ((Sphere*)(sphere.physics))->radius);
	sphere.material->setShaderUniforms(shaderSphere);
	shaderSphere->prepareUniforms();

	sphere->render();
}

void Renderer::renderPlane(const Mesh3D& plane) const {
	shaderPlane->bind();
	shaderPlane->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shaderPlane->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	if (plane.material->getTexture())
		shaderPlane->setUniform1f("isTextured", 1.0);
	else
		shaderPlane->setUniform1f("isTextured", 0.0);
	Vec3 bitangent = -cross(((Plane*)plane.physics)->tangent, ((Plane*)plane.physics)->normal);
	Mat4 rotation = Mat4().initSpecialRotation2(((Plane*)plane.physics)->tangent, bitangent, ((Plane*)plane.physics)->normal);
	shaderPlane->setUniformMatrix4f("rotation", rotation);
	shaderPlane->setUniform3f("position", ((Plane*)plane.physics)->position);
	shaderPlane->setUniform1f("scale", ((Plane*)plane.physics)->renderingScale);
	plane.material->setShaderUniforms(shaderPlane);
	shaderPlane->prepareUniforms();

	plane->render();
}

void Renderer::renderOBB(const Mesh3D& obb) const {
	shaderOBB->bind();
	shaderOBB->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shaderOBB->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	OBB* phys = (OBB*)obb.physics;
	Vec3 u2 = cross(phys->u[0], phys->u[1]);
	Mat4 translation = Mat4().initTranslation(phys->position);
	Mat4 rotation = Mat4().initSpecialRotation2(phys->u[0], phys->u[1], u2);
	Mat4 scale = Mat4().initScale(phys->e);
	Mat4 transform = translation.mul(rotation.mul(scale));
	shaderOBB->setUniformMatrix4f("transform", transform);
	obb.material->setShaderUniforms(shaderOBB);
	shaderOBB->prepareUniforms();

	cube->render();
}

void Renderer::renderRay(const Mesh3D& ray) const {
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	shaderLine3D->setUniform3f("start", ((Ray*)ray.physics)->position);
	shaderLine3D->setUniform3f("end", ((Ray*)ray.physics)->position + ((Ray*)ray.physics)->direction * 10000.0f);
	ray.material->setShaderUniforms(shaderLine3D);
	shaderLine3D->prepareUniforms();

	renderer->renderLine3D();
}

void Renderer::renderLine(const Mesh3D& line) const {
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix4f("projection", renderer->camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix4f("view", renderer->camera->getViewMatrix());
	shaderLine3D->setUniform3f("start", ((Line*)line.physics)->start);
	shaderLine3D->setUniform3f("end", ((Line*)line.physics)->end);
	line.material->setShaderUniforms(shaderLine3D);
	shaderLine3D->prepareUniforms();

	renderer->renderLine();
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

void Renderer::renderLightedMeshes3D(const std::vector<Mesh3D*>& meshes3D, const LightingDescription& lights) const {
	renderer->renderLightedMeshes3D(meshes3D, lights);
}

void Renderer::renderDeferredLightedMeshes3D(const std::vector<Mesh3D *> &meshes3D, const LightingDescription &lights) const {
	renderer->renderDeferredLightedMeshes3D(meshes3D, lights);
}

void Renderer::renderAnimatedMeshes3D(const std::vector<Mesh3D*>& meshes3D){
	renderer->renderAnimatedMeshes3D(meshes3D);
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

void Renderer::initializeGeometries(){
	// Point2D initialization
	{
		const float sizePoint = 0.025;
		float ar = getStaticWindowHeight() / getStaticWindowWidth();
		float offset = sizePoint / 2.0;
		float vertices[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };
		point2D = new Model2D(vertices, 12);
	}

	// Line2D initialization
	{
		float vertices = { 0,0,1,1 };
		line2D = new Model2D(vertices, 4);
	}
}

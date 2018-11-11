
#include "Mesh3D.hpp"

Mesh3D::Mesh3D(Model3D* model, Material* material, Shader* shader, Transform3D* transform, PhysicsPrimitive* physics) 
	: material(material), transform(transform), physics(physics)
{
	assert(shader);
	shaders.push_back(shader);
	this->fullModel->models.push_back(model);
}

Mesh3D::Mesh3D(FullModel3D* fullModel, Material* material, Shader* shader, Transform3D* transform, PhysicsPrimitive* physics)
	: fullModel(fullModel), material(material), transform(transform), physics(physics)
{
	assert(shader);
	shaders.push_back(shader);
}

Mesh3D::~Mesh3D() {
	
}

void Mesh3D::update() {
	fullModel->update();
}

bool Mesh3D::collidesWith(Mesh3D* mesh) const {
	return physics->collidesWith(mesh->physics);
}

void Mesh3D::render() const {
	fullModel->render();
}

void Mesh3D::bindForRender() const {
	fullModel->bindForRender();
}

void Mesh3D::renderBuffersOnly() const {
	// Set transform uniforms
	static Mat4 identity = Mat4().initIdentity();
	if (transform) {
		shaders[0]->setUniformMatrix4f("transform", *transform->getTransformation());
	}
	else
		shaders[0]->setUniformMatrix4f("transform", identity);

	fullModel->renderBuffersOnly();
}

void Mesh3D::unbindForRender() const {
	fullModel->unbindForRender();
}

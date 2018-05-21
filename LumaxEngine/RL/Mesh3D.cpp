
#include "Mesh3D.hpp"

Mesh3D::Mesh3D(Model3D* model, Material* material, Shader* shader, Transform3D* transform, PhysicsPrimitive* physics) 
	: material(material), shader(shader), transform(transform), physics(physics)
{
	this->fullModel->models.push_back(model);
}

Mesh3D::Mesh3D(FullModel3D* fullModel, Material* material, Shader* shader, Transform3D* transform, PhysicsPrimitive* physics)
	: fullModel(fullModel), material(material), shader(shader), transform(transform), physics(physics)
{

}

Mesh3D::~Mesh3D() {
	
}

void Mesh3D::update() {
	fullModel->update();
}

bool Mesh3D::collidesWith(Mesh3D* mesh) const {
	return physics->collidesWith(mesh->physics);
}

void Mesh3D::render(const Camera* camera) const {

	// this code is broken
	assert(false);

#ifdef _USE_OPENGL
	shader->bind();
#endif
	// Set material to shader uniforms
	//shader->setUniform3f("color", material->color);
#ifdef _USE_OPENGL
	material->setShaderUniforms(shader);
	//material->getTexture()->bind();
#elif defined _USE_DIRECTX11
	//material->setShaderUniforms((ShaderDX11*)shader);
	shader->setUniform3f("color", Vec3(1,1,1));
#endif
	// lighting uniforms
	//shader->setUniform3f("dLight.direction", Vec3(1, 1, 1).normalize());
	//shader->setUniform3f("dLight.color", Vec3(1, 1, 0));
	//shader->setUniform1f("dLight.intensity", 1.0);
	// Set uniforms
	static Mat4 identity = Mat4().initIdentity();
	shader->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader->setUniformMatrix4f("view", camera->getViewMatrix());
	if (transform) {
		shader->setUniformMatrix4f("transform", *transform->getTransformation());
	}else
		shader->setUniformMatrix4f("transform", identity);
#ifdef _USE_DIRECTX11
		shader->bind();
#endif

	fullModel->render();
}

void Mesh3D::bindForRender(const Camera* camera) const {
	shader->bind();
	shader->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader->setUniformMatrix4f("view", camera->getViewMatrix());
	// Set material to shader uniforms
	shader->setUniform3f("color", material->getColor());

	fullModel->bindForRender();
}

void Mesh3D::renderBuffersOnly() const {
	// Set transform uniforms
	static Mat4 identity = Mat4().initIdentity();
	if (transform) {
		shader->setUniformMatrix4f("transform", *transform->getTransformation());
	}
	else
		shader->setUniformMatrix4f("transform", identity);

	fullModel->renderBuffersOnly();
}

void Mesh3D::unbindForRender() const {
	fullModel->unbindForRender();
}

#include "Mesh3D.hpp"

#ifdef _USE_OPENGL
Mesh3D::Mesh3D(Model3DGL* model, Material* material, Shader* shader, Transform3D* transform, PhysicsPrimitive* physics) 
	: material(material), shader(shader), transform(transform), physics(physics)
{
	this->fullModel->models.push_back(model);
}
#elif defined _USE_DIRECTX11
Mesh3D::Mesh3D(Model3DDX11* model, Material* material, Shader* shader, Transform3D* transform, PhysicsPrimitive* physics)
	: material(material), shader(shader), transform(transform), physics(physics)
{
	this->fullModel.models.push_back(model);
}
#endif

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
#ifdef _USE_OPENGL
	shader->bind();
#endif
	// Set material to shader uniforms
	//shader->setUniform3f("color", material->color);
#ifdef _USE_OPENGL
	material->setShaderUniforms((ShaderGL*)shader);
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
	shader->setUniformMatrix("projection", camera->getProjectionMatrix());
	shader->setUniformMatrix("view", camera->getViewMatrix());
	if (transform) {
		shader->setUniformMatrix("transform", *transform->getTransformation());
	}else
		shader->setUniformMatrix("transform", identity);
#ifdef _USE_DIRECTX11
		shader->bind();
#endif

	fullModel->render();
}

void Mesh3D::bindForRender(const Camera* camera) const {
	shader->bind();
	shader->setUniformMatrix("projection", camera->getProjectionMatrix());
	shader->setUniformMatrix("view", camera->getViewMatrix());
	// Set material to shader uniforms
	shader->setUniform3f("color", material->getColor());

	fullModel->bindForRender();
}

void Mesh3D::renderBuffersOnly() const {
	// Set transform uniforms
	static Mat4 identity = Mat4().initIdentity();
	if (transform) {
		shader->setUniformMatrix("transform", *transform->getTransformation());
	}
	else
		shader->setUniformMatrix("transform", identity);

	fullModel->renderBuffersOnly();
}

void Mesh3D::unbindForRender() const {
	fullModel->unbindForRender();
}
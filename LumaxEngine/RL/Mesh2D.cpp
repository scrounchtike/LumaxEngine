
#include "Mesh2D.hpp"

#include "../RAL/Log.hpp"

Mesh2D::Mesh2D(Model2D* model2D, Material* material, Shader* shader, Transform2D* transform)
	: model2D(model2D), material(material), shader(shader), transform(transform)
{

}

void Mesh2D::update() {

}

void Mesh2D::render() const {
	//shader->bind();
	static Mat3 identityGL = Mat3().initIdentity();
	static Mat4 identityDX11 = Mat4().initIdentity();
	if (transform) {
		// We have a transform matrix
		shader->setUniformMatrix("transform", *transform->getTransformation());
	}
	else {
#ifdef _USE_OPENGL
		shader->setUniformMatrix("transform", identityGL);
#elif defined _USE_DIRECTX11
		shader->setUniformMatrix("transform", identityDX11);
#endif
	}
	shader->bind();

	// Bind uniforms for material
	model2D->render();
}

void Mesh2D::bindForRender() const {
	shader->bind();
	model2D->bindForRender();
}

void Mesh2D::renderBuffersOnly() const {
	if (transform) {
		// Send the transform matrix
		shader->setUniformMatrix("transform", *transform->getTransformation());
	}
	else {
		shader->setUniformMatrix("transform", Mat3().initIdentity());
	}
	model2D->renderBuffersOnly();
}

void Mesh2D::unbindForRender() const {
	model2D->unbindForRender();
}
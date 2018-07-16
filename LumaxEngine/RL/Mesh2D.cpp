
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
	static Mat4 identity = Mat4().initIdentity();
	if (transform) {
		// We have a transform matrix
		shader->setUniformMatrix4f("transform", *transform->getTransformation());
	}
	else {
		shader->setUniformMatrix4f("transform", identity);
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
		shader->setUniformMatrix4f("transform", *transform->getTransformation());
	}
	else {
		shader->setUniformMatrix4f("transform", Mat4().initIdentity());
	}
	model2D->renderBuffersOnly();
}

void Mesh2D::unbindForRender() const {
	model2D->unbindForRender();
}

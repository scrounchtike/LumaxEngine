
#ifndef FULL_MODEL_3D_HPP
#define FULL_MODEL_3D_HPP

#include "Model3D.hpp"

#include "../math.hpp"

class FullModel3D {
public:
	FullModel3D() { }
	FullModel3D(const std::vector<Model3D*> models) : models(models) { }
	std::vector<Model3D*> models;

	void update() {
		for (Model3D* model : models)
			model->update();
	}

	void addRotation(Vec3 rotation) {

	}

	void addScale(Vec3 scale) {

	}

	void render() const {
		for (Model3D* model : models)
			model->render();
	}

	void bindForRender() const {
		for (Model3D* model : models)
			model->bindForRender();
	}

	void renderBuffersOnly() const {
		for (Model3D* model : models)
			model->renderBuffersOnly();
	}

	void unbindForRender() const {
		for (Model3D* model : models)
			model->unbindForRender();
	}
};

#endif
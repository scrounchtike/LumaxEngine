
#ifndef MESH_3D_HPP
#define MESH_3D_HPP

#include "Material.hpp"
#include "Shader.hpp"
#include "Model3DGL.hpp"
#include "Model3DDX11.hpp"
#include "../math/Transform3D.hpp"
#include "Camera.hpp"
#include "ShaderDX11.hpp"
#include "../PL/PhysicsPrimitives.hpp"

struct FullModel3D {
#ifdef _USE_OPENGL
	typedef Model3DGL Model3D;
#elif defined _USE_DIRECTX11
	typedef Model3DDX11 Model3D;
#endif
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

struct FullMesh3D {
	FullModel3D model;
	// Add textures and possibly transforms
};

class Mesh3D {
public:
#ifdef _USE_OPENGL
	Mesh3D(Model3DGL* model, Material* material, Shader* shader, Transform3D* transform = nullptr, PhysicsPrimitive* physics = nullptr);
#elif defined _USE_DIRECTX11
	Mesh3D(Model3DDX11* model, Material* material, Shader* shader, Transform3D* transform = nullptr, PhysicsPrimitive* physics = nullptr);
#endif
	Mesh3D(FullModel3D* models, Material* material, Shader* shader, Transform3D* transform = nullptr, PhysicsPrimitive* physics = nullptr);

	~Mesh3D();

	void update();

	void addTranslation(Vec3 translation) {
		// 1. Update transform if present
		if (transform) {

		}
		// 2. Update physics primitive
		if (physics) {
			physics->addTranslation(translation);
		}
	}

	void addRotation(Vec3 rotation) {
		// 1. Update transform
		if (transform) {

		}
		// 2. Update physics primitive
		if (physics) {
			physics->addRotation(rotation);
		}
	}

	void addScale(Vec3 scale) {
		// 1. Update transform
		if (transform) {

		}
		// 2. Update physics primitive
		if (physics) {
			physics->addScale(scale);
		}
	}

	void render(const Camera* camera) const;

	bool collidesWith(Mesh3D* mesh) const;

	void bindForRender(const Camera* camera) const;
	void renderBuffersOnly() const;
	void unbindForRender() const;

	Transform3D* getTransform() const {
		return transform;
	}
	PhysicsPrimitive* getPhysics() const {
		return physics;
	}
	Material* getMaterial() const {
		return material;
	}
private:
	FullModel3D* fullModel;
	Material* material;
	Shader* shader;
	Transform3D* transform;
	PhysicsPrimitive* physics;
};

#endif
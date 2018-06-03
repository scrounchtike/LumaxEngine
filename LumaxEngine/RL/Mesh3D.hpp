
#ifndef MESH_3D_HPP
#define MESH_3D_HPP

#include "Material.hpp"
#include "Shader.hpp"
#include "Model3D.hpp"
#include "../math/Transform3D.hpp"
#include "Camera.hpp"
#include "../PL/PhysicsPrimitives.hpp"
#include "FullModel3D.hpp"

class Mesh3D {
public:
	Mesh3D(Model3D* model, Material* material, Shader* shader, Transform3D* transform = nullptr, PhysicsPrimitive* physics = nullptr);
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
	
	FullModel3D* fullModel;
	Material* material;
	Transform3D* transform;
	PhysicsPrimitive* physics;

	// Abstract shaders for easier use
	void addShader(Shader* shader) { shaders.push_back(shader); }
	Shader* getShader() const { assert(shaders.size()); return shaders[0]; }
	Shader* getGShader() const { assert(shaders.size()); return shaders[0];}
private:
	std::vector<Shader*> shaders;
};

#endif

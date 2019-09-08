
#ifndef MESH_2D_HPP
#define MESH_2D_HPP

#include "Shader.hpp"
#include "Model2D.hpp"
#include "Material.hpp"
#include "../math/Transform2D.hpp"

class Mesh2D {
public:
	Mesh2D(Model2D* model, Material* material, Shader* shader, Transform2D* transform = nullptr);

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;

	Transform2D* getTransform() const {
		return transform;
	}

	Material* getMaterial() const {
		return material;
	}
	
private:
	Model2D* model2D;
	Material* material;
	Shader* shader;
	Transform2D* transform;
};

#endif

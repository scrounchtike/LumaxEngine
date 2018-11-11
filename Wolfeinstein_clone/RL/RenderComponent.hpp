
#ifndef RENDER_COMPONENT_HPP
#define RENDER_COMPONENT_HPP

#include "ShaderPipeline.hpp"
#include "Material.hpp"
#include "Model3D.hpp"
#include "Model2D.hpp"

struct RenderComponent2D {
	RenderComponent2D(unsigned shaderPipelineID, Material& material, Model2D* model)
		: shaderPipelineID(shaderPipelineID), material(material), model(model) { }
	unsigned shaderPipelineID;
	Material& material;
	Model2D* model;
};

struct RenderComponent3D {
	RenderComponent3D(unsigned shaderPipelineID, Material& material, Model3D* model)
		: shaderPipelineID(shaderPipelineID), material(material), model(model) { }
	unsigned shaderPipelineID;
	Material& material;
	Model3D* model;
};

#endif

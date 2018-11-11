
#include "MedKit.hpp"

#include <cmath>
#include <iostream>
#include <cassert>

#include "../RL/Texture.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/Camera.hpp"

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"
#include "../RAL/RenderingContext.hpp"
#include "../RL/Renderer.hpp"

float MedKit::PICKUP_DISTANCE = 0.7f;
float MedKit::SCALE = 0.3f;

float MedKit::SIZEY = SCALE;
float MedKit::SIZEX = (float)(SIZEY / (1.9310344827586f * 1.2f));

float MedKit::OFFSET_X = 0.0f;
float MedKit::OFFSET_Y = 0.0f;

float MedKit::TEX_MAX_X = -1.0f - OFFSET_X;
float MedKit::TEX_MIN_X = -OFFSET_X;
float MedKit::TEX_MAX_Y = -1.0f - OFFSET_Y;
float MedKit::TEX_MIN_Y = -OFFSET_Y;

Model3D* MedKit::mesh = nullptr;

MedKit::MedKit(Vec3 position, Texture* texture) : texture(texture) {
	static float vertices[] = {
		-SIZEX, 0, 0,
		-SIZEX, SIZEY, 0,
		SIZEX, SIZEY, 0,
		SIZEX, 0, 0,
	};
	static float texCoords[] = {
		TEX_MAX_X, TEX_MIN_Y,
		TEX_MAX_X, TEX_MAX_Y,
		TEX_MIN_X, TEX_MAX_Y,
		TEX_MIN_X, TEX_MIN_Y,
	};
	static int indices[] = {
		0, 1, 2, 0, 2, 3,
	};
	static float normals[] = {
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
	};
	
	if(!mesh)
		mesh = new Model3D(vertices, 12, indices, 6, texCoords, normals);

	transform = new DynamicTransform3D(Vec3(position.x, position.y, position.z + 1));
}

void MedKit::update(Camera* camera){
	Vec3 direction = camera->getPosition() - transform->translation;
	//std::cout << direction.getLength() << std::endl;
	//if(direction.getLength() > 16)
	//	return;

	//std::cout << direction.x << std::endl;
	float angle = (float)(-std::atan2(direction.z, direction.x));
	if(direction.x > 0)
		angle = -(M_PI + angle);
	if(direction.x < 0)
		angle = -angle + M_PI;
	angle += M_PI/2.0f;

	//float angle = (float)(-std::atan2(direction.z, direction.x));
	
	//angle += M_PI/2.0f;
	
	transform->rotation = Vec3(0, angle, 0);
	transform->updateMatrix();

	if(direction.getLength() < PICKUP_DISTANCE){
		// On MedKit taken action
	}
}

void MedKit::render() const {
	lmx::setFaceCulling(true);
	texture->bind();

	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());

	Model3D* cube = lmx::getStaticResourceManager()->getModel3D("cube.obj");
	//cube->render();
	mesh->render();
}

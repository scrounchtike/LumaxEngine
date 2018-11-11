
#include "Door.hpp"

#include <chrono>

#include "../RL/Texture.hpp"
#include "../RL/Model3D.hpp"

#include "../RL/Renderer.hpp"

Model3D* Door::mesh = nullptr;

float Door::DOOR_DEPTH = 0.125f;
float Door::DOOR_HEIGHT = 1.0f;
float Door::DOOR_WIDTH = 1.0f;

float Door::OPEN_DISTANCE = 1.0f;
float Door::OPEN_DISTANCE_SQUARED = OPEN_DISTANCE*OPEN_DISTANCE;

float Door::TIME_TO_OPEN = 1000;
float Door::TIME_TO_CLOSE = 2000;

// Lerp : Move this somewhere more relevant
Vec3 lerpWolf(Vec3 A, Vec3 B, float alpha){
	return A + ((B - A) * alpha);
}

Door::Door(Vec3 position, Vec3 openPosition, Texture* texture) : closePosition(position), openPosition(openPosition), texture(texture)
{
	transform = new DynamicTransform3D();
	
	if(!mesh){
		float vertices[] = {
			0, 0, 0,
			0, DOOR_HEIGHT, 0,
			DOOR_WIDTH, DOOR_HEIGHT, 0,
			DOOR_WIDTH, 0, 0,
			
			0, 0, 0,
			0, DOOR_HEIGHT, 0,
			0, DOOR_HEIGHT, DOOR_DEPTH,
			0, 0, DOOR_DEPTH,
			
			0, 0, DOOR_DEPTH,
			0, DOOR_HEIGHT, DOOR_DEPTH,
			DOOR_WIDTH, DOOR_HEIGHT, DOOR_DEPTH,
			DOOR_WIDTH, 0, DOOR_DEPTH,
			
			DOOR_WIDTH, 0, 0,
			DOOR_WIDTH, DOOR_HEIGHT, 0,
			DOOR_WIDTH, DOOR_HEIGHT, DOOR_DEPTH,
			DOOR_WIDTH, 0, DOOR_DEPTH
		};
		
		float texCoords[] = {
			0.5f, 1,
			0.5f, 0.75f,
			0.75f, 0.75f,
			0.75f, 1,
			
			0.73f, 1,
			0.73f, 0.75f,
			0.75f, 0.75f,
			0.75f, 1,
			
			0.5f, 1,
			0.5f, 0.75f,
			0.75f, 0.75f,
			0.75f, 1,
			
			0.73f, 1,
			0.73f, 0.75f,
			0.75f, 0.75f,
			0.75f, 1,
		};
		
		int indices[] = {
			2, 1, 0, 3, 2, 0,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			14, 13, 12, 15, 14, 12
		};
		
		float normals[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			
			-1, 0, 0,
			-1, 0, 0,
			-1, 0, 0,
			-1, 0, 0,
			
			0, -1, 0,
			0, -1, 0,
			0, -1, 0,
			0, -1, 0,
			
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			1, 0, 0
		};
		mesh = new Model3D(vertices, 16*3, indices, 24, texCoords, normals);
	}
}

void Door::open(){
	if(isOpening)
		return;

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	openingStartTime = (float)((long)(ms.count())%10000000);
	openTime = openingStartTime + TIME_TO_OPEN;
	std::cout << openingStartTime << "    " << openTime << std::endl;
	closingStartTime = openTime + TIME_TO_CLOSE;
	closeTime = closingStartTime + TIME_TO_OPEN;

	isOpening = true;
}

Vec2 Door::getDoorSize(){
	if(rotated)
		return Vec2(DOOR_DEPTH, DOOR_WIDTH);
	else
		return Vec2(DOOR_WIDTH, DOOR_DEPTH);
}

void Door::update(){
	if(isOpening){
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		float time = (float)((long)(ms.count())%10000000);
		if(time < openTime){
			float lerpFactor = (float)(time - openingStartTime) / TIME_TO_OPEN;
			transform->translation = lerpWolf(closePosition, openPosition, lerpFactor);
			if(rotated)
				transform->translation += Vec3(0,0,-1);
			transform->updateMatrix();
		}
		else if(time < closingStartTime){
			transform->translation = openPosition;
			if(rotated)
				transform->translation += Vec3(0,0,-1);
			transform->updateMatrix();
		}
		else if(time < closeTime){
			float lerpFactor = (time - closingStartTime) / TIME_TO_OPEN;
			transform->translation = lerpWolf(openPosition, closePosition, lerpFactor);
			if(rotated)
				transform->translation += Vec3(0,0,-1);
			transform->updateMatrix();
		}
		else{
			transform->translation = closePosition;
			if(rotated)
				transform->translation += Vec3(0,0,-1);
			transform->updateMatrix();
			isOpening = false;
		}
	}
}

void Door::render() const {
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());

	texture->bind();

	mesh->render();
}

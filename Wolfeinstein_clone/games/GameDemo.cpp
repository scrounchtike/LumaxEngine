
#include "GameDemo.hpp"

#include "../RL/Model3D.hpp"
#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"
#include "../RL/Shader.hpp"
#include "../RL/LookAtCamera.hpp"
#include "../RL/Renderer.hpp"
#include "../RL/Texture.hpp"

#include "../GL/Enemy.hpp"
#include "../RL/WaterTile.hpp"

#include "../core/Level.hpp"
#include "../GL/LuaLevelLoader.hpp"

Level<Systems, RenderingSystems2D, RenderingSystems3D>* level;

void GameDemo::staticInits(){
	Enemy::init();
	WaterTile::initModel();
}

void GameDemo::init(){
	cube = lmx::getStaticResourceManager()->getModel3D("cube.obj");
	shader3D = lmx::getStaticResourceManager()->getShader("renderer/shader3D");
	//lmx::camera = new LookAtCamera(Vec3(0,0,0), Vec3(0,0,10));
	camera = lmx::getStaticCamera();
	textureTest = lmx::getStaticResourceManager()->getTexture("test.png");

	// Create an ECS level
	Log::println("Attempting to load lua level:");
	level = LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::loadLevel("levels/level_demo1.lua");
}

void GameDemo::input(){
	//camera->input();
	level->input();
}

void GameDemo::update(){
	camera->update();
	level->update();
}

void GameDemo::render() const {
	level->render();

	// This is a good test to make sure the GL rendering atleast works.
	// Rendering a cube
	/*
	shader3D->bind();

	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(10,0,-10).getHeadPointer());

	textureTest->bind();

	cube->render();
	*/
}

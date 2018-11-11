
#include "lumax.hpp"

#include "RAL/Window.hpp"
#include "RML/ResourceManager.hpp"
#include "RL/Camera.hpp"
#include "core/Level.hpp"

#include "main.hpp"
#include "core/Level.hpp"

#include "GL/LevelWolf.hpp"
#include "GL/Enemy.hpp"
#include "RL/RenderSystems.hpp"
#include "GL/LevelWolf.hpp"
#include "RL/RenderSystems.hpp"
#include "CL/Movement.hpp"
#include "CL/GeometryData.hpp"

Window* lmx::window = nullptr;
ResourceManager* lmx::resManager = nullptr;
Camera* lmx::camera = nullptr;
//LevelType* lmx::level = nullptr;

Window* lmx::getStaticWindow() {
	assert(lmx::window);
	return lmx::window;
}

float lmx::getStaticWindowWidth() {
	assert(lmx::window);
	return lmx::window->getWidth();
}

float lmx::getStaticWindowHeight() {
	assert(lmx::window);
	return lmx::window->getHeight();
}

// Resource Manager
ResourceManager* lmx::getStaticResourceManager() {
	assert(lmx::resManager);
	return lmx::resManager;
}

// Camera
Camera* lmx::getStaticCamera() {
	assert(lmx::camera);
	return lmx::camera;
}

//LevelType* lmx::getCurrentLevel(){
//	return lmx::level;
//}

// ECS Manager


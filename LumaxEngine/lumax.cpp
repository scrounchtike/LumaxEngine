
#include "lumax.hpp"

#include "RAL/Window.hpp"
#include "RML/ResourceManager.hpp"

Window* lmx::window = nullptr;
ResourceManager* lmx::resManager = nullptr;

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

// ECS Manager


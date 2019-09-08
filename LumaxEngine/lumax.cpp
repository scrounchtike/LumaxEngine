
#include "lumax.hpp"

#include <chrono>

#include "RAL/Window.hpp"
#include "RML/ResourceManager.hpp"
#include "main.hpp"
#include "core/Level.hpp"

Window* lmx::window = nullptr;
ResourceManager* lmx::resManager = nullptr;
LevelType* lmx::level = nullptr;

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

// Level
LevelType* lmx::getStaticLevel()
{
	assert(lmx::level);
	return lmx::level;
}

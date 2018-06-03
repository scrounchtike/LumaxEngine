
#include "main.hpp"

#include "RAL/Log.hpp"
#include "RAL/Window.hpp"

#include "GL/Level.hpp"
#include "GL/LuaLevelLoader.hpp"

#include "RAL/Window.hpp"
#include "RL/Renderer.hpp"
#include "RAL/input.hpp"

#include "core/Time.hpp"

#include "RML/loaders/ShaderLoader.hpp"
#include "RML/ResourceManager.hpp"

#include <sys/time.h>

const std::string TITLE = "Lumax Game Engine";
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Window
Window* window;
Window* getStaticWindow() {
	return window;
}

float getStaticWindowWidth() {
	return window->getWidth();
}

float getStaticWindowHeight() {
	return window->getHeight();
}

// Rendering context functions
#ifdef _USE_DIRECTX11
RenderingContextDX11* getStaticRenderingContextDX11() {
	return (RenderingContextDX11*)window->getRenderingContext();
}
#elif defined _USE_OPENGL
RenderingContextGL* getStaticRenderingContextGL() {
	return (RenderingContextGL*)window->getRenderingContext();
}
#endif

// Resource Manager
ResourceManager* resManager;
ResourceManager* getStaticResourceManager() {
	return resManager;
}

// Level
Level* level;

void run();

void input();
void render();
void update();

#ifdef _UNIX
// Main standard function
int main(int argc, char* argv[]) {
#elif defined _WINDOWS
// Main functions for Windows system
#ifdef _USE_WINAPI
// WinApi main function
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
#elif defined _USE_GLFW
int main(int argc, char* argv[]) {
#endif
#endif	

#ifdef UNICODE
	// TODO: Log an error for unicode
	Log::println("Error: Unicode characters not supported in code");
	assert(false);
#endif

	// Create Window
	window = new Window(0, TITLE, WIDTH, HEIGHT);

	// Initlialize Rendering Context
#ifdef _USE_DIRECTX11
	window->initDirectX11();
#elif defined _USE_OPENGL
	window->initOpenGL();
#endif

	// Input callback
#ifdef _USE_GLFW
	Input::getInputs = std::bind(&Window::input, window);
#elif defined _USE_DIRECTINPUT
	Input::getInputs = DirectInput::input;
#endif

	// Init static resource manager
	resManager = new ResourceManager("res");

	// Init physics collsion handlers
	PhysicsPrimitive::initialize();
	LuaLevelLoader::initLoaders();
	
	// Init Level
	level = LuaLevelLoader::loadLevel("levels/level_test.lua", resManager);
	
#ifdef _WINDOWS
	Time::initTimer();
#endif
	run();

	// Clean up
#ifdef _USE_DIRECTX11
	window->cleanUpDirectX11();
#elif defined _USE_OPENGL
	window->cleanUpOpenGL();
#endif
	delete window;

	// TEST: Input in cmd to stop
	// char c = getchar();
}

void run() {
	uint32 fps = 0;
	uint32 updates = 0;

#ifdef _WINDOWS
	float64 lastTime = Time::getTime();
	std::cout << lastTime << std::endl;
	float64 fpsTimeCounter = 0.0;
	float64 updateTimer = 1.0;
	float32 frameTime = 1000.0f / 60.0f; // (in milliseconds)
#elif defined _UNIX
	timeval lastTime, currentTime;
	gettimeofday(&lastTime, NULL);
	double deltaTime = 0; // in milliseconds
	double timeCounter = 0; // in milliseconds
#endif

	while (!window->shouldClose()) {
#ifdef _WINDOWS
		float64 currentTime = Time::getTime();
		float64 deltaTime = 10000 * (currentTime - lastTime);
		lastTime = currentTime;

		fpsTimeCounter += deltaTime;
		updateTimer += deltaTime;

		if (fpsTimeCounter >= 1000) {
			float64 msPerFrame = 1000.0 / (float64)fps;
			std::cout << "FPS TIMER: " << fps << " updates: " << updates << "   frame took " << msPerFrame << std::endl;
			fpsTimeCounter -= 1000, fps = 0, updates = 0;
		}
#elif defined _UNIX
		gettimeofday(&currentTime, NULL);
		deltaTime = (currentTime.tv_sec - lastTime.tv_sec) * 1000.0;
		deltaTime += (currentTime.tv_usec - lastTime.tv_usec) / 1000.0;
		lastTime = currentTime;
		timeCounter += deltaTime;

		if(timeCounter >= 1000.0){
			timeCounter -= 1000.0;
			std::cout << "FPS: " << fps << "  |  updates: " << updates << std::endl;
			fps = updates = 0;
		}
#endif

		input();
		update();
		render();
		++fps, ++updates;
		
		/*
		//bool shouldRender = false;
		//while (updateTimer >= frameTime) {
			input();
			update();
			++updates;

		//	updateTimer -= frameTime;
		//	shouldRender = true;
		//}
		
		//if (shouldRender) {
			render();
			++fps;
		//}
		//else {
			// sleep(1.0) // TODO: Sleep 1 ms
		//}
		*/
	}
}

void input() {
	Input::input();
}

void update() {
	level->update();
}

void render() {
	window->clear();

	// Rendering calls
	level->render();

	window->update();
}

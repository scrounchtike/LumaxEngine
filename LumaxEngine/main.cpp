
#include "main.hpp"

#include "RAL/Log.hpp"
#include "RAL/Window.hpp"

#include "GL/Level.hpp"
#include "loaders/LevelLoader.hpp"

#include "loaders/LevelLoader_lua.hpp"

#ifdef _USE_WINAPI
#include "RAL/WindowWinAPI.hpp"
#elif defined _USE_GLFW
#include "RAL/WindowGLFW.hpp"
#endif

#ifdef _USE_DIRECTX11
#include "RL/RendererDX11.hpp"
#elif defined _USE_OPENGL
#include "RL/RendererGL.hpp"
#endif

#include "core/Time.hpp"

// TEMP
#include "RL/ShaderDX11.hpp"
#include "RL/Model2DDX11.hpp"

#include "RL/ShaderGL.hpp"
#include "RL/Model2DGL.hpp"

#include "loaders/ShaderLoader.hpp"

const std::string TITLE = "Lumax Game Engine";
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Window
static Window* window;
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
static ResourceManager* resManager;
ResourceManager* getStaticResourceManager() {
	return resManager;
}

// Level
static Level* level;

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
#ifdef _USE_GLFW
	window = new WindowGLFW(0, TITLE, WIDTH, HEIGHT);
#elif defined _USE_WINAPI
	window = new WindowWinAPI(0, TITLE, WIDTH, HEIGHT);
#endif

	// Initlialize Rendering Context
#ifdef _USE_DIRECTX11
	window->initDirectX11();
#elif defined _USE_OPENGL
	window->initOpenGL();
#endif

	// Init static resource manager
	resManager = new ResourceManager("res");

	// Init physics collsion handlers
	PhysicsPrimitive::initialize();
	LuaLevelLoader::initLoaders();
	
	// Init Level
	//level = LevelLoader::loadLevel("level_test.lmx", resManager);
	level = LuaLevelLoader::loadLevel("levels/lua1.lua", resManager);

	Time::initTimer();
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
	float64 lastTime = Time::getTime();
	std::cout << lastTime << std::endl;
	float64 fpsTimeCounter = 0.0;
	float64 updateTimer = 1.0;
	float32 frameTime = 1000.0f / 60.0f; // (in milliseconds)

	while (!window->shouldClose()) {
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
	}
}

void input() {
	window->input();
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
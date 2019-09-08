
#include "main.hpp"

#include "RAL/Log.hpp"
#include "RAL/Window.hpp"

#include "GL/Level.hpp"
#include "GL/LuaLevelLoader.hpp"

#include "RAL/Window.hpp"
#include "RL/Renderer.hpp"
#include "RAL/input.hpp"

#include "core/Time.hpp"

#include "RML/ResourceManager.hpp"
#include "RAL/RenderingContext.hpp"

#include "RL/Renderer.hpp"

#include <sys/time.h>
#include <tuple>
#include <type_traits>

#include "utils/SMPL.hpp"
#include "lumax.hpp"

#include "RL/RenderSystems.hpp"
#include "core/Level.hpp"

#include "GL/Game1.hpp"

const std::string TITLE = "Lumax Game Engine";
const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 800;

// Level
//Level<Systems, RenderingSystems2D, RenderingSystems3D, RenderPasses2D, RenderPasses3D>* level;
//LevelType* lmx::getLevel()
//{
//	return level;
//}

ECSManagerLevelTest lmx::ecs;
ECSManagerLevelTest& lmx::getECSManager()
{
	return lmx::ecs;
}

void run();

void input();
void render();
void update();

template<typename... T> struct tl {};

template <typename T>
static constexpr bool isWindowType() {
	return std::is_same<T, Window>::value;
}
template <typename T>
using isWindow = std::integral_constant<bool, isWindowType<T>()>;

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
	Log::println("Error: Unicode characters not supported");
	assert(false);
#endif

	//
	// Create Window
	//
	lmx::window = new Window(0, TITLE, WIDTH, HEIGHT);
	assert(lmx::window);
	std::cout << "Window created successfully" << std::endl;
	
	//
	// Init static resource manager
	//
	lmx::resManager = new ResourceManager("res");
	assert(lmx::resManager);
	lmx::getStaticResourceManager();
	std::cout << "Resource Manager created successfully" << std::endl;

	//
	// Initialize Rendering Context
	//
	RenderingState state = RenderingState(WIDTH*2, HEIGHT*2); // default graphics state description (Retina display has 4x more pixels)
#ifdef _USE_DIRECTX11
	lmx::window->initDirectX11(state);
#elif defined _USE_OPENGL
	lmx::window->initOpenGL(state);
#endif
	float color[4] = { 0,0,1,1 };
	lmx::setClearColor(color);

	lmx::setBlending(true);

	//
	// Input callback
	//
#ifdef _USE_GLFW
	Input::getInputs = std::bind(&Window::input, lmx::window);
#elif defined _USE_DIRECTINPUT
	Input::getInputs = DirectInput::input;
#endif
	
	srand(time(NULL));
	
	// Init physics collsion handlers
	PhysicsPrimitive::initialize();
	//LuaLevelLoader::initLoaders();

	// Test Level Creation
	LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D, RenderPasses2D, RenderPasses3D>::initLoader();
	// Init level using Lua scripting
	//level = LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::loadLevel("levels/lmx2_test.lua");
	// Init level using C++ level constructor
	lmx::level = new Level<Systems, RenderingSystems2D, RenderingSystems3D, RenderPasses2D, RenderPasses3D>();
	Game1::loadLevel<Systems, RenderingSystems2D, RenderingSystems3D, RenderPasses2D, RenderPasses3D>(lmx::level);
	std::cout << "Level created successfully" << std::endl;

#ifdef _WINDOWS
	Time::initTimer();
#endif
	
	run();

	// Clean up
#ifdef _USE_DIRECTX11
	lmx::window->cleanUpDirectX11();
#elif defined _USE_OPENGL
	lmx::window->cleanUpOpenGL();
#endif
	delete lmx::window;

	std::cout << "Exiting Lumax Engine" << std::endl;
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

	while (!lmx::window->shouldClose()) {
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
	}
}

void input() {
	Input::input();
	lmx::level->input();
}

void update() {
	lmx::level->update();
}

void render() {
	lmx::window->clear();

	lmx::level->render();
	
	lmx::window->update();
}


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
#include "core/ECS.hpp"

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

template<typename... T> struct tl {};

template <typename T>
static constexpr bool isWindowType() {
	return std::is_same<T, Window>::value;
}
template <typename T>
using isWindow = std::integral_constant<bool, isWindowType<T>()>;

struct C0 {
	float a;
	float b;
};
struct C1 {};
struct C2 {
	float c;
};
struct C3 {
	float d = 4;
};
struct C4 {};

void TestSystemUpdate(C0& c0, C2& c2, C3& c3){
	std::cout << c0.a << " " << c0.b << " " << c2.c << " " << c3.d << std::endl;
	std::cout << "inside" << std::endl;
}
void test(C0& c0, C1& c1, C2& c2, C3& c3){
	std::cout << "inside2" << std::endl;
}

template <typename ECS>
class TestSystem : public System<ECS, C0, C2, C3> {
public:
	static void update(C0& c0, C2& c2, C3& c3) {
		std::cout << "system1 called!" << std::endl;
		std::cout << c0.a << " " << c0.b << " " << c2.c << " " << c3.d << std::endl;
	}
};
template <typename ECS>
class TestSystem2 : public System<ECS, C0, C2, C3> {
public:
	static void update(C0& c0, C2& c2, C3& c3) {
		std::cout << "system2 called!" << std::endl;
		std::cout << c0.a << std::endl;
	}
};


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
	
	// Create Window
	window = new Window(0, TITLE, WIDTH, HEIGHT);
	
	// Initialize Rendering Context
	RenderingState state = RenderingState(WIDTH*2, HEIGHT*2); // default graphics state description (Retina display has 4x more pixels)
#ifdef _USE_DIRECTX11
	window->initDirectX11(state);
#elif defined _USE_OPENGL
	window->initOpenGL(state);
#endif
	float color[4] = { 0,0,1,1 };
	lmx::setClearColor(color);
	
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

	// Renderer static intialization methods
	Renderer::initializeUBOs();
	Renderer::initializeGeometries();
	Renderer::initializeShaders();
	
	// Init Level
	level = LuaLevelLoader::loadLevel("levels/arch2_0.lua", resManager);
	
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
	
	level->render();
	
	window->update();
}

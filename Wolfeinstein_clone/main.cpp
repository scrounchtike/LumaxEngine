
#include "main.hpp"
#include "ECS_desc.hpp"
#include "RL/shaders/ShaderGL.hpp"

#include "RAL/Log.hpp"
#include "RAL/Window.hpp"

#include "RML/loaders/TextureLoader.hpp"

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

#include "GL/LevelWolf.hpp"
#include "GL/Enemy.hpp"
#include "RL/RenderSystems.hpp"
#include "GL/LevelWolf.hpp"
#include "CL/Movement.hpp"
#include "CL/GeometryData.hpp"
#include "PL/PhysicsPrimitives.hpp"

#include "TL/HeightMapTerrain.hpp"
#include "TL/Map.hpp"
#include "TL/FBmTerrain.hpp"

#include "RL/Skybox.hpp"
#include "RL/WaterTile.hpp"
#include "RL/WaterFramebuffer.hpp"

// Game
#include "GL/Game.hpp"

// Camera position
#define CAMPOSX 10
#define CAMPOSY 10
#define CAMPOSZ 0

const std::string TITLE = "Lumax Game Engine";
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

void staticInits(){
	// Init resource Manager
	lmx::resManager = new ResourceManager("res");
	Log::println("Resource Manager singleton created successfully.");

	// Initialize rendering layer
	// TODO: Make the screen size more consistent and not *2 for retina here...  -marcb 1 jan 2019
	RenderingState state = RenderingState(WIDTH*2, HEIGHT*2); // default graphics state description (Retina display has 4x more pixels)
#ifdef _USE_DIRECTX11
	lmx::window->initDirectX11(state);
#elif defined _USE_OPENGL
	lmx::window->initOpenGL(state);
#endif
	// Set Clear color to blue
	// Could already be in the default state?
	float color[4] = { 0,0,1,1 };
	lmx::setClearColor(color);
	// Renderer static data for rendering (makes sure there are no cross dependencies with future not intialized singletons)
	Renderer::initializeUBOs();
	Renderer::initializeGeometries();
	Renderer::initializeShaders();
	Log::println("Rendering layer and context initialized successfully");

	// Initialize input layer
	// Input callback
#ifdef _USE_GLFW
	Input::getInputs = std::bind(&Window::input, lmx::window);
#elif defined _USE_DIRECTINPUT
	Input::getInputs = DirectInput::input;
#endif
	Log::println("Input layer initialized successfully");

	// Initialize randomness
	// TODO: Move random functions to a Random file (if more features are needed)
	// Init RAND
	srand(time(0));

	// Init the static camera
	lmx::camera = new Camera(Vec3(CAMPOSX, CAMPOSY, CAMPOSZ));
	Log::println("Static camera initialized successfully");

	// Initialize physics layer
	PhysicsPrimitive::initialize();
	Log::println("Physics layer and primitives initialized successfully");

	// Init lua loader
	LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::initLoader();
	Log::println("Lua loader initialized successfully");

	// Game specific static inits
	Game::staticInits();
	Log::println("Game static inits initialized successfully");
	//Enemy::init();
	WaterTile::initModel();
}

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
	Log::println("Error: Unicode characters not supported");
	assert(false);
#endif
	
	// Create Window
	lmx::window = new Window(0, TITLE, WIDTH, HEIGHT);
	std::cout << "Window created successfully" << std::endl;

	// Static inits
	staticInits();
	
	// Enable clip planes
	// TODO: Move to proper rendering state function (init or default?)
	glEnable(GL_CLIP_DISTANCE0);

	Game::initGame();
	
	// Init Level
	//level_wolf = new LevelWolf("res/levels/level1.png");
	//LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::initLoader();
	//LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::loadLevel("levels/lmx2_test.lua");
	
	// Enable Translucency
	// TODO: Move to proper place for rendering initialization!
	lmx::setBlending(true);
	lmx::setBlendSrcOp(GL_SRC_ALPHA);
	lmx::setBlendDstOp(GL_ONE_MINUS_SRC_ALPHA);

	// Initialize timers
#ifdef _WINDOWS
	Time::initTimer();
#endif
	
	run();
	
	// Clean up
	// TODO: Cleanup more
#ifdef _USE_DIRECTX11
	lmx::window->cleanUpDirectX11();
#elif defined _USE_OPENGL
	lmx::window->cleanUpOpenGL();
#endif
	delete lmx::window;
}

void run() {
	uint32 fps = 0;
	uint32 updates = 0;
	
#ifdef _WINDOWS
	float64 lastTime = Time::getTime();
	float64 fpsTimeCounter = 0.0;
	float64 updateTimer = 1.0;
	float64 frameTimeCounter = 0; // in milliseconds
#elif defined _UNIX
	timeval lastTime, currentTime;
	gettimeofday(&lastTime, NULL);
	float64 deltaTime = 0; // in milliseconds
	float64 timeCounter = 0; // in milliseconds
	float64 frameTimeCounter = 0; // in milliseconds
#endif
	float32 frameTime = 1000.0f / 60.0f; // (in milliseconds)
	
	while (!lmx::window->shouldClose()) {
#ifdef _WINDOWS
		float64 currentTime = Time::getTime();
		float64 deltaTime = 10000 * (currentTime - lastTime);
		lastTime = currentTime;
		
		fpsTimeCounter += deltaTime;
		updateTimer += deltaTime;
		frameTimeCounter += deltaTime;

		if(frameTimeCounter >= frameTime){
			frameTimeCounter -= frameTime;
			input();
			update();
			render();
			++fps, ++updates;
		}
		
		if (fpsTimeCounter >= 1000) {
			float64 msPerFrame = 1000.0 / (float64)fps;
			std::cout << "FPS TIMER: " << fps << " updates: " << updates << "   frame took " << msPerFrame << std::endl;
			fpsTimeCounter -= 1000, fps = 0, updates = 0;
		}
#endif
		
#if defined _UNIX
		gettimeofday(&currentTime, NULL);
		deltaTime = (currentTime.tv_sec - lastTime.tv_sec) * 1000.0;
		deltaTime += (currentTime.tv_usec - lastTime.tv_usec) / 1000.0;
		lastTime = currentTime;
		timeCounter += deltaTime;
		frameTimeCounter += deltaTime;

		if(frameTimeCounter >= frameTime){
			frameTimeCounter -= frameTime;
			input();
			update();
			render();
			++fps, ++updates;
		}
		
		if(timeCounter >= 1000.0){
			timeCounter -= 1000.0;
			std::cout << "FPS: " << fps << "  |  updates: " << updates << std::endl;
			fps = updates = 0;
		}
#endif
	}
}

void input() {
	Input::input();
	Game::input();
}
 
void update() {
	Game::update();
}

 void render() {
	 lmx::window->clear();
	 Game::render();
	 lmx::window->update();
 };

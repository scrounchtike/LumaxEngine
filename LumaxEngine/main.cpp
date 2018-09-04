
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

const std::string TITLE = "Lumax Game Engine";
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Level
Level<Systems, RenderingSystems2D, RenderingSystems3D>* level;

ECSManagerLevelTest lmx::ecs;
ECSManagerLevelTest& lmx::getECSManager(){
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

	std::cout << "Beginning of main" << std::endl;
	
	// Create Window
	lmx::window = new Window(0, TITLE, WIDTH, HEIGHT);
	assert(lmx::window);
	std::cout << "Window created successfully" << std::endl;

	// Init static resource manager
	lmx::resManager = new ResourceManager("res");
	assert(lmx::resManager);
	lmx::getStaticResourceManager();
	std::cout << "Resource Manager created successfully" << std::endl;
	
	// Initialize Rendering Context
	RenderingState state = RenderingState(WIDTH*2, HEIGHT*2); // default graphics state description (Retina display has 4x more pixels)
#ifdef _USE_DIRECTX11
	lmx::window->initDirectX11(state);
#elif defined _USE_OPENGL
	lmx::window->initOpenGL(state);
#endif
	float color[4] = { 0,0,1,1 };
	lmx::setClearColor(color);
	
	// Input callback
#ifdef _USE_GLFW
	Input::getInputs = std::bind(&Window::input, lmx::window);
#elif defined _USE_DIRECTINPUT
	Input::getInputs = DirectInput::input;
#endif

	// Very important
	//lmx::ecs.initialize();
	std::cout << "Initialization of ECS done" << std::endl;
	
	// Init physics collsion handlers
	PhysicsPrimitive::initialize();
	//LuaLevelLoader::initLoaders();

	// Renderer static intialization methods
	Renderer::initializeUBOs();
	Renderer::initializeGeometries();
	Renderer::initializeShaders();
	
	// Init Level
	//level = LuaLevelLoader::loadLevel("levels/arch2_0.lua", resManager);

	// Test Level Creation
	LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::initLoader();
	level = LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::loadLevel("levels/lmx2_test.lua");
	
	// 1. ECS is taken care of globally (will move to Level.hpp)
	
	// 2. Load time asset loading
	
	// 2D Square
	{
		float vertices[8] = {
			-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5
		};
		float texCoords[8] = {
			0,0,0,1,1,1,1,0
		};
		int indices[6] = {
			0,1,2,2,3,0
		};
		Model2D* square = new Model2D(vertices, 8, indices, 6, texCoords);
		lmx::resManager->addModel2D("square2D", square);
	}
	// 3D square
	{
		float vertices[12] = {
			-0.5,-0.5,0,-0.5,0.5,0,0.5,0.5,0,0.5,-0.5,0
		};
 		int indices[6] = {
			0,1,2,2,3,0
		};
		Model3D* square = new Model3D(vertices, 12, indices, 6);
		lmx::resManager->addModel3D("square3D", square);
	}
	
	// Test texture
	lmx::resManager->addTexture("test", "test.png");
	// 2D Shader Pipeline
	{
		Shader* shader2D = lmx::resManager->getShader("renderer/shader2Dtexture");
		ShaderPipeline* pipeline2D = new ShaderPipeline(shader2D);
		lmx::resManager->addShaderPipeline("pipeline2D", pipeline2D);
	}
	{
		Shader* shader2Dcolor = lmx::resManager->getShader("renderer/shader2Dcolor");
		ShaderPipeline* pipeline2Dcolor = new ShaderPipeline(shader2Dcolor);
		lmx::resManager->addShaderPipeline("pipeline2Dcolor", pipeline2Dcolor);
	}
	{
		Shader* shader3Dcolor = lmx::resManager->getShader("renderer/shader3Dcolor");
		shader3Dcolor->addUniform("projection");
		shader3Dcolor->addUniform("view");
		shader3Dcolor->addUniform("transform");
		ShaderPipeline* pipeline3Dcolor = new ShaderPipeline(shader3Dcolor);
		lmx::resManager->addShaderPipeline("pipeline3Dcolor", pipeline3Dcolor);
	}
	{
		Shader* shader3Dtexture = lmx::resManager->getShader("renderer/shader3Dtexture");
		ShaderPipeline* pipeline3Dtexture = new ShaderPipeline(shader3Dtexture);
		lmx::resManager->addShaderPipeline("pipeline3Dtexture", pipeline3Dtexture);
	}

	// 3. Creation of the renderer
	Camera* camera = new Camera(70.0f, 0.1f, 1000.0f);
	Player* player = new Player(camera);
	Renderer* renderer = new Renderer(player);
	//lmx::ecs.setRenderer(renderer);
	
	// 4. Creation of entities + components
	
	Material* material1 = new Material(lmx::resManager->getTexture("test"));
	Vec3 colors[2] = { Vec3(1,0,0), Vec3(0,1,0) };
	Material* material2 = new Material(colors, 2);

	//lmx::ecs.initialize();
	LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::initLoader();
	level = LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::loadLevel("levels/lmx2_test.lua");

	/*
	uint32 square1 = ecs.createEntity();
	ecs.addComponent<Transform2D>(square1, new Transform2D(Vec2(0.5, 0)));
	ecs.addRenderComponent2D<smpl::type_list<Transform2D, Model2D>>(square1, resManager->getShaderPipeline("pipeline2D"), material1, resManager->getModel2D("square2D"), "pipeline2D");
	ecs.registerEntity(square1);
	

	
	uint32 square2 = ecs.createEntity();
	ecs.addComponent<DynamicTransform2D>(square2, new DynamicTransform2D(Vec2(-0.5, 0)));
	Movement2D* movement = new Movement2D();
	movement->tx = [](float delta) { return 0.001; };
	ecs.addComponent<Movement2D>(square2, movement);
	ecs.addRenderComponent2D<smpl::type_list<DynamicTransform2D, Model2D>>(square2, resManager->getShaderPipeline("pipeline2Dcolor"), material2, resManager->getModel2D("square2D"), "pipeline2Dcolor");
	ecs.registerEntity(square2);
	*/

	// Dumb yet necessary call = Bug waiting to happen...
	/**/
	//lmx::ecs.initRenderer(); // BAD!!
	/**/

	/*
	uint32 cube1 = ecs.createEntity();
	ecs.addComponent<DynamicTransform3D>(cube1, new DynamicTransform3D(Vec3(0,0,10)));
	ecs.addRenderComponent3D<smpl::type_list<DynamicTransform3D, Model3D>>(cube1, resManager->getShaderPipeline("pipeline3Dcolor"), new Material(colors, 2), resManager->getModel3D("cube.obj"), "pipeline3Dcolor");
	ecs.addComponent<Movement3D>(cube1, new Movement3D(Func3([](float delta){ return 0.1f * cos(2.0f * delta);}, function0, function0)));
	ecs.addPhysicsComponent(cube1, new AABB(Vec3(0,0,10), Vec3(1,1,1)));
	ecs.addToGroup<FlagRender1>(cube1);
	ecs.registerEntity(cube1);
	*/

	// Better way to create ECS entity
	/*
	ecs.makeEntity(new DynamicTransform3D(Vec3(-2,0,-2)),
								 new Movement3D(Func3([](float delta){ return 0.1f * cos(2.0f * delta);}, function0, function0)),
								 new AABB(Vec3(-2,0,-2), Vec3(1,1,1)),
								 new Material(colors, 2),
								 resManager->getModel3D("cube.obj"));
	*/

	srand(time(NULL));
	
	//GeometryDataComponent* geometry = new GeometryDataComponent();
	//geometry->polygons = ModelLoader::loadModelData("res/models/cube.obj");
	//std::cout << "geometry polygons = " << geometry->polygons.size() << std::endl;
	//uint32 entity1 = ecs.makeEntity(new DynamicTransform3D(Vec3(-2,0,-2)),
	//																new Material(colors, 2),
	//																geometry);
	
	//ecs.buildBSP();

	
	ShaderPipeline* pipeline1 = lmx::resManager->getShaderPipeline("pipeline3Dcolor");
	Material* material5 = new Material(Vec3(1,0,1));
	//uint32 entity7 = lmx::ecs.makeEntity(new Transform3D(Vec3(10,3,11)), pipeline1, material5, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity7"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(10,3,11)), pipeline1, material5, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity7"));
	// Entity creation should be:
	//ecs.makeEntity<ForwardRender>(new Transform3D(Vec3(10,3,11)), rM->getShaderPipeline(pipeline1), rM->getMaterial(material5), rM->getModel3D(cube), new PrintComponent("entity7"));
	Material* material3 = new Material(colors, 2);
	//uint32 entity1 = ecs.makeEntity(new Transform3D(Vec3(0,0,10)), pipeline1, material3, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity1"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(0,0,10)), pipeline1, material3, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity1"));
	ShaderPipeline* pipeline2 = lmx::resManager->getShaderPipeline("pipeline3Dcolor");
	//uint32 entity3 = ecs.makeEntity(new Transform3D(Vec3(-5,1,10)), pipeline2, new Material(Vec3(1,1,0)), lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity3"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(-5,1,10)), pipeline2, new Material(Vec3(1,1,0)), lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity3"));
	ShaderPipeline* pipeline3 = lmx::resManager->getShaderPipeline("pipeline3Dtexture");
	Material* material4 = new Material(lmx::resManager->getTexture("test.png"));
	//uint32 entity8 = ecs.makeEntity(new Transform3D(Vec3(-8, 6, 4)), pipeline3, material4, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity8"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(-8, 6, 4)), pipeline3, material4, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity8"));
	//uint32 entity5 = ecs.makeEntity(new Transform3D(Vec3(0,6,11)), pipeline3, material4, lmx::resManager->getModel3D("sphere.obj"), new PrintComponent("entity5"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(0,6,11)), pipeline3, material4, lmx::resManager->getModel3D("sphere.obj"), new PrintComponent("entity5"));
	//uint32 entity6 = ecs.makeEntity(new Transform3D(Vec3(5,5,10)), pipeline2, material5, lmx::resManager->getModel3D("sphere.obj"), new PrintComponent("entity6"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(5,5,10)), pipeline2, material5, lmx::resManager->getModel3D("sphere.obj"), new PrintComponent("entity6"));
	//uint32 entity2 = ecs.makeEntity(new Transform3D(Vec3(4,0,9)), pipeline1, material3, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity2"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(ecs, new Transform3D(Vec3(4,0,9)), pipeline1, material3, lmx::resManager->getModel3D("cube.obj"), new PrintComponent("entity2"));
	//uint32 entity4 = ecs.makeEntity(new Transform3D(Vec3(-6,5,11)), pipeline3, material4, lmx::resManager->getModel3D("sphere.obj"), new PrintComponent("entity4"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(lmx::ecs, new Transform3D(Vec3(-6,5,11)), pipeline3, material4, lmx::resManager->getModel3D("sphere.obj"));
	//ECSManagerLevelTest::MakeEntity<ECSManagerLevelTest, ForwardRender>::create(lmx::ecs, new Transform3D(Vec3(0,4,9)), pipeline3, material4, lmx::resManager->getModel3D("cube.obj"));
	//uint32 entitySphere = lmx::ecs.createEntity();
	//lmx::ecs.addToGroup<ForwardRender>(entitySphere);
	//lmx::ecs.addComponent<Transform3D>(entitySphere, new Transform3D(Vec3(0,4,9)));
	//lmx::ecs.addComponent<Model3D>(entitySphere, lmx::resManager->getModel3D("sphere.obj"));
	//lmx::ecs.addComponent<Material>(entitySphere, material4);
	//lmx::ecs.addComponent<ShaderPipeline>(entitySphere, pipeline3);
	//lmx::ecs.registerEntity(entitySphere);
	
	//lmx::ecs.sortDrawcalls();

	uint32 newEntity = level->createEntity();
	level->addToGroup<ForwardRender>(newEntity);
	level->addComponent<Transform3D>(newEntity, new Transform3D(Vec3(0,0,10)));
	level->addComponent<Model3D>(newEntity, lmx::resManager->getModel3D("cube.obj"));
	level->addComponent<Material>(newEntity, new Material(Vec3(1,0,1)));
	level->addComponent<ShaderPipeline>(newEntity, lmx::resManager->getShaderPipeline("pipeline3Dcolor"));
	level->registerEntity(newEntity);

	/*
	uint32 cube2 = ecs.createEntity();
	ecs.addComponent<DynamicTransform3D>(cube2, new DynamicTransform3D(Vec3(1,1,11)));
	ecs.addRenderComponent3D<smpl::type_list<DynamicTransform3D, Model3D>>(cube2, resManager->getShaderPipeline("pipeline3Dcolor"), new Material(colors, 2), resManager->getModel3D("cube.obj"), "pipeline3Dcolor");
	ecs.addPhysicsComponent(cube2, new AABB(Vec3(1,1,11), Vec3(1,1,1)));
	ecs.addToGroup<FlagRender1>(cube2);
	ecs.registerEntity(cube2);

	uint32 cube3 = ecs.createEntity();
	ecs.addComponent<DynamicTransform3D>(cube3, new DynamicTransform3D(Vec3(0,0,15)));
	ecs.addRenderComponent3D<smpl::type_list<DynamicTransform3D, Model3D>>(cube3, resManager->getShaderPipeline("pipeline3Dcolor"), new Material(colors, 2), resManager->getModel3D("cube.obj"), "pipeline3Dcolor");
	ecs.addPhysicsComponent(cube3, new AABB(Vec3(0,0,15), Vec3(1,1,1)));
	ecs.addToGroup<FlagRender1>(cube3);
	ecs.registerEntity(cube3);

	uint32 sphere1 = ecs.createEntity();
	ecs.addComponent<DynamicTransform3D>(sphere1, new DynamicTransform3D(Vec3(2,1,16)));
	ecs.addRenderComponent3D<smpl::type_list<DynamicTransform3D, Model3D>>(sphere1, resManager->getShaderPipeline("pipeline3Dcolor"), new Material(colors, 2), resManager->getModel3D("sphere.obj"), "pipeline3Dcolor");
	ecs.addPhysicsComponent(sphere1, new Sphere(Vec3(2,1,16), 1.0f));
	ecs.addComponent<Movement3D>(sphere1, new Movement3D(Func3([](float delta){ return 0.15f*cos(1.8f*delta); }, function0, function0)));
	ecs.registerEntity(sphere1);

	lmx::setFaceCulling(false);
	lmx::setDepthClip(false);
	*/
	
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

	// TEST: Input in cmd to stop
	// char c = getchar();

	std::cout << "end of main" << std::endl;
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
	level->input();
	//lmx::ecs.input();
	
}

void update() {
	level->update();
	//lmx::ecs.update();
}

 float temp = 0;
void render() {
	lmx::window->clear();

	level->render();
	
	//lmx::ecs.render();

	/*
	//lmx::setFaceCulling(false);
	//lmx::setDepthClip(false);
	
	getStaticResourceManager()->getShaderPipeline("pipeline3Dcolor")->bind();
	
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*0, sizeof(float)*16, ecs.getRenderer()->getCamera()->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*1, sizeof(float)*16, ecs.getRenderer()->getCamera()->getViewMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,0,-10).getHeadPointer());
	ecs.render();
	*/
	
	lmx::window->update();
}

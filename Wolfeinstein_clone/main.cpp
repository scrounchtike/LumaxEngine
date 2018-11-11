
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

const std::string TITLE = "Lumax Game Engine";
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Level<Systems, RenderingSystems2D, RenderingSystems3D>* level;

void run();

void input();
void render();
void renderScene();
void render2(Vec4 clip);
void update();

LevelWolf* level_wolf;

WaterFrameBuffer* fbo_reflection;
WaterFrameBuffer* fbo_refraction;

TileHolder* tile0;
TileHolder* tile1;
TileHolder* tile2;

void initTerrains(){
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//FBmTerrain* desc = new FBmTerrain();
	//tile0 = new TileHolder();
	//desc->generate_async_LOD0(tile0);
	
	//tile1 = new TileHolder();
	//desc->generate_async_LOD1(tile1);
	//desc->upgrade_to_LOD0(tile1);

	//tile2 = new TileHolder();
	//desc->generate_async_LOD1(tile2);
	
	// [x] LOD3->LOD2
	// [x] LOD2->LOD1
	// [x] LOD1->LOD0

	// [x] LOD0->LOD1
	// [x] LOD1->LOD2
	// [x] LOD2->LOD3
}

//static Map* map;

 void createWaterFBO(){
	 /*
	 const int width = 800;
	 const int height = 600;
	 
	 //GLuint fbo;
	 glGenFramebuffers(1, &fbo_reflection);
	 glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflection);
	 
	 //GLuint texture;
	 glGenTextures(1, &tex_reflection);
	 glBindTexture(GL_TEXTURE_2D, tex_reflection);
	 
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	 //glBindTexture(GL_TEXTURE_2D, 0);

	 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_reflection, 0);
	 
	 GLuint rbo;
	 glGenRenderbuffers(1, &rbo);
	 glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	 //glBindRenderbuffer(GL_RENDERBUFFER, 0);
	 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	 //GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	 //glDrawBuffers(1, drawBuffers);

	 std::cout << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	 std::cout << GL_FRAMEBUFFER_COMPLETE << std::endl;

	 glBindFramebuffer(GL_FRAMEBUFFER, 0);

	 //return std::pair<GLuint, GLuint>(fbo, texture);
	 */
 }

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

	// Init static resource manager
	lmx::resManager = new ResourceManager("res");
	lmx::getStaticResourceManager();
	std::cout << "Resource Manager created successfully" << std::endl;
	
	// Initialize Rendering Context
	RenderingState state = RenderingState(WIDTH*2, HEIGHT*2); // default graphics state description (Retina display has 4x more pixels)
#ifdef _USE_DIRECTX11
	lmx::window->initDirectX11(state);
#elif defined _USE_OPENGL
	lmx::window->initOpenGL(state);
#endif
	// Set Clear color to blue
	float color[4] = { 0,0,1,1 };
	lmx::setClearColor(color);
	
	// Input callback
#ifdef _USE_GLFW
	Input::getInputs = std::bind(&Window::input, lmx::window);
#elif defined _USE_DIRECTINPUT
	Input::getInputs = DirectInput::input;
#endif
	
	// Init RAND
	srand(time(NULL));
	
	// Create camera
	//lmx::camera = new Camera(Vec3(1000,10,1000));
	lmx::camera = new Camera(Vec3(0,10,0));
	
	// Init physics collsion handlers
	PhysicsPrimitive::initialize();
	//LuaLevelLoader::initLoaders();
	
	// Renderer static intialization methods
	Renderer::initializeUBOs();
	Renderer::initializeGeometries();
	Renderer::initializeShaders();
	
	Enemy::init();

	WaterTile::initModel();
	// Enable clip planes
	glEnable(GL_CLIP_DISTANCE0);

	//map = new Map(400, 100);
	
	// Init Level
	//level_wolf = new LevelWolf("res/levels/level1.png");
	//LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::initLoader();
	//level = LuaLevelLoader<Systems, RenderingSystems2D, RenderingSystems3D>::loadLevel("levels/lmx2_test.lua");

	// Enable Translucency
	lmx::setBlending(true);
	lmx::setBlendSrcOp(GL_SRC_ALPHA);
	lmx::setBlendDstOp(GL_ONE_MINUS_SRC_ALPHA);

	/*
	
	ResourceManager* resManager = lmx::getStaticResourceManager();
	
	// Test Medkit
	uint32 medkit_test = level->createEntity();
	level->addComponent<ShaderPipeline>(medkit_test, new ShaderPipeline(resManager->getShader("wolfeinstein/shader3D")));
	{
		float SIZEY = 0.7f;
		float SIZEX = (float)(SIZEY / (1.9310344827586f * 1.2f));
		float TEX_MAX_X = -1;
		float TEX_MIN_X = 0;
		float TEX_MAX_Y = -1;
		float TEX_MIN_Y = 0;
		
		static float vertices[] = {
			-SIZEX, 0, 0,
			-SIZEX, SIZEY, 0,
			SIZEX, SIZEY, 0,
			SIZEX, 0, 0,
		};
		static float texCoords[] = {
			TEX_MAX_X, TEX_MIN_Y,
			TEX_MAX_X, TEX_MAX_Y,
			TEX_MIN_X, TEX_MAX_Y,
			TEX_MIN_X, TEX_MIN_Y,
		};
		static int indices[] = {
			0, 1, 2, 0, 2, 3,
		};
		static float normals[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
		};
		static Model3D* model_medkit = new Model3D(vertices, 12, indices, 6, texCoords, normals);
		level->addComponent<Model3D>(medkit_test, model_medkit);
	}
	level->addComponent<DynamicTransform3D>(medkit_test, new DynamicTransform3D(Vec3(5,5,0)));
	level->addComponent<Material>(medkit_test, new Material(resManager->getTexture("medkit.png")));
	level->addToGroup<MEDKIT>(medkit_test);
	level->registerEntity(medkit_test);
	
	// Enemy Test
	uint32 enemy_test = level->createEntity();
	level->addComponent<Model3D>(enemy_test, Enemy::mesh);
	level->addComponent<Material>(enemy_test, new Material(resManager->getTexture("medkit.png")));
	level->addComponent<ShaderPipeline>(enemy_test, new ShaderPipeline(resManager->getShader("wolfeinstein/shader3D")));
	level->addComponent<DynamicTransform3D>(enemy_test, new DynamicTransform3D(Vec3(-4, 4, 4)));
	level->addComponent<EnemyComponent>(enemy_test, new EnemyComponent());
	level->addToGroup<ENEMY>(enemy_test);
	level->registerEntity(enemy_test);

	// Map test
	uint32 map_test = level->createEntity();
	level->addComponent<Model3D>(map_test, resManager->getModel3D("cube.obj"));
	level->addComponent<LevelWolfComponent>(map_test, new LevelWolfComponent("res/textures/levels/level1.png"));
	level->addComponent<Material>(map_test, new Material(resManager->getTexture("WolfCollection2.png")));
	level->addComponent<DynamicTransform3D>(map_test, new DynamicTransform3D());
	level->addComponent<ShaderPipeline>(map_test, new ShaderPipeline(resManager->getShader("wolfeinstein/shader3D")));
	level->registerEntity(map_test);
	*/
	
	// Terrain test
	//Model3D* terrain1_model = HeightMapTerrain::createHeightMapTerrain("res/textures/island_heightmap.png");
	/*
	uint32 terrain1 = level->createEntity();
	level->addComponent<Model3D>(terrain1, createHeightMapTerrain("res/textures/island_heightmap.png"));
	level->addComponent<Material>(terrain1, new Material(resManager->getTexture("test.png")));
	level->addComponent<ShaderPipeline>(terrain1, new ShaderPipeline(resManager->getShader("wolfeinstein/shader3D")));
	level->addComponent<Transform3D>(terrain1, new Transform3D());
	level->registerEntity(terrain1);
	*/

	initTerrains();

	//createWaterFBO();
	fbo_reflection = new WaterFrameBuffer();
	fbo_refraction = new WaterFrameBuffer();
	
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
}

void run() {
	uint32 fps = 0;
	uint32 updates = 0;
	
#ifdef _WINDOWS
	float64 lastTime = Time::getTime();
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
	//lmx::window->input();
	
	Input::input();
	//level->input();
	
	//level_wolf->input();
	
	//lmx::ecs.input();
	
}
 
void update() {
	//level->update();
	//level_wolf->update();
	
	//lmx::ecs.update();
	//map->update();
}

 void render() {
	 lmx::window->clear();

	 //renderScene();
	 Vec4 clip = Vec4(0,0,0,1);
	 render2(clip);

	 lmx::window->update();
 };
 
 void renderScene() {
	 //std::cout << fbo_reflection << "  " << tex_reflection << std::endl;
	 
	 fbo_refraction->bind();
	 glClearColor(0,0,1,1);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 // TODO: Implement clip planes
	 static Vec4 clip_refraction(0, -1, 0, 0);
	 render2(clip_refraction);
	 
	 fbo_refraction->unbind();

	 fbo_reflection->bind();
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	 float distance = 2.0 * lmx::camera->getPosition().y;
	 lmx::camera->addMovement(Vec3(0, -distance, 0));
	 lmx::camera->invertPitch();
	 lmx::camera->updateViewMatrix();

	 /// TODO: Implement clip planes
	 static Vec4 clip_reflection(0, 1, 0, 0);
	 //render2(clip_reflection);

	 lmx::camera->addMovement(Vec3(0, distance, 0));
	 lmx::camera->invertPitch();
	 lmx::camera->updateViewMatrix();
	 
	 fbo_reflection->unbind();
	 
	 
	 // WATER //
	static Shader* waterShader = new Shader("shaders/GLSL/renderer/shaderWater");
	waterShader->bind();
	
	waterShader->addUniform("reflectionTexture");
	waterShader->addUniform("refractiveTexture");
	waterShader->addUniform("dudvMap");
	waterShader->addUniform("normalMap");
	
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, lmx::camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, lmx::camera->getViewMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,0,0).mul(Mat4().initScale(200,0,200)).getHeadPointer());
	
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboWater);
	static float moveFactor = 0;
	moveFactor += 0.001;
	static float waterData[] = {0.04, 0, 64, 0.5};
	waterData[1] = moveFactor;
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, waterData, GL_DYNAMIC_DRAW);
	
	waterShader->setUniform1i("reflectionTexture", 0);
	waterShader->setUniform1i("refractionTexture", 1);
	waterShader->setUniform1i("dudvMap", 2);
	waterShader->setUniform1i("normalMap", 3);
	glActiveTexture(GL_TEXTURE3);
	static Texture* normalMap = lmx::getStaticResourceManager()->getTexture("matchingNormalMap.png");
	normalMap->bind();
	glActiveTexture(GL_TEXTURE2);
	static Texture* dudvMap = lmx::getStaticResourceManager()->getTexture("waterDUDV.png");
	dudvMap->bind();
	glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, tex_reflection);
	fbo_refraction->bindTexture();
	glActiveTexture(GL_TEXTURE0);
	static Texture* test = lmx::getStaticResourceManager()->getTexture("test.png");
	//test->bind();
	//glBindTexture(GL_TEXTURE_2D, tex_reflection);
	fbo_reflection->bindTexture();
	
	static WaterTile* water = new WaterTile();
	
	//water->update();
	//water->render();

	//map->renderWater();

	render2(Vec4(0,0,0,1));
 }

 float temp = 0;
void render2(Vec4 clip) {
	static Player* player = new Player(0.03f);
	player->update();
	Camera* camera = player->getCamera();
	
	static Shader* shaderSkybox = new Shader("shaders/GLSL/renderer/shader3Dskybox");
	shaderSkybox->bind();
	
	glDepthMask(false);
	
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(camera->getPosition()).mul(Mat4().initScale(550,550,550)).getHeadPointer());
	
  static std::string paths[6] = {"/textures/right.png", "/textures/left.png", "/textures/top.png", "/textures/bottom.png", "/textures/back.png", "/textures/front.png"};
	static Skybox* skybox = new Skybox(paths);

	lmx::setFaceCulling(false);
	skybox->update();
	skybox->render();
	lmx::setFaceCulling(true);
	
	glDepthMask(true);

	
	
	/*

	// MAPPING //

	static Shader* shaderNormal = new Shader("shaders/GLSL/renderer/shaderNormal");
	shaderNormal->bind();

	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(10,0,10).mul(Mat4().initScale(100,0,100)).getHeadPointer());
	
	static Model3D* tile;
	{
		static const float vertices[] = {
			0,0,0, 1,0,0, 1,0,1, 0,0,1
		};
		static const int indices[] = {
			0,1,2,2,3,0
		};
		static const float texCoords[] = {
			0,0, 0,1, 1,1, 1,0
		};
		static const float normals[] = {
			0,1,0, 0,1,0, 0,1,0, 0,1,0
		};
		static const float tangents[] = {
			1,0,0, 1,0,0, 1,0,0, 1,0,0
		};
		tile = new Model3D(vertices, 4, indices, 6, texCoords, normals, tangents);
	}

	shaderNormal->addUniform("texAlbedo");
	shaderNormal->addUniform("texNormal");
	shaderNormal->addUniform("texDisp");

	shaderNormal->addUniform("cameraPos");
	shaderNormal->setUniform3f("cameraPos", lmx::camera->getPosition());

	static Texture* normalTex = lmx::getStaticResourceManager()->getTexture("bricks2_normal.png");
	static Texture* bricks = lmx::getStaticResourceManager()->getTexture("bricks2.png");
	static Texture* dispTex = lmx::getStaticResourceManager()->getTexture("bricks2_disp.jpg");
	
	shaderNormal->setUniform1i("texAlbedo", 0);
	shaderNormal->setUniform1i("texNormal", 1);
	shaderNormal->setUniform1i("texDisp", 2);
	glActiveTexture(GL_TEXTURE2);
	dispTex->bind();
	glActiveTexture(GL_TEXTURE1);
	normalTex->bind();
	glActiveTexture(GL_TEXTURE0);
	bricks->bind();

	tile->render();
	*/
	
	//return;

	// PLANE //

	/*
	static Shader* shader3D = new Shader("shaders/GLSL/renderer/shader3D");
	shader3D->bind();
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,20,0).getHeadPointer());

	static Texture* texPlane = lmx::getStaticResourceManager()->getTexture("gold.png");
	texPlane->bind();
	
	static Model3D* plane = lmx::getStaticResourceManager()->getModel3D("dragon.obj");
	//plane->render();
	*/
	
	//return;

	// TERRAIN //
	
	//static Texture* texture3 = lmx::getStaticResourceManager()->getTexture("rocks.png");
	//static Texture* texture4 = lmx::getStaticResourceManager()->getTexture("sand.png");
	static Texture* texture1 = new Texture(TextureLoader::loadTextureSTB("res/textures/grass.png"));
	static Texture* texture2 = new Texture(TextureLoader::loadTextureSTB("res/textures/dirt.png"));
	static Texture* texture3 = new Texture(TextureLoader::loadTextureSTB("res/textures/rocks.png"));
	static Texture* texture4 = new Texture(TextureLoader::loadTextureSTB("res/textures/sand.png"));
	//static ShaderPipeline* shader = new ShaderPipeline(lmx::getStaticResourceManager()->getShader("terrain/terrainShader"));
	static Shader* shader = new Shader("shaders/GLSL/terrain/terrainShader");
	
	shader->bind();
	
	// Bind projection and view matrices
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
	
	shader->addUniform("texture1");
	shader->addUniform("texture2");
	shader->addUniform("texture3");
	shader->addUniform("texture4");
	
	shader->setUniform1i("texture1", 0);
	shader->setUniform1i("texture2", 1);
	shader->setUniform1i("texture3", 2);
	shader->setUniform1i("texture4", 3);
	
	shader->setUniform3f("cameraPos", camera->getPosition());
	
	glEnable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	texture4->bind();
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	texture3->bind();
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	texture2->bind();
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	texture1->bind();

	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,15,10).getHeadPointer());
	//model->render();

	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(100,15,10).getHeadPointer());
	//model2->render();
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(-100,15,10).getHeadPointer());
	//model2->render();
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,15,110).getHeadPointer());
	//model2->render();
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,20,0).getHeadPointer());
	//model2->render();
	//lmx::getStaticResourceManager()->getModel3D("cube.obj")->render();
	
	shader->addUniform("plane");
	shader->setUniform4f("plane", clip);
	
	//shader->addUniform("projection");
	//shader->addUniform("view");
	
	//shader->setUniformMatrix4f("projection", lmx::camera->getProjectionMatrix());
	//shader->setUniformMatrix4f("view", lmx::camera->getViewMatrix());
	
	//static Map* map = new Map(400, 100);
	
	//map->update();
	//map->render(shader);
	
	//static Model3D* terrain = HeightMapTerrain::createMPDTerrain();
	//static Model3D* terrain = HeightMapTerrain::createFaultTerrain();
	//static Model3D* terrain = HeightMapTerrain::createFBmTerrain(128, 0, 0);
	//TileHolder* tile = nullptr;
	//terrain->generate_async_LOD1(tile);

	/*
	static FBmTerrain* terrain0 = new FBmTerrain(0);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,0,0).getHeadPointer());
	terrain0->render();
	static FBmTerrain* terrain1 = new FBmTerrain(1);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(100,0,0).getHeadPointer());
	terrain1->render();
	static FBmTerrain* terrain2 = new FBmTerrain(2);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(200,0,0).getHeadPointer());
	terrain2->render();
	static FBmTerrain* terrain3 = new FBmTerrain(3);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(300,0,0).getHeadPointer());
	terrain3->render();
	*/

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,0,0).getHeadPointer());
	
	/*
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(0,0,0).getHeadPointer());
	tile0->terrain->render();
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(100,0,0).getHeadPointer());
	tile1->terrain->render();
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, Mat4().initTranslation(100,0,100).getHeadPointer());
	tile2->terrain->render();
	*/

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	static Map* map = new Map(400, 100);
	map->update();
	map->render(shader);
	
	//TileHolder* tile;
	//HeightMapTerrain::createFBmTerrainASYNC_LOD1_new(tile, 128, 0, 0);
	
	//terrainFault->render();
	
	//level->render();
	//level_wolf->render();
	
	//lmx::ecs.render();
}

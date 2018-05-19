
#include "LevelLoader_lua.hpp"

#include "TextureLoader.hpp"
#include "ModelLoader.hpp"
#include "../RL/lights.hpp"

#include "../utils/StringUtils.hpp"

ResourceManager*  LuaLevelLoader::resManager;
Level*			      LuaLevelLoader::level;

lua_State*        LuaLevelLoader::state;

PrimitiveLoaders  LuaLevelLoader::primitiveLoaders;

// Default variables
Transform3D*	    LuaLevelLoader::defaultTransform = nullptr;
Movement3D*		    LuaLevelLoader::defaultMovement = nullptr;
PhysicsPrimitive* LuaLevelLoader::defaultPhysics = nullptr;
Material*		      LuaLevelLoader::defaultMaterial = nullptr;
Shader*		        LuaLevelLoader::defaultShader = nullptr;
FullModel3D*      LuaLevelLoader::defaultFullMesh = nullptr;
std::string		    LuaLevelLoader::defaultRendergroup = "";

std::string		    LuaLevelLoader::updateMaterial = "pointer";
std::string		    LuaLevelLoader::updateShader = "pointer";
std::string		    LuaLevelLoader::updateTransform = "pointer";
std::string		    LuaLevelLoader::updateMovement = "pointer";

void print(const std::string& str) {
	std::cout << str << std::endl;
}

void LuaLevelLoader::loadLevelParameters(luabridge::LuaRef r) {
	if (r.isNil())
		assert(false);
	std::cout << "Loading level global parameters" << std::endl;

	//level->setClearColor(loadVector3D(getLuaRef(r, "clearColor"), Vec3(0, 0, 1)));
}

void LuaLevelLoader::updateDefaultVariables(luabridge::LuaRef r) {
	if (r.isNil())
		assert(false);
	std::cout << "Updating level default variables" << std::endl;

	// Load default variables if provided
	if (!r["material"].isNil())
		defaultMaterial = loadMaterial(r["material"]);
	if (!r["shader"].isNil())
		defaultShader = loadShader(r["shader"]);
	if (!r["transform"].isNil())
		defaultTransform = loadTransform3D(r["transform"]);
	if (!r["movement"].isNil())
		defaultMovement = loadMovement3D(r["movement"]);
	if (!r["physics"].isNil())
		defaultPhysics = loadPhysics(r["physics"]);
	if (!r["fullmesh"].isNil())
		defaultFullMesh = loadFullMesh3D(r["fullmesh"]);
	if (!r["rendergroup"].isNil())
		defaultRendergroup = r["rendergroup"].cast<std::string>();

	// Load assignment methods
	if (!r["updateMaterial"].isNil())
		updateMaterial = r["updateMaterial"].cast<std::string>();
	if (!r["updateShader"].isNil())
		updateShader = r["updateShader"].cast<std::string>();
	if (!r["updateTransform"].isNil())
		updateTransform = r["updateTransform"].cast<std::string>();
	if (!r["updateMovement"].isNil())
		updateMovement = r["updateMovement"].cast<std::string>();
}

Model3DGL* LuaLevelLoader::loadMesh3D(luabridge::LuaRef r) {
	luabridge::LuaRef _file = r["file"];
	// Assuming there is only one mesh in the file
	// If not, call loadFullMesh already!
	if (!_file.isNil())
		return ModelLoader::loadModel(_file.cast<std::string>())[0];

	std::vector<float> vertices;
	std::vector<int> indices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<float> tangents;

	// Load vertices
	loadFloatVector(getLuaRef(r, "vertices"), vertices);
	bool isIndexed = loadIntVector(getLuaRef(r, "indices"), indices);
	bool hasTexCoords = loadFloatVector(getLuaRef(r, "texCoords"), texCoords);
	bool hasNormals = loadFloatVector(getLuaRef(r, "normals"), normals);
	bool hasTangents = loadFloatVector(getLuaRef(r, "tangents"), tangents);

	Model3DGL* mesh = nullptr;
	if (!isIndexed) {
		// Not indexed
		if (hasTangents)
			mesh = new Model3DGL(vertices, texCoords, normals, tangents);
		else if (hasNormals)
			mesh = new Model3DGL(vertices, texCoords, normals);
		else if (hasTexCoords)
			mesh = new Model3DGL(vertices, texCoords);
		else
			mesh = new Model3DGL(vertices);
	}
	else {
		// Indexed
		if (hasTangents)
			mesh = new Model3DGL(vertices, indices, texCoords, normals, tangents);
		else if (hasNormals)
			mesh = new Model3DGL(vertices, indices, texCoords, normals);
		else if (hasTexCoords)
			mesh = new Model3DGL(vertices, indices, texCoords);
		else
			mesh = new Model3DGL(vertices, indices);
	}

	return mesh;
}

FullModel3D* LuaLevelLoader::loadFullMesh3D(luabridge::LuaRef r) {
	luabridge::LuaRef _file = r["file"];
	if (!_file.isNil())
		return new FullModel3D(ModelLoader::loadModel(_file.cast<std::string>()));

	std::vector<Model3DGL*> meshes;
	for (int i = 0; i < r.length(); ++i)
		meshes.push_back(loadMesh3D(r[i + 1]));

	return new FullModel3D(meshes);
}

TextureGL* LuaLevelLoader::loadTexture2D(luabridge::LuaRef r) {
	TextureGL* texture = TextureLoader::loadTextureSTB(checkLuaRef(r, "file").cast<std::string>());
	return texture;
}

Material* LuaLevelLoader::loadMaterial(luabridge::LuaRef r) {
	std::vector<Vec3> colors;

	if (!r["color"].isNil())
		colors.push_back(loadVector3D(r["color"]));
	else if (!r["baseColor"].isNil())
		colors.push_back(loadVector3D(r["baseColor"]));
	if (!r["hitColor"].isNil())
		colors.push_back(loadVector3D(r["hitColor"]));

	if (!colors.size())
		colors.push_back(Vec3(0, 0, 0));

	TextureGL* texture = nullptr;
	luabridge::LuaRef _texture = getLuaRef(r, "texture");
	if(!_texture.isNil())
		texture = loadTexture2D(_texture);

	Material* material;
	if (texture)
		material = new Material(texture);
	else
		material = new Material(&colors[0], colors.size());
	return material;
}

Shader* LuaLevelLoader::loadShader(luabridge::LuaRef r) {
	std::string filename = checkLuaRef(r, "file", "ERROR: Failed to load shader from lua level script. No shader file provided: ").cast<std::string>();
	return resManager->loadShader(filename);
}

PhysicsPrimitive* LuaLevelLoader::loadAABB(luabridge::LuaRef r) {
	Vec3 position = loadVector3D(getLuaRef(r, "position"), Vec3(0, 0, 0));
	Vec3 extents = loadVector3D(getLuaRef(r, "extents"), Vec3(1, 1, 1));
	return new AABB(position, extents);
}

PhysicsPrimitive* LuaLevelLoader::loadSphere(luabridge::LuaRef r) {
	Vec3 position = loadVector3D(getLuaRef(r, "position"), Vec3(0, 0, 0));
	float radius = loadFloat(getLuaRef(r, "radius"), 1.0);
	return new Sphere(position, radius);
}

PhysicsPrimitive* LuaLevelLoader::loadPlane(luabridge::LuaRef r) {
	Vec3 position = loadVector3D(getLuaRef(r, "position"), Vec3(0, 0, 0));
	Vec3 normal = loadVector3D(getLuaRef(r, "normal"), Vec3(0, 0, -1));
	if (dot(normal, normal) != 1.0)
		normal.normalize();
	Plane* plane = new Plane(position, normal);
	plane->rotationMatrix = Mat4().initIdentity();  //TODO: Get rid of this?
	plane->tangent = Vec3(1, 0, 0);
	plane->renderingScale = 1.0;
	return plane;
}

PhysicsPrimitive* LuaLevelLoader::loadOBB(luabridge::LuaRef r) {
	Vec3 position = loadVector3D(getLuaRef(r, "position"), Vec3(0, 0, 0));
	Vec3 u[3];
	u[0] = loadVector3D(getLuaRef(r, "u1"), Vec3(1, 0, 0));
	u[1] = loadVector3D(getLuaRef(r, "u2"), Vec3(0, 1, 0));
	u[2] = cross(u[0], u[1]);
	Vec3 extents = loadVector3D(getLuaRef(r, "extents"), Vec3(1, 1, 1));
	return new OBB(position, u, extents);
}

PhysicsPrimitive* LuaLevelLoader::loadRay(luabridge::LuaRef r) {
	Vec3 position = loadVector3D(getLuaRef(r, "position"), Vec3(0, 0, 0));
	Vec3 direction = loadVector3D(getLuaRef(r, "direction"), Vec3(0, 0, - 1));
	float allowNegativeT = loadFloat(getLuaRef(r, "allowNegativeT"), 0.0);
	return new Ray(position, direction, allowNegativeT);
}

PhysicsPrimitive* LuaLevelLoader::loadLine(luabridge::LuaRef r) {
	Vec3 start = loadVector3D(getLuaRef(r, "start"), Vec3(0, 0, 0));
	Vec3 end = loadVector3D(getLuaRef(r, "end_"), Vec3(1, 1, 1));
	return new Line(start, end);
}

PhysicsPrimitive* LuaLevelLoader::loadPhysics(luabridge::LuaRef r) {
	// Get physics type
	std::string stringType = checkLuaRef(r, "type", "ERROR: no type given in physics primitive table").cast<std::string>();

	// Get corresponding loader
	PrimitiveLoaders::iterator it = primitiveLoaders.find(stringType);
	if (it == primitiveLoaders.end()) {
		std::cerr << "ERROR: Unrecognized physics primitive type: " << stringType << std::endl;
		assert(false);
	}

	// Call loader with primitive
	PhysicsPrimitive* physics = it->second(r);

	return physics;
}

Transform3D* LuaLevelLoader::loadTransform3D(luabridge::LuaRef r) {
	Vec3 translation = loadVector3D(getLuaRef(r, "translation"), Vec3(0, 0, 0));
	Vec3 rotation = loadVector3D(getLuaRef(r, "rotation"), Vec3(0, 0, 0));
	Vec3 scale = loadVector3D(getLuaRef(r, "scale"), Vec3(1, 1, 1));
	return new Transform3D(translation, rotation, scale);
}

Movement3D* LuaLevelLoader::loadMovement3D(luabridge::LuaRef r) {
	Movement3D* movement = new Movement3D(nullptr);
	if (!r["translation"].isNil())
		 movement->t = loadMovComponent(r["translation"]);
	if (!r["rotation"].isNil())
		movement->r = loadMovComponent(r["rotation"]);
	if (!r["scale"].isNil())
		movement->s = loadMovComponent(r["scale"]);
	return movement;
}

Mesh3D* LuaLevelLoader::loadModel3D(luabridge::LuaRef r, std::string& rendergroup) {
	std::cout << "Loading Model3D from Lua level script" << std::endl;

	FullModel3D* fullMesh = defaultFullMesh;
	Material* material = defaultMaterial;
	Shader* shader = defaultShader;
	PhysicsPrimitive* physics = defaultPhysics;
	Transform3D* transform = defaultTransform;

	// Aux variables
	rendergroup = defaultRendergroup;
	Movement3D* movement = defaultMovement;

	// Load physics
	luabridge::LuaRef _physics = r["physics"];
	if (!_physics.isNil())
		physics = loadPhysics(_physics);

	// Load FullMesh3D
	luabridge::LuaRef _fullMesh = r["fullmesh"];
	if (!_fullMesh.isNil()) {
		if (_fullMesh.isString()) {	
			// Check if specific string value
			std::string flag_value = _fullMesh.cast<std::string>();
			if (flag_value == "genFromPhysics") {
				
			}
			else {
				// Unrecongnized string flag
				std::cerr << "Unrecognized string flag value in fullmesh : " << flag_value << std::endl;
				assert(false);
			}
		}else
			fullMesh = loadFullMesh3D(_fullMesh);
	}
	if(fullMesh){
		bool hasNormals = fullMesh->models[0]->hasNormals;
		std::cout << "Model has normals = " << hasNormals << std::endl;
	}

	// Load Material
	luabridge::LuaRef _material = r["material"];
	if (!_material.isNil())
		material = loadMaterial(_material);

	// Load Shader
	luabridge::LuaRef _shader = r["shader"];
	if (!_shader.isNil())
		shader = loadShader(_shader);

	// Load transform
	luabridge::LuaRef _transform = r["transform"];
	if (!_transform.isNil())
		transform = loadTransform3D(_transform);

	// Load render group
	luabridge::LuaRef _rendergroup = r["rendergroup"];
	if (!_rendergroup.isNil())
		rendergroup = _rendergroup.cast<std::string>();

	// Load 3D movement
	luabridge::LuaRef _movement = r["movement"];
	if (!_movement.isNil())
		movement = loadMovement3D(_movement);

	// Make copies of elements if necessary
	if (updateMaterial == "copy")
		material = new Material(*material);
	if (updateShader == "copy")
		shader = new ShaderGL(*((ShaderGL*)shader));
	if (updateTransform == "copy")
		transform = new Transform3D(*transform);
	if (updateMovement == "copy")
		movement = new Movement3D(*movement);

	Mesh3D* model = new Mesh3D(fullMesh, material, shader, transform, physics);
	
	// Link movement with model3D
	if (movement)
		movement->mesh = model, level->addMovement3D(movement);

	return model;
}

Attenuation* LuaLevelLoader::loadLightAttenuation(luabridge::LuaRef r){
	float constant = 1.0;
	float linear = 0.1;
	float exponent = 0.01;

	// No attenuation was provided
	if(r.isNil())
		return new Attenuation(constant, linear, exponent);
	
	if(!r["constant"].isNil())
		constant = r["constant"].cast<float>();
	if(!r["linear"].isNil())
		linear = r["linear"].cast<float>();
	if(!r["exponent"].isNil())
		exponent = r["exponent"].cast<float>();

	return new Attenuation(constant, linear, exponent);
}

DirectionalLight* LuaLevelLoader::loadDirectionalLight(luabridge::LuaRef r){
	Vec3 direction = Vec3(0,0,-1);
	Vec3 color = Vec3(1,1,1);
	float intensity = 1.0f;

	if(!r["direction"].isNil())
		direction = loadVector3D(r["direction"]).normalize();
	if(!r["color"].isNil())
		color = loadVector3D(r["color"]);
	if(!r["intensity"].isNil())
		intensity = r["intensity"].cast<float>();

	DirectionalLight* light = new DirectionalLight(direction, color, intensity);
	return light;
}

PointLight* LuaLevelLoader::loadPointLight(luabridge::LuaRef r){
	Vec3 position = Vec3(0,0,0);
	Vec3 color = Vec3(1,1,0);
	Attenuation* attenuation = loadLightAttenuation(r["attenuation"]);

	if(!r["position"].isNil())
		position = loadVector3D(r["position"]);
	if(!r["color"].isNil())
		color = loadVector3D(r["color"]);

	PointLight* light = new PointLight(position, color, *attenuation);
	return light;
}

SpotLight* LuaLevelLoader::loadSpotLight(luabridge::LuaRef r){
	PointLight* pointlight = nullptr;
	if(!r["pointlight"].isNil())
		pointlight = loadPointLight(r["pointlight"]);
	Vec3 direction = loadVector3D(r["direction"], Vec3(0,0,-1)).normalize();
	float cutoff = loadFloat(r["cutoff"], 0.5f);
	
	SpotLight* light = new SpotLight(*pointlight, direction, cutoff);
	return light;
}

void LuaLevelLoader::addModel3D(luabridge::LuaRef r){
	std::string rendergroup;
	Mesh3D* model = loadModel3D(r, rendergroup);
	
	if (rendergroup == "AABB")
		level->addAABB(model);
	else if (rendergroup == "Sphere")
		level->addSphere(model);
	else if (rendergroup == "Plane")
		level->addPlane(model);
	else if (rendergroup == "OBB")
		level->addOBB(model);
	else if (rendergroup == "Ray")
		level->addRay(model);
	else if (rendergroup == "Line")
		level->addLine(model);
	else if(rendergroup == "lighted")
		level->addLightedMesh3D(model);
	else
		level->addMesh3D(model);
}

void LuaLevelLoader::addDirectionalLight(luabridge::LuaRef r){
	level->addDirectionalLight(loadDirectionalLight(r));
}

void LuaLevelLoader::addPointLight(luabridge::LuaRef r){
	level->addPointLight(loadPointLight(r));
}

void LuaLevelLoader::addSpotLight(luabridge::LuaRef r){
	level->addSpotLight(loadSpotLight(r));
}

Level* LuaLevelLoader::loadLevel(const std::string& filename, ResourceManager* resManager) {
	// Set the resource manager
	LuaLevelLoader::resManager = resManager;

	// Create new camera for level
	Camera* camera = new Camera();

	// Create the renderer
#ifdef _USE_DIRECTX11
	RendererDX11* renderer;
	renderer = new RendererDX11(camera);
#elif defined _USE_OPENGL
	RendererGL* renderer;
	renderer = new RendererGL(camera);
#endif

	// Create the level
	Level* newLevel = new Level(renderer, camera);
	level = newLevel;

	// Start lua scripting support
	std::cout << "Starting lua scripting loading" << std::endl;
	state = luaL_newstate();
	luaL_openlibs(state);

	// Add necessary C++ functions for lua
	luabridge::getGlobalNamespace(state).
		addFunction("print", print).
		addFunction("addModel3D", addModel3D).
		addFunction("loadLevelParameters", loadLevelParameters).
		addFunction("updateDefaultVariables", updateDefaultVariables).
		addFunction("addDirectionalLight", addDirectionalLight).
		addFunction("addPointLight", addPointLight).
		addFunction("addSpotLight", addSpotLight);

	// Open and execute lua script
	if (luaL_loadfile(state, filename.c_str())) {
		std::cout << "ERROR: Lua level script not found: " << filename << std::endl;
		assert(false);
	}
	luaL_dofile(state, filename.c_str());

	// End lua session
	lua_close(state);

	return level;
}

void LuaLevelLoader::initLoaders() {
	primitiveLoaders.insert(PrimitivePair("AABB", loadAABB));
	primitiveLoaders.insert(PrimitivePair("Sphere", loadSphere));
	primitiveLoaders.insert(PrimitivePair("Plane", loadPlane));
	primitiveLoaders.insert(PrimitivePair("OBB", loadOBB));
	primitiveLoaders.insert(PrimitivePair("Ray", loadRay));
	primitiveLoaders.insert(PrimitivePair("Line", loadLine));
}

luabridge::LuaRef LuaLevelLoader::getLuaRef(luabridge::LuaRef r, const std::string& param) {
	return r[param.c_str()];
}

luabridge::LuaRef LuaLevelLoader::checkLuaRef(luabridge::LuaRef r, const std::string& param) {
	return checkLuaRef(r, param, "ERROR: Could not find param in checkLuaRef: ");
}

luabridge::LuaRef LuaLevelLoader::checkLuaRef(luabridge::LuaRef r, const std::string& param, const std::string& errorMsg) {
	luabridge::LuaRef _param = r[param.c_str()];
	if (_param.isNil()) {
		std::cerr << errorMsg << " " <<  r << std::endl;
		assert(false);
	}
	return _param;
}

Vec3 LuaLevelLoader::loadVector3D(luabridge::LuaRef r, const Vec3& def) {
	if (r.isNil())
		return def;
	return loadVector3D(r);
}

Vec2 LuaLevelLoader::loadVector2D(luabridge::LuaRef r, const Vec2& def) {
	if (r.isNil())
		return def;
	return loadVector2D(r);
}

float LuaLevelLoader::loadFloat(luabridge::LuaRef r, float def) {
	if (r.isNil())
		return def;
	return r.cast<float>();
}

bool LuaLevelLoader::loadFloatVector(luabridge::LuaRef r, std::vector<float>& vector) {
	if (r.isNil())
		return false;
	for (int i = 0; i < r.length(); ++i)
		vector.push_back(r[1 + i].cast<float>());
	return true;
}

bool LuaLevelLoader::loadIntVector(luabridge::LuaRef r, std::vector<int>& vector) {
	if (r.isNil())
		return false;
	for (int i = 0; i < r.length(); ++i)
		vector.push_back(r[1 + i].cast<int>());
	return true;
}

MovComponent* LuaLevelLoader::loadMovComponent(luabridge::LuaRef r) {
	MovComponent* component = new MovComponent();
	for (int i = 0; i < 3; ++i) {
		std::string str = r[i + 1].cast<std::string>();
		if (str.length() >= 3 && str.substr(0, 3) == "sin") {
			component->functions[i] = MovFunctions::fsin;
			loadMovFunction(str, component->amplitudes[i], component->frequencies[i]);
		}
		else if (str.length() >= 3 && str.substr(0, 3) == "cos") {
			component->functions[i] = MovFunctions::fcos;
			loadMovFunction(str, component->amplitudes[i], component->frequencies[i]);
		}
		else {
			// Float value
			component->value[i] = r[i + 1].cast<float>();
		}
	}
	return component;
}

void LuaLevelLoader::loadMovFunction(const std::string& str, float& amplitude, float& frequency) {
	std::vector<std::string> args = Utils::splitByRegex(str, '<');
	if (args.size() == 1) {
		amplitude = 1.0, frequency = 1.0;
		return;
	}
	args = Utils::splitByRegex(args[1], ',');
	std::string amplitude_ = args[0];
	std::cout << amplitude_ << std::endl;
	std::string frequency_ = args[1].substr(0, args[1].length() - 1);
	amplitude = std::stof(amplitude_);
	frequency = std::stof(frequency_);
}

Vec3 LuaLevelLoader::loadVector3D(luabridge::LuaRef r) {
	Vec3 result(r[1].cast<float>(), r[2].cast<float>(), r[3].cast<float>());
	return result;
}

Vec2 LuaLevelLoader::loadVector2D(luabridge::LuaRef r) {
	Vec2 result(r[1].cast<float>(), r[2].cast<float>());
	return result;
}

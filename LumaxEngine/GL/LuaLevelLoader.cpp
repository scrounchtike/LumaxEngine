
#include "LuaLevelloader.hpp"
#include "../main.hpp"

#include "../RL/lights.hpp"

#include "../utils/StringUtils.hpp"

ResourceManager*  LuaLevelLoader::resManager;
Level*			      LuaLevelLoader::level;
bool              LuaLevelLoader::initControls = false;
std::map<std::string, unsigned> LuaLevelLoader::keys;

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
	//std::cout << str << std::endl;
	Log::println(str);
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
	//if (!r["shader"].isNil())
	//	defaultShader = loadShader(r["shader"]);
	if (!r["transform"].isNil())
		defaultTransform = loadTransform3D(r["transform"]);
	if (!r["movement"].isNil())
		defaultMovement = loadMovement3D(r["movement"]);
	if (!r["physics"].isNil())
		defaultPhysics = loadPhysics(r["physics"]);
	if (!r["fullmesh"].isNil())
		defaultFullMesh = loadFullMesh3D(r["fullmesh"], false);
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

Model3D* LuaLevelLoader::loadMesh3D(luabridge::LuaRef r) {
	luabridge::LuaRef _file = r["file"];
	// Assuming there is only one mesh in the file
	// If not, call loadFullMesh already!
	if (!_file.isNil())
		return resManager->getModel3D(_file.cast<std::string>());

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

	Model3D* mesh = nullptr;
	if (!isIndexed) {
		// Not indexed
		if (hasTangents)
			mesh = new Model3D(vertices, texCoords, normals, tangents);
		else if (hasNormals)
			mesh = new Model3D(vertices, texCoords, normals);
		else if (hasTexCoords)
			mesh = new Model3D(vertices, texCoords);
		else
			mesh = new Model3D(vertices);
	}
	else {
		// Indexed
		if (hasTangents)
			mesh = new Model3D(vertices, indices, texCoords, normals, tangents);
		else if (hasNormals)
			mesh = new Model3D(vertices, indices, texCoords, normals);
		else if (hasTexCoords)
			mesh = new Model3D(vertices, indices, texCoords);
		else
			mesh = new Model3D(vertices, indices);
	}

	return mesh;
}

Model2D* LuaLevelLoader::loadMesh2D(luabridge::LuaRef r){
	std::vector<float> vertices;
	std::vector<int> indices;
	std::vector<float> texCoords;

	loadFloatVector(getLuaRef(r, "vertices"), vertices);
	bool isIndexed = loadIntVector(getLuaRef(r, "indices"), indices);
	bool hasTexCoords = loadFloatVector(getLuaRef(r, "texCoords"), texCoords);

	Model2D* mesh = nullptr;
	if(!isIndexed){
		if(hasTexCoords)
			mesh = new Model2D(vertices, texCoords);
		else
			mesh = new Model2D(vertices);
	}else{
		if(hasTexCoords)
			mesh = new Model2D(vertices, indices, texCoords);
		else
			mesh = new Model2D(vertices, indices);
	}
	assert(mesh);
	return mesh;
}

Model3D* LuaLevelLoader::loadAnimatedMesh3D(luabridge::LuaRef r){
	luabridge::LuaRef _file = r["file"];
	if(_file.isNil()){
		Log::println("Error: Animated meshes can only be loaded from files and through Assimp");
		assert(false);
	}

	return resManager->getAnimatedModel3D(_file.cast<std::string>());
}

FullModel3D* LuaLevelLoader::loadFullMesh3D(luabridge::LuaRef r, bool animated) {
	luabridge::LuaRef _file = r["file"];
	if (!_file.isNil())
		return getStaticResourceManager()->getFullModel3D(_file.cast<std::string>());
	
	std::vector<Model3D*> meshes;
	for (int i = 0; i < r.length(); ++i) {
		if(animated)
			meshes.push_back(loadAnimatedMesh3D(r[i + 1]));
		else
			meshes.push_back(loadMesh3D(r[i + 1]));
	}

	return new FullModel3D(meshes);
}

Texture* LuaLevelLoader::loadTexture2D(luabridge::LuaRef r) {
	Texture* texture = getStaticResourceManager()->getTexture(checkLuaRef(r, "file").cast<std::string>());
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

	Texture* texture = nullptr;
	luabridge::LuaRef _texture = getLuaRef(r, "texture");
	if(!_texture.isNil())
		texture = loadTexture2D(_texture);

	float blend = -1;
	luabridge::LuaRef _blend = getLuaRef(r, "blend");
	if(!_blend.isNil())
		blend = _blend.cast<float>();

	Material* material;
	if(texture && blend >= 0)
		material = new Material(texture, colors[0], blend);
	else if (texture)
		material = new Material(texture);
	else
		material = new Material(&colors[0], colors.size());
	return material;
}

ShaderPipeline* LuaLevelLoader::loadShader(luabridge::LuaRef r) {
	//std::string filename = checkLuaRef(r, "file", "ERROR: Failed to load shader from lua level script. No shader file provided: ").cast<std::string>();
	if(!r["file"].isNil()){
		std::string filename = r["file"].cast<std::string>();
		return new ShaderPipeline(getStaticResourceManager()->getShader(filename));
	}
	else{
		luabridge::LuaRef _vertex = checkLuaRef(r, "vertex", "Error: no vertex shader or shader file provided from lua level script");
		std::string vertexFile = _vertex["file"].cast<std::string>();
		luabridge::LuaRef _fragment = checkLuaRef(r, "fragment", "Error: no fragment shader or shader file provided from lua level script");
		std::string fragmentFile = _fragment["file"].cast<std::string>();
		
		Shader* vertexShader = getStaticResourceManager()->getVertexShader(vertexFile);
		Shader* fragmentShader = getStaticResourceManager()->getFragmentShader(fragmentFile);

		ShaderPipeline* pipeline = new ShaderPipeline(vertexShader, fragmentShader);
		return pipeline;
	}
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

Transform2D* LuaLevelLoader::loadTransform2D(luabridge::LuaRef r){
	Vec2 translation = loadVector2D(r["translation"], Vec2(0,0));
	float rotation = loadFloat(r["rotation"], 0);
	Vec2 scale = loadVector2D(r["scale"], Vec2(1,1));
	return new Transform2D(translation, rotation, scale);
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

Mesh3D* LuaLevelLoader::loadModel3D(luabridge::LuaRef r, std::string& rendergroup, bool animated) {
	std::cout << "Loading Model3D from Lua level script" << std::endl;

	FullModel3D* fullMesh = defaultFullMesh;
	Material* material = defaultMaterial;
	//Shader* shader = defaultShader;
	std::vector<Shader*> shaders;
	shaders.push_back(defaultShader);
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
			fullMesh = loadFullMesh3D(_fullMesh, animated);
	}
	//if(fullMesh){
	//	bool hasNormals = fullMesh->models[0]->hasNormals();
	//	std::cout << "Model has normals = " << hasNormals << std::endl;
	//}

	// Load Material
	luabridge::LuaRef _material = r["material"];
	if (!_material.isNil())
		material = loadMaterial(_material);

	// Load Shader
	//luabridge::LuaRef _shader = r["shader"];
	//if (!_shader.isNil())
	//	shaders[0] = loadShader(_shader);

	luabridge::LuaRef _shaders = r["shaders"];
	//if(!_shaders.isNil()){
	//	for(int i = 0; i < _shaders.length(); ++i){
	//		shaders[i] = loadShader(_shaders[i+1]);
	//	}
	//}

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
	//if (updateShader == "copy")
	// TODO: Update copy for shaders
	if (updateTransform == "copy")
		transform = new Transform3D(*transform);
	if (updateMovement == "copy")
		movement = new Movement3D(*movement);

	Mesh3D* model = new Mesh3D(fullMesh, material, shaders[0], transform, physics);
	for(int i = 1; i < shaders.size(); ++i)
		model->addShader(shaders[i]);
	
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

// v0.2 new interface methods
Mesh3D* LuaLevelLoader::loadStaticModel3D(luabridge::LuaRef r){
	// Load mesh
	FullModel3D* fullmesh = defaultFullMesh;
	if(!r["fullmesh"].isNil()){
		fullmesh = loadFullMesh3D(r["fullmesh"], defaultFullMesh);
	}else if(!r["mesh"].isNil()){
		std::vector<Model3D*> meshes;
		meshes.resize(1);
		meshes[0] = loadMesh3D(r["mesh"]);
		fullmesh = new FullModel3D(meshes);
	}
	assert(fullmesh);

	// Load material
	Material* material = defaultMaterial;
	if(!r["material"].isNil())
		material = loadMaterial(r["material"]);
	assert(material);

	// Load shader
	Shader* shader = defaultShader;
	//if(!r["shader"].isNil())
		//shader = loadShader(r["shader"]);
	assert(shader);

	// Load Transform3D
	Transform3D* transform = defaultTransform;
	if(!r["transform"].isNil())
		transform = loadTransform3D(r["transform"]);
	assert(transform);

	// Load physics
	PhysicsPrimitive* physics = defaultPhysics;
	if(!r["physics"].isNil())
		physics = loadPhysics(r["physics"]);

	// Load movement
	Movement3D* movement = defaultMovement;
	if(!r["movement"].isNil())
		movement = loadMovement3D(r["movement"]);

	// Load skeleton
	Skeleton* skeleton;
	
}

Mesh3D* LuaLevelLoader::loadDynamicModel3D(luabridge::LuaRef r){
	return nullptr;
}

void LuaLevelLoader::addStaticModel3D(luabridge::LuaRef r){
	Mesh3D* model = loadStaticModel3D(r);

	// Find the state of the model (lighted, animated, etc..)
	
}

void LuaLevelLoader::addDynamicModel3D(luabridge::LuaRef r){
	
}

void LuaLevelLoader::addModel3D(luabridge::LuaRef r){
	// Load mesh
	FullModel3D* fullmesh = defaultFullMesh;
	if(!r["fullmesh"].isNil()){
		fullmesh = loadFullMesh3D(r["fullmesh"], false);
	}else if(!r["mesh"].isNil()){
		std::vector<Model3D*> meshes;
		meshes.resize(1);
		meshes[0] = loadMesh3D(r["mesh"]);
		fullmesh = new FullModel3D(meshes);
	}
	assert(fullmesh);

	// Load material
	Material* material = defaultMaterial;
	if(!r["material"].isNil())
		material = loadMaterial(r["material"]);
	assert(material);

	// Load shader
	ShaderPipeline* pipeline = nullptr;
	std::string shaderName = "";
	if(!r["shader"].isNil()){
		pipeline = loadShader(r["shader"]);
		luabridge::LuaRef shaderRef = r["shader"];
		shaderName = shaderRef["name"].cast<std::string>();
	}
	assert(pipeline);
	assert(shaderName != "");
	
	// Load Transform3D
	Transform3D* transform = defaultTransform;
	if(!r["transform"].isNil())
		transform = loadTransform3D(r["transform"]);
	assert(transform);

	// Load physics
	PhysicsPrimitive* physics = defaultPhysics;
	if(!r["physics"].isNil())
		physics = loadPhysics(r["physics"]);

	// Load movement
	Movement3D* movement = defaultMovement;
	if(!r["movement"].isNil())
		movement = loadMovement3D(r["movement"]);

	// Load skeleton
	Skeleton* skeleton; // TODO

	level->addModel3D(fullmesh->models[0], pipeline, material, transform, shaderName);
}

void LuaLevelLoader::addModel2D(luabridge::LuaRef r){
	Model2D* mesh = nullptr;
	if(!r["mesh"].isNil())
		mesh = loadMesh2D(r["mesh"]);
	assert(mesh);

	ShaderPipeline* pipeline = nullptr;
	std::string shaderName = "";
	if(!r["shader"].isNil()){
		pipeline = loadShader(r["shader"]);
		luabridge::LuaRef shaderRef = r["shader"];
		shaderName = shaderRef["name"].cast<std::string>();
	}
	assert(pipeline);
	assert(shaderName != "");

	Material* material = nullptr;
	if(!r["material"].isNil())
		material = loadMaterial(r["material"]);
	assert(material);

	Transform2D* transform = new Transform2D();
	if(!r["transform"].isNil())
		transform = loadTransform2D(r["transform"]);

	level->addModel2D(mesh, pipeline, material, transform, shaderName);
}

void LuaLevelLoader::addInstancedModel3D(luabridge::LuaRef r, unsigned int count){
	
}

void LuaLevelLoader::addInstancedModel2D(luabridge::LuaRef r, unsigned int count){
	Model2D* mesh = nullptr;
	if(!r["mesh"].isNil())
		mesh = loadMesh2D(r["mesh"]);
	assert(mesh);

	std::cout << "0" << std::endl;

	ShaderPipeline* pipeline = nullptr;
	std::string shaderName = "";
	if(!r["shader"].isNil()){
		pipeline = loadShader(r["shader"]);
		std::cout << "loaded shader" << std::endl;
		luabridge::LuaRef shaderRef = r["shader"];
		shaderName = shaderRef["name"].cast<std::string>();
	}
	assert(pipeline);
	assert(shaderName != "");

	std::cout << "1" << std::endl;
	
	Material* material = nullptr;
	if(!r["material"].isNil())
		material = loadMaterial(r["material"]);
	assert(material);

	std::cout << "2" << std::endl;

	if(r["positions"].isNil())
		assert(false);
	luabridge::LuaRef _positions = r["positions"];
	std::vector<Vec2> positions;
	std::cout << count << std::endl;
	positions.reserve(count);
	for(int i = 0; i < count; ++i){
		float x = _positions[i*2+0+1].cast<float>();
		float y = _positions[i*2+1+1].cast<float>();
		positions.push_back(Vec2(x, y));
	}

	std::cout << "3" << std::endl;

	std::cout << positions.size() << std::endl;
	level->addInstancedModel2D(mesh, pipeline, material, positions, shaderName);
}

void LuaLevelLoader::addAnimatedModel3D(luabridge::LuaRef r){
	//std::string rendergroup;
	//Mesh3D* model = loadModel3D(r, rendergroup, true);
	//
	//level->addAnimatedMesh3D(model);
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

void LuaLevelLoader::addDeferredDirectionalLight(luabridge::LuaRef r){
	level->addDeferredDirectionalLight(loadDirectionalLight(r));
}

void LuaLevelLoader::addDeferredPointLight(luabridge::LuaRef r){
	level->addDeferredPointLight(loadPointLight(r));
}

void LuaLevelLoader::addDeferredSpotLight(luabridge::LuaRef r){
	level->addDeferredSpotLight(loadSpotLight(r));
}

void LuaLevelLoader::registerPlayer(luabridge::LuaRef r){
	level->player->speed = loadFloat(r["speed"], 0.35f);

	if(!r["controls"].isNil()){
		luabridge::LuaRef c = r["controls"];
		// Load custom control values
		initControls = true;
		
		registerControl(c["forward"], Player::FORWARD, 1.0);
		registerControl(c["backward"], Player::BACKWARD, -1.0); 
		registerControl(c["right"], Player::RIGHT, 1.0);
		registerControl(c["left"], Player::LEFT, -1.0);
		registerControl(c["up"], Player::UP, 1.0);
		registerControl(c["down"], Player::DOWN, -1.0);
	}
}

Level* LuaLevelLoader::loadLevel(const std::string& filename, ResourceManager* resManager) {
	initKeyMap();
	
	// Set the resource manager
	LuaLevelLoader::resManager = resManager;

	// Create default player
	Player* player = new Player(new Camera());

	// Create the renderer
	Renderer* renderer = new Renderer(player);

	// Create the level
	Level* newLevel = new Level(renderer, player, resManager);
	level = newLevel;

	// Start lua scripting support
	std::cout << "Starting lua scripting loading" << std::endl;
	state = luaL_newstate();
	luaL_openlibs(state);

	// Add necessary C++ functions for lua
	luabridge::getGlobalNamespace(state).
		addFunction("print", print).
		addFunction("addModel3D", addModel3D).
		addFunction("addModel2D", addModel2D).
		addFunction("addInstancedModel2D", addInstancedModel2D).
		addFunction("addInstancedModel3D", addInstancedModel3D).
		addFunction("addAnimatedModel3D", addAnimatedModel3D).
		addFunction("loadLevelParameters", loadLevelParameters).
		addFunction("updateDefaultVariables", updateDefaultVariables).
		addFunction("addDirectionalLight", addDirectionalLight).
		addFunction("addPointLight", addPointLight).
		addFunction("addSpotLight", addSpotLight).
		addFunction("addDeferredDirectionalLight", addDeferredDirectionalLight).
		addFunction("addDeferredPointLight", addDeferredPointLight).
		addFunction("addDeferredSpotLight", addDeferredSpotLight).
		addFunction("registerPlayer", registerPlayer);

	// Open and execute lua script
	if (luaL_loadfile(state, filename.c_str())) {
		std::cout << "ERROR: Lua level script not found: " << filename << std::endl;
		assert(false);
	}
	luaL_dofile(state, filename.c_str());

	// End lua session
	lua_close(state);

	// Post initialization
	if(!initControls){
		level->player->registerAxisControl(new InputControl(LMX_KEY_W, 1.0f), Player::FORWARD);
		level->player->registerAxisControl(new InputControl(LMX_KEY_S, -1.0f), Player::BACKWARD);
		level->player->registerAxisControl(new InputControl(LMX_KEY_A, -1.0f), Player::LEFT);
		level->player->registerAxisControl(new InputControl(LMX_KEY_D, 1.0f), Player::RIGHT);
		level->player->registerAxisControl(new InputControl(LMX_KEY_SPACE, 1.0f), Player::UP);
		level->player->registerAxisControl(new InputControl(LMX_KEY_LSHIFT, -1.0f), Player::DOWN);
	}

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

void LuaLevelLoader::registerControl(luabridge::LuaRef r, unsigned control, float weight){
	if(r.isNil())
		return;
	std::cout << "input" << std::endl;
	std::cout << "loading input for " << r.cast<std::string>() << std::endl;
	std::map<std::string, unsigned>::iterator it = keys.find(r.cast<std::string>());
	if(it == keys.end())
		std::cout << "Key not recognized in Player controls section" << std::endl, assert(false);
	unsigned key = it->second;

	level->player->registerAxisControl(new InputControl(key, weight), static_cast<Player::AxisControl>((int)control));
}

void LuaLevelLoader::initKeyMap(){
	keys.insert(std::pair<std::string,unsigned>("KEY_A", LMX_KEY_A));
	keys.insert(std::pair<std::string,unsigned>("KEY_B", LMX_KEY_B));
	keys.insert(std::pair<std::string,unsigned>("KEY_C", LMX_KEY_C));
	keys.insert(std::pair<std::string,unsigned>("KEY_D", LMX_KEY_D));
	keys.insert(std::pair<std::string,unsigned>("KEY_E", LMX_KEY_E));
	keys.insert(std::pair<std::string,unsigned>("KEY_F", LMX_KEY_F));
	keys.insert(std::pair<std::string,unsigned>("KEY_G", LMX_KEY_G));
	keys.insert(std::pair<std::string,unsigned>("KEY_H", LMX_KEY_H));
	keys.insert(std::pair<std::string,unsigned>("KEY_I", LMX_KEY_I));
	keys.insert(std::pair<std::string,unsigned>("KEY_J", LMX_KEY_J));
	keys.insert(std::pair<std::string,unsigned>("KEY_K", LMX_KEY_K));
	keys.insert(std::pair<std::string,unsigned>("KEY_L", LMX_KEY_L));
	keys.insert(std::pair<std::string,unsigned>("KEY_M", LMX_KEY_M));
	keys.insert(std::pair<std::string,unsigned>("KEY_N", LMX_KEY_N));
	keys.insert(std::pair<std::string,unsigned>("KEY_O", LMX_KEY_O));
	keys.insert(std::pair<std::string,unsigned>("KEY_P", LMX_KEY_P));
	keys.insert(std::pair<std::string,unsigned>("KEY_Q", LMX_KEY_Q));
	keys.insert(std::pair<std::string,unsigned>("KEY_R", LMX_KEY_R));
	keys.insert(std::pair<std::string,unsigned>("KEY_S", LMX_KEY_S));
	keys.insert(std::pair<std::string,unsigned>("KEY_T", LMX_KEY_T));
	keys.insert(std::pair<std::string,unsigned>("KEY_U", LMX_KEY_U));
	keys.insert(std::pair<std::string,unsigned>("KEY_V", LMX_KEY_V));
	keys.insert(std::pair<std::string,unsigned>("KEY_W", LMX_KEY_W));
	keys.insert(std::pair<std::string,unsigned>("KEY_X", LMX_KEY_X));
	keys.insert(std::pair<std::string,unsigned>("KEY_Y", LMX_KEY_Y));
	keys.insert(std::pair<std::string,unsigned>("KEY_Z", LMX_KEY_Z));
	keys.insert(std::pair<std::string,unsigned>("KEY_0", LMX_KEY_0));
	keys.insert(std::pair<std::string,unsigned>("KEY_1", LMX_KEY_1));
	keys.insert(std::pair<std::string,unsigned>("KEY_2", LMX_KEY_2));
	keys.insert(std::pair<std::string,unsigned>("KEY_3", LMX_KEY_3));
	keys.insert(std::pair<std::string,unsigned>("KEY_4", LMX_KEY_4));
	keys.insert(std::pair<std::string,unsigned>("KEY_5", LMX_KEY_5));
	keys.insert(std::pair<std::string,unsigned>("KEY_6", LMX_KEY_6));
	keys.insert(std::pair<std::string,unsigned>("KEY_7", LMX_KEY_7));
	keys.insert(std::pair<std::string,unsigned>("KEY_8", LMX_KEY_8));
	keys.insert(std::pair<std::string,unsigned>("KEY_9", LMX_KEY_9));
	keys.insert(std::pair<std::string,unsigned>("KEY_ESCAPE", LMX_KEY_ESCAPE));
	keys.insert(std::pair<std::string,unsigned>("KEY_SPACE", LMX_KEY_SPACE));
	keys.insert(std::pair<std::string,unsigned>("KEY_MINUS", LMX_KEY_MINUS));
	keys.insert(std::pair<std::string,unsigned>("KEY_EQUAL", LMX_KEY_EQUAL));
	keys.insert(std::pair<std::string,unsigned>("KEY_BACKSPACE", LMX_KEY_BACKSPACE));
	keys.insert(std::pair<std::string,unsigned>("KEY_TAB", LMX_KEY_TAB));
	keys.insert(std::pair<std::string,unsigned>("KEY_LBRACKET", LMX_KEY_LBRACKET));
	keys.insert(std::pair<std::string,unsigned>("KEY_RBRACKET", LMX_KEY_RBRACKET));
	keys.insert(std::pair<std::string,unsigned>("KEY_SEMICOLON", LMX_KEY_SEMICOLON));
	keys.insert(std::pair<std::string,unsigned>("KEY_ENTER", LMX_KEY_ENTER));
	keys.insert(std::pair<std::string,unsigned>("KEY_APOSTROPHE", LMX_KEY_APOSTROPHE));
	keys.insert(std::pair<std::string,unsigned>("KEY_COMMA", LMX_KEY_COMMA));
	keys.insert(std::pair<std::string,unsigned>("KEY_PERIOD", LMX_KEY_PERIOD));
	keys.insert(std::pair<std::string,unsigned>("KEY_SLASH", LMX_KEY_SLASH));
	keys.insert(std::pair<std::string,unsigned>("KEY_F1", LMX_KEY_F1));
	keys.insert(std::pair<std::string,unsigned>("KEY_F2", LMX_KEY_F2));
	keys.insert(std::pair<std::string,unsigned>("KEY_F3", LMX_KEY_F3));
	keys.insert(std::pair<std::string,unsigned>("KEY_F4", LMX_KEY_F4));
	keys.insert(std::pair<std::string,unsigned>("KEY_F5", LMX_KEY_F5));
	keys.insert(std::pair<std::string,unsigned>("KEY_F6", LMX_KEY_F6));
	keys.insert(std::pair<std::string,unsigned>("KEY_F7", LMX_KEY_F7));
	keys.insert(std::pair<std::string,unsigned>("KEY_F8", LMX_KEY_F8));
	keys.insert(std::pair<std::string,unsigned>("KEY_F9", LMX_KEY_F9));
	keys.insert(std::pair<std::string,unsigned>("KEY_F10", LMX_KEY_F10));
	keys.insert(std::pair<std::string,unsigned>("KEY_F11", LMX_KEY_F11));
	keys.insert(std::pair<std::string,unsigned>("KEY_F12", LMX_KEY_F12));
	keys.insert(std::pair<std::string,unsigned>("KEY_CAPSLOCK", LMX_KEY_CAPSLOCK));
	keys.insert(std::pair<std::string,unsigned>("KEY_LSHIFT", LMX_KEY_LSHIFT));
	keys.insert(std::pair<std::string,unsigned>("KEY_RSHIFT", LMX_KEY_RSHIFT));
	keys.insert(std::pair<std::string,unsigned>("KEY_SHIFT", LMX_KEY_SHIFT));
	keys.insert(std::pair<std::string,unsigned>("KEY_LCTRL", LMX_KEY_LCTRL));
	keys.insert(std::pair<std::string,unsigned>("KEY_RCTRL", LMX_KEY_RCTRL));
	keys.insert(std::pair<std::string,unsigned>("KEY_CTRL", LMX_KEY_CTRL));
	keys.insert(std::pair<std::string,unsigned>("KEY_LALT", LMX_KEY_LALT));
	keys.insert(std::pair<std::string,unsigned>("KEY_RALT", LMX_KEY_RALT));
	keys.insert(std::pair<std::string,unsigned>("KEY_ALT", LMX_KEY_ALT));
}

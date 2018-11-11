
#ifndef LUA_LEVEL_LOADER_TPP
#define LUA_LEVEL_LOADER_TPP

#include <iostream>

#include "../RAL/Keys.hpp"

#include "../RML/ResourceManager.hpp"
#include "../math/types.hpp"
#include "../math.hpp"

#include "../RL/ShaderPipeline.hpp"
#include "../RL/Model2D.hpp"
#include "../RL/Model3D.hpp"

#include "../lumax.hpp"

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>* LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::level;

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
lua_State* LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::state;
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
std::map<std::string, unsigned> LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::keys;

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
ResourceManager* LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::resManager;

void print(const std::string& str){
	std::cout << str << std::endl;
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadMeshes3D(luabridge::LuaRef s, luabridge::LuaRef r){
	std::cout << "Loading meshes 3D" << std::endl;
	assert(s.length() == r.length());
	
	for(int i = 0; i < r.length(); ++i){
		// Load each mesh
		luabridge::LuaRef meshRef = r[i+1];

		std::string name = s[i+1].cast<std::string>();
		if(!meshRef["file"].isNil()){
			// From file
			resManager->addModel3D(name, meshRef["file"].cast<std::string>());
		}else{
			// From static geometry
			std::vector<float> vertices;
			std::vector<int> indices;
			std::vector<float> texCoords;
			std::vector<float> normals;
			std::vector<float> tangents;
			
			// Load vertices
			LuaHelper::loadFloatVector(meshRef["vertices"], vertices);
			bool isIndexed = LuaHelper::loadIntVector(meshRef["indices"], indices);
			bool hasTexCoords = LuaHelper::loadFloatVector(meshRef["texCoords"], texCoords);
			bool hasNormals = LuaHelper::loadFloatVector(meshRef["normals"], normals);
			bool hasTangents = LuaHelper::loadFloatVector(meshRef["tangents"], tangents);
			std::cout << "load static geometry mesh: " << isIndexed << std::endl;
			std::cout << vertices.size() << std::endl;

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
			resManager->addModel3D(name, mesh);
		}
	}
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadMaterials(luabridge::LuaRef s, luabridge::LuaRef r){
	std::cout << "Loading materials" << std::endl;
	assert(s.length() == r.length());
	
	for(int i = 0; i < r.length(); ++i){
		// Load each material
		luabridge::LuaRef materialRef = r[i+1];

		std::string name = s[i+1].cast<std::string>();
		// Create material
		Material* material = new Material();
		if(!materialRef["color"].isNil()){
			// Load single color
			Vec3 color = LuaHelper::loadVector3D(materialRef["color"]);
			material->setColor(color);
		}else if(!materialRef["colors"].isNil()){
			// Load multiple colors
			std::vector<Vec3> colors;
			luabridge::LuaRef colorRef = materialRef["colors"];
			for(int c = 0; c < colorRef.length(); ++c){
				colors.push_back(LuaHelper::loadVector3D(colorRef[c+1]));
			}
			material->setColors(colors);
		}
		if(!materialRef["texture"].isNil()){
			luabridge::LuaRef textureRef = materialRef["texture"];
			if(textureRef["file"].isNil())
				std::cerr << "Error: Texture needs a file to be loaded." << std::endl, assert(false);
			Texture* texture = resManager->getTexture(textureRef["file"].cast<std::string>());
			material->setTexture(texture);
		}
		resManager->addMaterial(name, material);
	}
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadPipelines(luabridge::LuaRef s, luabridge::LuaRef r){
	std::cout << "Loading shader pipelines" << std::endl;
	assert(s.length() == r.length());

	for(int i = 0; i < r.length(); ++i){
		// Load each shader pipeline
		// TODO: Add support for more pipeline with different shader stages (Pipeline staging)
		luabridge::LuaRef pipelineRef = r[i+1];
		luabridge::LuaRef shaderRef = pipelineRef["shader"];
		if(shaderRef.isNil())
			std::cout << "Error: Pipeline needs a shader!" << std::endl, assert(false);
		if(shaderRef["file"].isNil())
			std::cerr << "Error: Shader needs a file" << std::endl, assert(false);
		
		std::string name = s[i+1].cast<std::string>();
		resManager->addShaderPipeline(name, new ShaderPipeline(resManager->getShader(shaderRef["file"].cast<std::string>())));
	}
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadDirectionalLight(luabridge::LuaRef r){
	
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadPointLights(luabridge::LuaRef r){
	for(int i = 0; i < r.length(); ++i){
		luabridge::LuaRef light = r[i+1];
		Vec3 position = LuaHelper::loadVector3D(light["position"]);
		Vec3 color = LuaHelper::loadVector3D(light["color"]);
		Vec3 attenuation = LuaHelper::loadVector3D(light["attenuation"]);
		level->addPointLight(PointLight(position, color, attenuation));
	}
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadSpotLights(luabridge::LuaRef r){
	
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::makeEntities(luabridge::LuaRef r){
	std::cout << "Adding entities to ECS : " << r.length() << std::endl;
	for(int i = 0; i < r.length(); ++i){
		// Create each entity
		uint32 entity = level->createEntity();
		
		luabridge::LuaRef entityRef = r[i+1];
		assert(!entityRef.isNil());
		luabridge::LuaRef keys = entityRef[1];
		luabridge::LuaRef components = entityRef[2];
		assert(!keys.isNil());
		assert(!components.isNil());
		assert(keys.length() == components.length());

		for(int c = 0; c < keys.length(); ++c){
			std::string key = keys[c+1].cast<std::string>();
			luabridge::LuaRef cref = components[c+1];
			if(key == "groups"){
				for(int g = 0; g < cref.length(); ++g){
					std::string group = cref[g+1].cast<std::string>();
					if(group == "ForwardRender")
						level->template addToGroup<ForwardRender>(entity);
				}
			}else if(key == "Transform3D")
				ComponentAdder::template execute<Transform3D>(level, entity, cref);
			else if(key == "DynamicTransform3D")
				ComponentAdder::template execute<DynamicTransform3D>(level, entity, cref);
			else if(key == "Mesh3D")
				ComponentAdder::template execute<Model3D>(level, entity, cref);
			else if(key == "Material")
				ComponentAdder::template execute<Material>(level, entity, cref);
			else if(key == "ShaderPipeline")
				ComponentAdder::template execute<ShaderPipeline>(level, entity, cref);
			else if(key == "LightComponent")
				ComponentAdder::template execute<LightComponent>(level, entity, cref), std::cout << "TESSSSSST" << std::endl;
		}
		
		//level->registerEntity(entity);
	}
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::initLoader(){
	// TODO: Move the init keymap function somewhere else (in core perhaps)
	initKeyMap();

	// Get static Resource Manager
	resManager = lmx::getStaticResourceManager();
	assert(resManager); // Making sure the static resource manager was initialized before hand

	// Create lua global state
	state = luaL_newstate();
	luaL_openlibs(state);
	
	// Register necessary C++ functions for lua
	luabridge::getGlobalNamespace(state).
		addFunction("print", print).
		addFunction("makeEntities", makeEntities).
		addFunction("loadMeshes3D", loadMeshes3D).
		addFunction("loadMaterials", loadMaterials).
		addFunction("loadPipelines", loadPipelines).
		addFunction("loadDirectionalLight", loadDirectionalLight).
		addFunction("loadPointLights", loadPointLights).
		addFunction("loadSpotLights", loadSpotLights);
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>* LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::loadLevel(const std::string& filename){
	level = new Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>();
	// Open and execute lua script
	if (luaL_loadfile(state, filename.c_str())) {
		std::cout << "ERROR: A problem occured while loading: " << filename << std::endl;
		std::cout << "Make sure the file exists in level directory and that it contains valid lua scripting" << std::endl;
		assert(false);
	}
	luaL_dofile(state, filename.c_str());

	// End lua session
	lua_close(state);

	return level;
}

Vec3 LuaHelper::loadVector3D(luabridge::LuaRef r, const Vec3& def) {
	if (r.isNil())
		return def;
	return loadVector3D(r);
}

Vec2 LuaHelper::loadVector2D(luabridge::LuaRef r, const Vec2& def) {
	if (r.isNil())
		return def;
	return loadVector2D(r);
}

float LuaHelper::loadFloat(luabridge::LuaRef r, float def) {
	if (r.isNil())
		return def;
	return r.cast<float>();
}

bool LuaHelper::loadFloatVector(luabridge::LuaRef r, std::vector<float>& vector) {
	if (r.isNil())
		return false;
	for (int i = 0; i < r.length(); ++i)
		vector.push_back(r[i+1].cast<float>());
	return true;
}

bool LuaHelper::loadIntVector(luabridge::LuaRef r, std::vector<int>& vector) {
	if (r.isNil())
		return false;
	for (int i = 0; i < r.length(); ++i)
		vector.push_back(r[1 + i].cast<int>());
	return true;
}

Vec3 LuaHelper::loadVector3D(luabridge::LuaRef r) {
	Vec3 result(r[1].cast<float>(), r[2].cast<float>(), r[3].cast<float>());
	return result;
}

Vec2 LuaHelper::loadVector2D(luabridge::LuaRef r) {
	Vec2 result(r[1].cast<float>(), r[2].cast<float>());
	return result;
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void LuaLevelLoader<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::initKeyMap(){
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

#endif

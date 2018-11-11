

#ifndef LEVEL_LOADER_LUA_HPP
#define LEVEL_LOADER_LUA_HPP

// Lua Bridge dependencies
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#ifdef _WINDOWS
#pragma comment(lib, "lua51.lib")
#endif
#include "LuaBridge.h"

#include "../math/types.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "../math/Transform3D.hpp"
#include "../math/Transform2D.hpp"

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"
#include "../CL/LightComponent.hpp"
#include "../core/Level.hpp"

// Forward declarations
class ResourceManager;
class Vector3;
class Vector2;
typedef Vector3 Vec3;
typedef Vector2 Vec2;

// Lua Helper
class LuaHelper {
public:
	static Vec3 loadVector3D(luabridge::LuaRef r, const Vec3& def);
	static Vec2 loadVector2D(luabridge::LuaRef r, const Vec2& def);
	static float loadFloat(luabridge::LuaRef, float def);
	static bool loadFloatVector(luabridge::LuaRef r, std::vector<float>& vector);
	static bool loadIntVector(luabridge::LuaRef r, std::vector<int>& vector);

	static Vec3 loadVector3D(luabridge::LuaRef r);
	static Vec2 loadVector2D(luabridge::LuaRef r);
};

// Component Adder to help create entities
template <typename Component>
struct ComponentAdder {
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		std::cerr << "Component is not supported" << std::endl;
		assert(false);
	}
};
// Component specializations
template <>
struct ComponentAdder<Transform3D>{
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		Vec3 translation(0,0,0);
		Vec3 rotation(0,0,0);
		Vec3 scale(1,1,1);
		if(!r["translation"].isNil())
			translation = LuaHelper::loadVector3D(r["translation"]);
		if(!r["rotation"].isNil())
			rotation = LuaHelper::loadVector3D(r["rotation"]);
		if(!r["scale"].isNil())
			scale = LuaHelper::loadVector3D(r["scale"]);
		Transform3D* transform = new Transform3D(translation, rotation, scale);
		addComponent(entity, transform);
	}
};
template <>
struct ComponentAdder<DynamicTransform3D>{
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		Vec3 translation(0,0,0);
		Vec3 rotation(0,0,0);
		Vec3 scale(1,1,1);
		if(!r["translation"].isNil())
			translation = LuaHelper::loadVector3D(r["translation"]);
		if(!r["rotation"].isNil())
			rotation = LuaHelper::loadVector3D(r["rotation"]);
		if(!r["scale"].isNil())
			scale = LuaHelper::loadVector3D(r["scale"]);
		DynamicTransform3D* transform = new DynamicTransform3D(translation, rotation, scale);
		addComponent(entity, transform);
	}
};
// TODO: PivotTransform3D
// TODO: Transform2D
// TODO: DynamicTransform2D
// TODO: PivotTransform2D
template <>
struct ComponentAdder<Model3D>{
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		addComponent(entity, lmx::getStaticResourceManager()->getModel3D(r.cast<std::string>()));
	}
};
template <>
struct ComponentAdder<Material>{
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		addComponent(entity, lmx::getStaticResourceManager()->getMaterial(r.cast<std::string>()));
	}
};
template <>
struct ComponentAdder<ShaderPipeline>{
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		addComponent(entity, lmx::getStaticResourceManager()->getShaderPipeline(r.cast<std::string>()));
	}
};

template <>
struct ComponentAdder<LightComponent>{
	template <typename Function>
	static void execute(Function&& addComponent, uint32 entity, luabridge::LuaRef r) noexcept {
		std::cout << "adding light component" << std::endl;
		LightComponent* lc = new LightComponent();
		//lc->pointLights[0] = (new PointLight(Vec3(2,2,12), Vec3(1,1,0), Vec3(0.1,0.1,0.1)));
		addComponent(entity, lc);
	}
};

// Overloaded Add Component for Level signature
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
struct AddComponent{
	template <typename Component>
	static void execute(Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>* level, uint32 entity, luabridge::LuaRef r){
		ComponentAdder<Component>::execute([level](uint32 entity, auto* t){
				typedef typename std::remove_reference<decltype(*t)>::type T;
				level->template addComponent<T>(entity, t);
			}, entity, r);
	}
};

// Lua Level Loader
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
class LuaLevelLoader {
	static Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>* level;
public:
	static void initLoader();
	
	static Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>* loadLevel(const std::string& levelName);
private:
	static void loadMeshes3D(luabridge::LuaRef s, luabridge::LuaRef r);
	static void loadMaterials(luabridge::LuaRef s, luabridge::LuaRef r);
	static void loadPipelines(luabridge::LuaRef s, luabridge::LuaRef r);
	static void loadDirectionalLight(luabridge::LuaRef r);
	static void loadPointLights(luabridge::LuaRef r);
	static void loadSpotLights(luabridge::LuaRef r);
	
	static void makeEntities(luabridge::LuaRef r);
	
	static lua_State* state;

	static ResourceManager* resManager;
	
	// TODO: Move keymap elsewhere more relevant
	static void initKeyMap();
	static std::map<std::string, unsigned> keys;

	using ComponentAdder = AddComponent<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>;
	
};



#include "LuaLevelLoader.tpp"

#endif

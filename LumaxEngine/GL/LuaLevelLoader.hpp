
#ifndef LEVEL_LOADER_LUA_HPP
#define LEVEL_LOADER_LUA_HPP

#include <string>
#include <map>
#include <iostream>
#include <functional>

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

#include "../GL/Level.hpp"
#include "../RML/ResourceManager.hpp"

#include "../RL/Mesh3D.hpp"
#include "../RL/Mesh2D.hpp"

#include "../RL/Model3D.hpp"
#include "../RL/Shader.hpp"
#include "../RL/ShaderPipeline.hpp"
#include "../RL/Texture.hpp"
#include "../RL/Material.hpp"

#include "../PL/PhysicsPrimitives.hpp"
#include "../PL/Movement3D.hpp"

#include "../math.hpp"

typedef std::function<PhysicsPrimitive*(luabridge::LuaRef)> PrimitiveLoader;
typedef std::map<std::string, PrimitiveLoader> PrimitiveLoaders;
typedef std::pair<std::string, PrimitiveLoader> PrimitivePair;

class LuaLevelLoader {
public:
	static Level* loadLevel(const std::string& filename, ResourceManager* resManager);

	// Init physics loaders
	static void initLoaders();
private:
	// Default variables
	static Transform3D* defaultTransform;
	static Movement3D* defaultMovement;
	static PhysicsPrimitive* defaultPhysics;
	static Material* defaultMaterial;
	static Shader* defaultShader;
	static FullModel3D* defaultFullMesh;
	static std::string defaultRendergroup;

	static std::string updateMaterial;
	static std::string updateShader;
	static std::string updateTransform;
	static std::string updateMovement;

	static void loadLevelParameters(luabridge::LuaRef r);
	static void updateDefaultVariables(luabridge::LuaRef r);

	static Texture* loadTexture2D(luabridge::LuaRef r);

	static PhysicsPrimitive* loadAABB(luabridge::LuaRef r);
	static PhysicsPrimitive* loadSphere(luabridge::LuaRef r);
	static PhysicsPrimitive* loadPlane(luabridge::LuaRef r);
	static PhysicsPrimitive* loadOBB(luabridge::LuaRef r);
	static PhysicsPrimitive* loadRay(luabridge::LuaRef r);
	static PhysicsPrimitive* loadLine(luabridge::LuaRef r);

	static PhysicsPrimitive* loadPhysics(luabridge::LuaRef r);
	static ShaderPipeline* loadShader(luabridge::LuaRef r);
	static Material* loadMaterial(luabridge::LuaRef r);
	static Transform3D* loadTransform3D(luabridge::LuaRef r);
	static Transform2D* loadTransform2D(luabridge::LuaRef r);
	static Movement3D* loadMovement3D(luabridge::LuaRef r);

	static Model3D* loadMesh3D(luabridge::LuaRef r);
	static Model2D* loadMesh2D(luabridge::LuaRef r);
	
	static Model3D* loadAnimatedMesh3D(luabridge::LuaRef r);
	static FullModel3D* loadFullMesh3D(luabridge::LuaRef r, bool animated);

	static Mesh3D* loadModel3D(luabridge::LuaRef r, std::string& rendergroup, bool animated);

	static Attenuation* loadLightAttenuation(luabridge::LuaRef r);
	static DirectionalLight* loadDirectionalLight(luabridge::LuaRef r);
	static PointLight* loadPointLight(luabridge::LuaRef r);
	static SpotLight* loadSpotLight(luabridge::LuaRef r);

	// Lua interface methods (Have to be void for some reason?)
	static void addModel3D(luabridge::LuaRef r);
	static void addModel2D(luabridge::LuaRef r);
	static void addInstancedModel3D(luabridge::LuaRef r, unsigned int count);
	static void addInstancedModel2D(luabridge::LuaRef r, unsigned int count);
	
	static void addDirectionalLight(luabridge::LuaRef r);
	static void addPointLight(luabridge::LuaRef r);
	static void addSpotLight(luabridge::LuaRef r);
	static void addDeferredDirectionalLight(luabridge::LuaRef r);
	static void addDeferredPointLight(luabridge::LuaRef r);
	static void addDeferredSpotLight(luabridge::LuaRef r);
	static void registerPlayer(luabridge::LuaRef r);

	// v0.2 interface methods
	static Mesh3D* loadStaticModel3D(luabridge::LuaRef r);
	static Mesh3D* loadDynamicModel3D(luabridge::LuaRef r);
	
	static void addStaticModel3D(luabridge::LuaRef r);
	static void addDynamicModel3D(luabridge::LuaRef r);

	static void addAnimatedModel3D(luabridge::LuaRef r);
	
	static ResourceManager* resManager;
	static Level* level;
	static bool initControls;

	static PrimitiveLoaders primitiveLoaders;

	// Lua state
	static lua_State* state;

	// Helper functions
	static luabridge::LuaRef getLuaRef(luabridge::LuaRef r, const std::string& param);
	static luabridge::LuaRef checkLuaRef(luabridge::LuaRef r, const std::string& param);
	static luabridge::LuaRef checkLuaRef(luabridge::LuaRef r, const std::string& param, const std::string& errorMsg);

	static Vec3 loadVector3D(luabridge::LuaRef r, const Vec3& def);
	static Vec2 loadVector2D(luabridge::LuaRef r, const Vec2& def);
	static float loadFloat(luabridge::LuaRef, float def);
	static bool loadFloatVector(luabridge::LuaRef r, std::vector<float>& vector);
	static bool loadIntVector(luabridge::LuaRef r, std::vector<int>& vector);

	static MovComponent* loadMovComponent(luabridge::LuaRef r);
	static void loadMovFunction(const std::string& str, float& amplitude, float& frequency);

	static Vec3 loadVector3D(luabridge::LuaRef r);
	static Vec2 loadVector2D(luabridge::LuaRef r);

	static void registerControl(luabridge::LuaRef r, unsigned control, float weight);
	static void initKeyMap();
	static std::map<std::string, unsigned> keys;
};

#endif

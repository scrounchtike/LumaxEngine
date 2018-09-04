
/*
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>
#include <map>

#include "../RAL/buildDesc.hpp"
#include "../RL/RenderPrimitives.hpp"

#include "../RL/Renderer.hpp"

#include "Player.hpp"

// Lights
#include "../RL/lights.hpp"

#include "../PL/PhysicsPrimitives.hpp"
#include "../PL/Movement3D.hpp"
#include "../RML/ResourceManager.hpp"

struct LightingDescription {
	std::vector<DirectionalLight*> directionalLights;
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;
};

class Level {
	friend class LuaLevelLoader;
public:
	Level(Renderer* renderer, Player* player, ResourceManager* resManager);
	~Level();

	DirectionalLight* addDirectionalLight(DirectionalLight* light);
	PointLight* addPointLight(PointLight* light);
	SpotLight* addSpotLight(SpotLight* light);

	DirectionalLight* addDeferredDirectionalLight(DirectionalLight* light);
	PointLight* addDeferredPointLight(PointLight* light);
	SpotLight* addDeferredSpotLight(SpotLight* light);

	Movement3D* addMovement3D(Movement3D* movement);

	void addModel3D(Model3D* mesh, ShaderPipeline* pipeline, Material* material, Transform3D* transform, const std::string& shaderName);
	void addModel2D(Model2D* mesh, ShaderPipeline* pipeline, Material* material, Transform2D* transform, const std::string& shaderName);
	void addInstancedModel3D(Model3D* mesh, ShaderPipeline* pipeline, Material* material, const std::vector<Vec2>& positions, const std::string& shaderName);
	void addInstancedModel2D(Model2D* mesh, ShaderPipeline* pipeline, Material* material, const std::vector<Vec2>& positions, const std::string& shaderName);

	void update();
	void render() const;

	// Specific level update functions
	void updateLevelTest();
private:
	// Implementation methods
	bool addMaterial3D(PipelineGroup3D* shaderGroup, Material* material);
	bool addMaterial2D(PipelineGroup2D* shaderGroup, Material* material);
	
	// Shader Groups
	std::vector<PipelineGroup3D*> groups3D;
	std::map<std::string, PipelineGroup3D*> mapGroups3D;

	std::vector<PipelineGroup2D*> groups2D;
	std::map<std::string, PipelineGroup2D*> mapGroups2D;
	
	// Physics
	std::vector<Mesh3D*> dynamicMeshes3D;
	std::vector<Mesh3D*> staticMeshes3D;
	
	// Lights
	LightingDescription lights;
	LightingDescription deferredLights;

	// Debug Movement
	std::vector<Movement3D*> movements3D;

	ResourceManager* resManager;
	Renderer* renderer;

	// Player
	Player* player;
};

#endif
*/

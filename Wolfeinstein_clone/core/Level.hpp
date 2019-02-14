
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <string>
#include <vector>

#include "ECS.hpp"

#include "../RL/lights.hpp"

#include "../main.hpp"

#include "../ECS_desc.hpp"

class Renderer;
class Camera;

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
class Level{
private:
	using ThisType = Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>;
	
	using SystemList = typename TSystemList::type;
	using RenderingSystem2DList = typename TRenderingSystem2DList::type;
	using RenderingSystem3DList = typename TRenderingSystem3DList::type;
	
	// ECS Interface
	using ComponentMap = typename ECSDesc::ComponentMap;
	
	using LevelECS = ECSManager<ECSDesc, TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>;
	using SystemTupleStorage = typename LevelECS::SystemTupleStorage;
	using RenderSystem2DTupleStorage = typename LevelECS::RenderSystem2DTupleStorage;
	using RenderSystem3DTupleStorage = typename LevelECS::RenderSystem3DTupleStorage;
	using ComponentTupleVectors = typename LevelECS::ComponentTupleVectors;
	
public:
	// Current working entity
	static uint32 current_entity;
	
	Level();

	// Lights interface
	void setDirectionalLight(const DirectionalLight& light){
		directionalLight = light;
	}
	DirectionalLight& getDirectionalLight() const {
		return directionalLight;
	}
	void addPointLight(const PointLight& light){
		pointLights.push_back(light);
	}
	std::vector<PointLight>& getPointLights() const {
		return pointLights;
	}
	void addSpotLight(const SpotLight& light){
		spotLights.push_back(light);
	}
	std::vector<SpotLight>& getSpotLights() const {
		return spotLights;
	}
	void fillStaticStaticLightComponent(uint32 entity, LightComponent* lc) const {
		
	}

	// Entity management
	uint32 createEntity();
	void removeEntity(uint32 entity);
	void registerEntity(uint32 entity);

	// This kinda shouldnt be here
	// A lot of ECS stuff should not be in the Level class really
	// Move this somewhere else?
	// Component management
	template <typename Component>
	void addComponent(uint32 entity, Component* component);
	void addLightComponent(uint32 entity, LightComponent* lc);
	template <typename Component>
	void removeComponent(uint32 entity);
	template <typename Component>
	Component* getComponent(uint32 entity);
	template <typename Component>
	bool hasComponent(uint32 entity);

	// Group management
	template <typename Group>
	void addToGroup(uint32 entity, bool value = true);

	// Flag management
	template <typename Flag>
	void setFlag(uint32 entity, bool value = true);

	void input();
	void update();
	void render();

	Renderer* getRenderer() const { return renderer; }
	Camera* getCamera() const;
private:
	// Entity Storage
	std::vector<Entity<ECSDesc>> entities;

	// Store lights
	DirectionalLight directionalLight;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	
	// Component Storage
	ComponentTupleVectors components;

	// System Storage
	// Systems <- dependent on ECS
	// Each level has unique systems
	SystemTupleStorage systems;
	RenderSystem2DTupleStorage renderSystems2D;
	RenderSystem3DTupleStorage renderSystems3D;

	Renderer* renderer = nullptr;
};
template <typename A, typename B, typename C>
uint32 Level<A, B, C>::current_entity = 0;

#include "Level.tpp"

#endif

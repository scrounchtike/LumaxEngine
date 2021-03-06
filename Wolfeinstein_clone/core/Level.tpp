
#ifndef LEVEL_TPP
#define LEVEL_TPP

#include "../PL/PrimitiveCollision.hpp"

#include "../RL/Renderer.hpp"
#include "../RL/Camera.hpp"

// IMPORTANT
// This method is called by the ECS system to pass the camera to the systems!
// Make sure this is the correct camera used!
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
Camera* Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::getCamera() const {
	//return new Camera();
	//return nullptr;
	return lmx::getStaticCamera();
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::Level() : renderer(new Renderer(new Player())) {
	// Initialize all systems : instances unique to this level
	// The following lines simply call constructors of each systems if it exists
	for_each<SystemList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			std::get<T>(systems) = T();
			smpl::call_if_exists<T, smpl::has_construct<T>>(&std::get<T>(systems));
		});
	for_each<RenderingSystem2DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			std::get<T>(renderSystems2D) = T();
				smpl::call_if_exists<T, smpl::has_construct<T>>(&std::get<T>(renderSystems2D));
		});
	for_each<RenderingSystem3DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			std::get<T>(renderSystems3D) = T();
			smpl::call_if_exists<T, smpl::has_construct<T>>(&std::get<T>(renderSystems3D));
		});
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
uint32 Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::createEntity(){
	entities.push_back(Entity<ECSDesc>());
	return entities.size() - 1;
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::removeEntity(uint32 entity){
	// TODO: Remove entity
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::registerEntity(uint32 entity){
	// Register entity into non-rendering systems
	for_each<typename TSystemList::type>::execute([this, entity](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			T& system = std::get<T>(systems);
			if((system.bitset & entities[entity].bitset) == system.bitset){
				system.entities.push_back(entity);
			}
		});
	// Register entity into Rendering Systems 2D
	for_each<typename TRenderingSystem2DList::type>::execute([this, entity](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			T& system = std::get<T>(renderSystems2D);
			if((system.bitset & entities[entity].bitset) == system.bitset){
				// Calculate entity hint value
				uint16 modelID = 0;
				uint16 materialID = 0;
				uint16 pipelineID = 0;
				if(hasComponent<Model2D>(entity))
					modelID = 0; // Add support for model2D IDs
					//modelID = getComponent<Model2D>(entity)->ID;
				if(hasComponent<Material>(entity))
					materialID = getComponent<Material>(entity)->ID;
				if(hasComponent<ShaderPipeline>(entity))
					pipelineID = getComponent<ShaderPipeline>(entity)->ID;
				uint16 flags = 0;
				
				uint64 entityHint = ((uint64)flags << 48) + ((uint64)pipelineID << 32) + ((uint64)materialID << 16) + (uint64)modelID;
				system.entities.insert(std::pair<uint64, uint32>(entityHint, entity));
			}
		});
	// Register entity into Rendering Systems 3D
	for_each<typename TRenderingSystem3DList::type>::execute([this, entity](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			T& system = std::get<T>(renderSystems3D);
			std::cout << "Checking entity against bitset" << std::endl;
			//std::cout << std::hex << system.bitset << std::endl;
			//std::cout << std::hex << entities[entity].bitset << std::endl;
			std::cout << ECSDesc::template getComponentID<Model3D>() << std::endl;
			std::cout << ECSDesc::template getComponentID<ShaderPipeline>() << std::endl;
			std::cout << ECSDesc::template getComponentID<Material>() << std::endl;
			std::cout << ECSDesc::template getGroupID<ForwardRender>() << std::endl;
			if((system.bitset & entities[entity].bitset) == system.bitset){
				// Calculate entity hint value
				uint16 modelID = 0;
				uint16 materialID = 0;
				uint16 pipelineID = 0;
				if(hasComponent<Model3D>(entity))
					modelID = getComponent<Model3D>(entity)->ID;
				if(hasComponent<Material>(entity))
					materialID = getComponent<Material>(entity)->ID;
				if(hasComponent<ShaderPipeline>(entity))
					pipelineID = getComponent<ShaderPipeline>(entity)->ID;
				uint16 flags = 0;
				
				uint64 entityHint = ((uint64)flags << 48) + ((uint64)pipelineID << 32) + ((uint64)materialID << 16) + (uint64)modelID;
				system.entities.insert(std::pair<uint64, uint32>(entityHint, entity));
			}
		});
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Component>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::addComponent(uint32 entity, Component* component){
	// Set entity as non-valid
	entities[entity].isValid = false;
	
	// Update bitset of entity
	entities[entity].bitset[ECSDesc::template getComponentID<Component>()] = true;
	// Add component into component storage
	std::vector<Component*>& componentStorage = std::get<std::vector<Component*>>(components);
	uint32 location = componentStorage.size();
	
	// Push new Component at the end of Component array
	componentStorage.push_back(component);
	
	// Map the new component to location returned from ECS
	entities[entity].map.insert(std::pair<uint32, void*>(ECSDesc::template getComponentID<Component>(), (void*)component));
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::addLightComponent(uint32 entity, LightComponent* lc){
	// Fill in the light component struct with static lights if the object is static
	if(hasComponent<Transform3D>(entity)){
		// TODO
	}

	// Add the component like any other
	addComponent<LightComponent>(entity, lc);
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Component>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::removeComponent(uint32 entity){
	// TODO: Remove component
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Component>
Component* Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::getComponent(uint32 entity){
	typename ComponentMap::iterator it = entities[entity].map.find(ECSDesc::template getComponentID<Component>());
		return (Component*)it->second;
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Component>
bool Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::hasComponent(uint32 entity){
	auto it = entities[entity].map.find(ECSDesc::template getComponentID<Component>());
		return it != entities[entity].map.end();
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Group>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::addToGroup(uint32 entity, bool value){
	entities[entity].bitset[ECSDesc::template getGroupID<Group>()] = value;
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Flag>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::setFlag(uint32 entity, bool value){
	entities[entity].flags[ECSDesc::template getFlagID<Flag>()] = value;
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::input(){
	renderer->input();
}
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::update(){
	// Register all entities that have changed (or been added) in the last frame
	for(int i = 0; i < entities.size(); ++i){
		if(!entities[i].isValid){
			registerEntity(i);
			entities[i].isValid = true;
		}
	}
	
	// Camera update
	renderer->update();

	// System list update
  for_each<SystemList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, UPDATE_TYPE, typename T::Signature::type> caller;
			// Loop over entities the system has to manage
			T& system = std::get<T>(systems);
			for(int i = 0; i < system.entities.size(); ++i){
				// Check if required flags match
				//uint32 entityID = systems.entities[i];
				//current_entity = entityID;
				if((system.flags & this->entities[system.entities[i]].flags) == system.flags)
					caller.template call(this, system.entities[i], T::update);
			}
		});
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::render() {
	// Update rendering systems 2D
	for_each<RenderingSystem2DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, RENDER2D_TYPE, typename T::Signature::type> caller;
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems2D);
			std::vector<uint32> entities;
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it){
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}
			caller.template call(this, entities, T::update);
		});
	// Update rendering systems 3D
  for_each<RenderingSystem3DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, RENDER3D_TYPE, typename T::Signature::type> caller;
			
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems3D);
			std::vector<uint32> entities;
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it){
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}
			caller.template call(this, entities, T::update);
		});
}

#endif

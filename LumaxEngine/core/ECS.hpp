
#ifndef ECS_HPP
#define ECS_HPP

#include <iostream>
#include <tuple>
#include <vector>
#include <type_traits>
#include <bitset>
#include <unordered_map>

#include "../utils/SMPL.hpp"
#include "../math/types.hpp"

template <typename... Ts> using ComponentList = typename smpl::type_list<Ts...>;
template <typename... Ts> using SystemList = typename smpl::type_list<Ts...>;

// Forward declarations
template <typename Manager>
class Entity;

//
// ---------------------------------------- 
//

// System instantiation at run-time
template <typename Storage, typename T>
struct instantiate_systems;
template <typename Storage, typename T, typename U>
struct instantiate_systems<Storage, smpl::type_list_impl<T, U>> {
	static constexpr void execute(Storage systems) noexcept {
		std::get<T>(systems) = T();
		instantiate_systems<Storage, U>::execute(systems);
	}
};
template <typename Storage>
struct instantiate_systems<Storage, smpl::type_list_null> {
	static constexpr void execute(Storage systems) noexcept {}
};

// System updating at run-time
// System caller for expanding variadic typenames (signature)
template <typename Manager, typename T>
struct SystemCaller;
template <typename Manager, typename T, typename U>
struct SystemCaller<Manager, smpl::type_list_impl<T, U>> {
	template <typename Function, typename... Ts>
	static constexpr void call(uint32 entity, Function&& function){
		SystemCaller<Manager, U>::template call<Function, Ts..., T>(entity, function);
	}
};
template <typename Manager>
struct SystemCaller<Manager, smpl::type_list_null>{
	template <typename Function, typename... Ts>
	static constexpr void call(uint32 entity, Function&& function){
		function(Manager::template getComponent<Ts>(entity)...);
	}
};
// Recursively update the systems
template <typename Manager, typename Storage, typename T>
struct update_systems;
template <typename Manager, typename Storage, typename T, typename U>
struct update_systems<Manager, Storage, smpl::type_list_impl<T, U>> {
	static void execute(Storage systems) noexcept {
		SystemCaller<Manager, typename T::Signature::type> caller;
		// Loop over entities the system has to manage
		T& system = std::get<T>(systems);
		for(int i = 0; i < system.entities.size(); ++i){
			caller.template call(system.entities[i], T::update);
		}
		update_systems<Manager, Storage, U>::execute(systems);
	}
};
template <typename Manager, typename Storage>
struct update_systems<Manager, Storage, smpl::type_list_null>{
	static void execute(Storage systems) noexcept {}
};

// Adding entity to systems at run-time
template <typename Manager, typename Storage, typename T>
struct register_entity_systems;
template <typename Manager, typename Storage, typename T, typename U>
struct register_entity_systems<Manager, Storage, smpl::type_list_impl<T, U>> {
	static constexpr void execute(Storage& systems, uint32 entity) noexcept {
		T& system = std::get<T>(systems);
		if((system.bitset & Manager::entities[entity].bitset) == system.bitset){
			system.entities.push_back(entity);
		}
		register_entity_systems<Manager, Storage, U>::execute(systems, entity);
	}
};
template <typename Manager, typename Storage>
struct register_entity_systems<Manager, Storage, smpl::type_list_null>{
	static constexpr void execute(Storage& systems, uint32 entity) noexcept {}
};

//
// ---------------------------------------- 
//

//
// ----- ECS Description
//

template <typename TComponentList>
class ECS {
public:
	using ComponentList = typename TComponentList::type;

	// Components static compile-time help methods
	static constexpr size_t getComponentCount() noexcept {
		return smpl::getSize<ComponentList>();
	}
	template <typename T>
	static constexpr size_t getComponentID() noexcept {
		return smpl::getIndexOf<ComponentList, T>();
	}

	// Define a Bitset type of enough size for every component
	using Bitset = std::bitset<getComponentCount()>;
	// Define a Component hashmap          Comp ID  Comp location
	using ComponentMap = std::unordered_map<uint32, uint32>;
};

//
// ----- ECS Manager
//

template <typename ECS, typename TSystemList>
class ECSManager {
public:
	using SystemList = typename TSystemList::type;
	using ThisType = ECSManager<ECS, TSystemList>;
	
	// Entity management methods
	uint32 createEntity();
	//TODO: Remove entity
	
	using ComponentMap = typename ECS::ComponentMap;
	// Component get and set
	template <typename T>
	static T& getComponent(uint32 entity) noexcept {
		typename ComponentMap::iterator it = entities[entity].map.find(ECS::template getComponentID<T>());
		return std::get<std::vector<T>>(components)[it->second];
	}
	template <typename T>
	static void setComponent(uint32 entity, uint32 location) noexcept {
		entities[entity].map.insert(std::pair<uint32, uint32>(ECS::template getComponentID<T>(), location));
	}
	
	// Component management methods
	template <typename Component>
	Component& addComponent(uint32 entity){
		// Update bitset of entity
		entities[entity].bitset[ECS::template getComponentID<Component>()] = true;
		
		// Get memory location for new Component
		std::vector<Component>& componentStorage = std::get<std::vector<Component>>(components);
		uint32 location = componentStorage.size();
		
		// Construct new Component at the end of Component array
		componentStorage.push_back(Component());
		
		// Update hashmap of components to reference new component
		setComponent<Component>(entity, location);
		
		// return reference to new Component
		return componentStorage[location];
	}
	
	// System management methods
	ECSManager(){
		// At construction time, instantiate all systems
		instantiate_systems<SystemTupleStorage, SystemList>::execute(systems);
	}

	void update(){
		update_systems<ThisType, SystemTupleStorage, SystemList>::execute(systems);
	}

	void registerEntity(uint32 entity){
		register_entity_systems<ThisType, SystemTupleStorage, SystemList>::execute(systems, entity);
	}
	
	// Entities storage
	static std::vector<Entity<ECS>> entities;

private:
	// Component Storage
	template <typename T, typename... Ts>
	struct create_componentlist_tuple;
	template <typename T, typename U, typename... Ts>
	struct create_componentlist_tuple<smpl::type_list_impl<T, U>, Ts...> {
		using type = typename create_componentlist_tuple<U, Ts..., std::vector<T>>::type;
	};
	template <typename... Ts>
	struct create_componentlist_tuple<smpl::type_list_null, Ts...> {
		using type = std::tuple<Ts...>;
	};
	
	using ComponentTupleVectors = typename create_componentlist_tuple<typename ECS::ComponentList>::type;
	static ComponentTupleVectors components;
	
	// Systems storage
	template <typename T, typename... Ts>
	struct create_systemlist_tuple;
	template <typename T, typename U, typename... Ts>
	struct create_systemlist_tuple<smpl::type_list_impl<T, U>, Ts...> {
		using type = typename create_systemlist_tuple<U, Ts..., T>::type;
	};
	template <typename... Ts>
	struct create_systemlist_tuple<smpl::type_list_null, Ts...> {
		using type = std::tuple<Ts...>;
	};

	using SystemTupleStorage = typename create_systemlist_tuple<SystemList>::type;
	SystemTupleStorage systems;
};

template <typename ECS, typename U>
std::vector<Entity<ECS>> ECSManager<ECS, U>::entities;

template <typename ECS, typename U>
typename ECSManager<ECS, U>::ComponentTupleVectors ECSManager<ECS, U>::components;

//
// ----- Entity
//

template <typename ECS>
class Entity {
public:
	// Bitset signature
	using Bitset = typename ECS::Bitset;
	Bitset bitset;
	
	// Component hashmap
	using ComponentMap = typename ECS::ComponentMap;
	ComponentMap map;
};

//
// ----- System Recursive Init of bitset from required components
//

template <typename ECS, typename B, typename T>
struct init_bitset;
template <typename ECS, typename B, typename T, typename U>
struct init_bitset<ECS, B, smpl::type_list_impl<T, U>> {
	static constexpr void execute(B& bitset) noexcept {
		// Get Component T ID and update bitset
		bitset.set(ECS::template getComponentID<T>());
		
		// Recursively update bitset with all components
		init_bitset<ECS, B, U>::execute(bitset);
	}
};
template <typename ECS, typename B>
struct init_bitset<ECS, B, smpl::type_list_null> {
	static constexpr void execute(B& bitset) noexcept {}
};

//
// ----- System
//

template <typename ECS, typename... Ts>
class System {
public:
	using Signature = smpl::type_list<Ts...>;
	
	System(){
		// Initialize bitset signature
		init_bitset<ECS, Bitset, typename Signature::type>::execute(bitset);
	}
	// Signature for required components to operate on entity
	using Bitset = typename ECS::Bitset;
	Bitset bitset;
	
	// Entities to keep track of based on signature
	std::vector<uint32> entities;
};

//
// ----- ECSManager method implementations
//

template <typename ECS, typename U>
uint32 ECSManager<ECS, U>::createEntity() {
	entities.push_back(Entity<ECS>());
	return entities.size() - 1;
}

#endif

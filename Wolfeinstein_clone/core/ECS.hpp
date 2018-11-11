
#ifndef ECS_HPP
#define ECS_HPP

#include <iostream>
#include <tuple>
#include <vector>
#include <type_traits>
#include <bitset>
#include <unordered_map>
#include <stdarg.h>

#include "../utils/SMPL.hpp"
#include "../math/types.hpp"

#include "RenderComponentStorage.hpp"

//#include "../RL/ShaderPipeline.hpp"
//#include "../RL/Material.hpp"
//#include "../RL/Model2D.hpp"
//#include "../RL/Model3D.hpp"
//#include "../RL/Renderer.hpp"

#include "BSPTree.hpp"

// Groups
struct DeferredRender {};
struct ForwardRender {};

template <typename... Ts> using ComponentList = typename smpl::type_list<Ts...>;
template <typename... Ts> using GroupList = typename smpl::type_list<Ts...>;
template <typename... Ts> using FlagList = typename smpl::type_list<Ts...>;
template <typename... Ts> using SystemList = typename smpl::type_list<Ts...>;

// Built-in flags
struct FlagCollided {};
struct FlagNotCollided {};

using BuiltInFlags = smpl::type_list<FlagCollided, FlagNotCollided>;

// Forward declarations
template <typename Manager>
class Entity;

//
// ---------------------------------------- 
//

// For each type list meta-programming help method
template <typename T>
struct for_each;
template <typename T, typename U>
struct for_each<smpl::type_list_impl<T, U>>{
	template <typename Action>
	static constexpr void execute(Action&& action){
		// Call lambda
		action((T*)nullptr);
		for_each<U>::execute(action);
	}
};
template <>
struct for_each<smpl::type_list_null>{
	template <typename Action>
	static constexpr void execute(Action&& action) { /* End of recursion */ }
};

// System updating at run-time
// Expand call

typedef float  RENDER3D_TYPE;
typedef long   RENDER2D_TYPE;
typedef int    UPDATE_TYPE;

template <typename Caller, typename LevelType, typename... Ts>
struct ExpandCall{
	template <typename Function>
	static void call(LevelType* level, uint32 entity, Function&& function){
		function(level->template getComponent<Ts>(entity)...);
	}
};
template <typename LevelType, typename T>
struct CreateVectorForRender{
	static std::vector<T*> getContainer(LevelType* level, const std::vector<uint32>& entities){
		std::vector<T*> container;
		for(int i = 0; i < entities.size(); ++i)
			container.push_back(level->template getComponent<T>(entities[i]));
		return container;
	}
};
template <typename LevelType, typename... Ts>
struct ExpandCall<RENDER3D_TYPE, LevelType, Ts...>{
	template <typename Function>
	static void call(LevelType* level, uint32 entity, Function& function){
		function(level->template getComponent<Ts>(entity)..., level->getCamera());
	}
	template <typename Function>
	static void call(LevelType* level, const std::vector<uint32>& entities, Function& function){
		function(CreateVectorForRender<LevelType, Ts>::getContainer(level, entities)..., level->getCamera());
	}
};
// System caller for expanding variadic typenames (signature)
template <typename LevelType, typename ECS, typename Caller, typename T>
struct SystemCaller;
template <typename LevelType, typename ECS, typename Caller, typename T, typename U, int index>
struct SystemCallerImpl{
	template <typename Function, typename... Ts>
	static void call(LevelType* level, uint32 entity, Function&& function){
		SystemCaller<LevelType, ECS, Caller, U>::template call<Function, Ts..., T>(level, entity, function);
	}
	template <typename Function, typename... Ts>
	static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function){
		SystemCaller<LevelType, ECS, Caller, U>::template call<Function, Ts..., T>(level, entities, function);
	}
};
template <typename LevelType, typename ECS, typename Caller, typename T, typename U>
struct SystemCallerImpl<LevelType, ECS, Caller, T, U, -1>{
	template <typename Function, typename... Ts>
		static void call(LevelType* level, uint32 entity, Function&& function){
		SystemCaller<LevelType, ECS, Caller, U>::template call<Function, Ts...>(level, entity, function);
	}
	template <typename Function, typename... Ts>
		static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function){
		SystemCaller<LevelType, ECS, Caller, U>::template call<Function, Ts...>(level, entities, function);
	}
};
template <typename LevelType, typename ECS, typename Caller, typename T, int index>
struct SystemCallerImpl<LevelType, ECS, Caller, T, smpl::type_list_null, index>{
	template <typename Function, typename... Ts>
	static void call(LevelType* level, uint32 entity, Function&& function){ 
		ExpandCall<Caller, LevelType, Ts..., T>::template call(level, entity, function);
	}
	template <typename Function, typename... Ts>
	static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function){
		ExpandCall<Caller, LevelType, Ts..., T>::template call(level, entities, function);
	}
};
template <typename LevelType, typename ECS, typename Caller, typename T>
struct SystemCallerImpl<LevelType, ECS, Caller, T, smpl::type_list_null, -1>{
	template <typename Function, typename... Ts>
		static void call(LevelType* level, uint32 entity, Function&& function){
		ExpandCall<Caller, LevelType, Ts...>::template call(level, entity, function);
	}
	template <typename Function, typename... Ts>
		static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function){
		ExpandCall<Caller, LevelType, Ts...>::template call(level, entities, function);
	}
};
template <typename LevelType, typename ECS, typename Caller, typename T, typename U>
struct SystemCaller <LevelType, ECS, Caller, smpl::type_list_impl<T, U>>{
	template <typename Function, typename... Ts>
		static void call(LevelType* level, uint32 entity, Function&& function){
		SystemCallerImpl<LevelType, ECS, Caller, T, U, ECS::template getComponentID<T>()>::template call<Function, Ts...>(level, entity, function);
	}
	template <typename Function, typename... Ts>
		static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function){
		SystemCallerImpl<LevelType, ECS, Caller, T, U, ECS::template getComponentID<T>()>::template call<Function, Ts...>(level, entities, function);
	}
};

//template <Iterator, Caller, Function>
//static void dispatcher(Iterator begin, Iterator end, Caller caller, Function&& function){
//	for(Iterator it = begin; it != end; ++it){
//		caller.template call(it->second, function);
//	}
//}

//
// ----- ECS Description
//

template <typename TComponentList, typename TGroupList, typename TFlagList>
class ECS {
public:
	using ComponentList = typename TComponentList::type;
	using GroupList = typename TGroupList::type;
	using FlagList = typename smpl::concat<typename TFlagList::type, typename BuiltInFlags::type>::type::type;

	// Components static compile-time helper methods
	static constexpr size_t getComponentCount() noexcept {
		return smpl::getSize<ComponentList>();
	}
	template <typename T>
	static constexpr int getComponentID() noexcept {
		return smpl::getIndexOf<ComponentList, T>();
		//return 0;
	}

	// Groups static compile-time helper methods
	static constexpr size_t getGroupCount() noexcept {
		return smpl::getSize<GroupList>();
	}
	template <typename T>
	static constexpr int getGroupID() noexcept {
		if(smpl::getIndexOf<GroupList, T>() == -1)
			return -1;
		return getComponentCount() + smpl::getIndexOf<GroupList, T>();
	}

	// Flags static compile-time helper methods
	static constexpr size_t getFlagCount() noexcept {
		return smpl::getSize<FlagList>();
	}
	template <typename T>
	static constexpr int getFlagID() noexcept {
		return smpl::getIndexOf<FlagList, T>();
	}
	
	// Define a Bitset type of enough size for every component and every flag
	using Bitset = std::bitset<getComponentCount() + getGroupCount()>;
	using FlagBitset = std::bitset<getFlagCount()>;
	// Define a Component hashmap          Comp ID  Comp pointer
	using ComponentMap = std::unordered_map<uint32, void*>;
};

//
// ----- ECS Manager
//

template <typename ECS, typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
class ECSManager {
public:
	
	using SystemList = typename TSystemList::type;
	using RenderingSystem2DList = typename TRenderingSystem2DList::type;
	using RenderingSystem3DList = typename TRenderingSystem3DList::type;
	using ThisType = ECSManager<ECS, TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>;
	
	// System storage declaration
	using SystemTupleStorage = typename smpl::create_tuple_from_list<SystemList>::type;
	using RenderSystem2DTupleStorage = typename smpl::create_tuple_from_list<RenderingSystem2DList>::type;
	using RenderSystem3DTupleStorage = typename smpl::create_tuple_from_list<RenderingSystem3DList>::type;

	/*
	template <typename Manager, typename Component>
	struct AddComponent{
		static constexpr void execute(Manager& manager, uint32 entity, Component* component){
			manager.template addComponent<Component>(entity, component);
		}
	};
	template <typename Manager, typename T, typename... Ts>
	struct ComponentAdder{
		static constexpr void execute(Manager& manager, uint32 entity, T component, Ts... rest){
			AddComponent<Manager, typename std::remove_pointer<T>::type>::execute(manager, entity, component);
			ComponentAdder<Manager, Ts...>::execute(manager, entity, rest...);
		}
	};
	template <typename Manager, typename T>
	struct ComponentAdder<Manager, T>{
		static constexpr void execute(Manager& manager, uint32 entity, T component){
			AddComponent<Manager, typename std::remove_pointer<T>::type>::execute(manager, entity, component);
		}
	};
	// Specialization of components
	template <typename Manager>
	struct AddComponent<Manager, Transform3D>{
		static constexpr void execute(Manager& manager, uint32 entity, Transform3D* transform){
			manager.template staticEntities.push_back(entity);
			manager.template addComponent<Transform3D>(entity, transform);
		}
	};
	template <typename Manager>
	struct AddComponent<Manager, DynamicTransform3D>{
		static constexpr void execute(Manager& manager, uint32 entity, DynamicTransform3D* transform){
			manager.template dynamicEntities.push_back(entity);
			manager.template addComponent<DynamicTransform3D>(entity, transform);
		}
	};
	template <typename Manager>
	struct AddComponent<Manager, PivotTransform3D>{
		static constexpr void execute(Manager& manager, uint32 entity, PivotTransform3D* transform){
			manager.template dynamicEntities.push_back(entity);
			manager.template addComponent<PivotTransform3D>(entity, transform);
		}
	};
	template <typename Manager>
	struct AddComponent<Manager, PhysicsPrimitive>{
		static constexpr void execute(Manager& manager, uint32 entity, PhysicsPrimitive* physics){
			manager.template physicsEntities.push_back(entity);
			manager.template addComponent<PhysicsPrimitive>(entity, physics);
		}
	};
	template <typename Manager>
	struct AddComponent<Manager, MaterialPipeline>{
		static constexpr void execute(Manager& manager, uint32 entity, MaterialPipeline* material){
			// TODO: Allow sorting based on material pipeline
			manager.template addComponent<MaterialPipeline>(entity, material);
		}
	};
	// For polygon soup, adding of individual polygons:
	template <typename Manager>
	struct AddComponent<Manager, GeometryDataComponent>{
		static constexpr void execute(Manager& manager, uint32 entity, GeometryDataComponent* component){
			// Add Geometry to BSP tree
			std::cout << "adding polygons to scene" << std::endl;
			for(int i = 0; i < component->polygons.size(); ++i){
				manager.template scenePolygons.push_back(component->polygons[i]);
			}
		}
	};
	*/
	
	template <typename Manager, typename G, typename... groups>
	struct GroupAdder{
		static constexpr void execute(Manager& manager, uint32 entity){
			manager.template addToGroup<G>(entity);
			GroupAdder<Manager, groups...>::execute(manager, entity);
		}
	};
	template <typename Manager, typename G>
	struct GroupAdder<Manager, G>{
		static constexpr void execute(Manager& manager, uint32 entity){
			manager.template addToGroup<G>(entity); // End of recursion
		}
	};
	
	template <typename... Ts>
	constexpr uint32 makeEntity(Ts... components){
		//uint32 entity = createEntity();

		// Add components
		//ComponentAdder<ThisType, Ts...>::execute(*this, entity, components...);

		// Register entity to systems
		//registerEntity(entity);
		
		//return entity;
		return 1;
	}
	
	// Render component specialization
	//template <typename Signature>
	//void addRenderComponent2D(uint32 entity, ShaderPipeline* pipeline, Material* material, Model2D* model, const std::string& name);
	//template <typename Signature>
	//void addRenderComponent3D(uint32 entity, ShaderPipeline* pipeline, Material* material, Model3D* model, const std::string& name);
	
	// System management methods
	ECSManager(){ }
	
	/*
	void renderBSP(BSPNode* node, Vec3 cameraPos){
		int index = BSPTree::testPointWithPlane(cameraPos, node->plane);
		
		// Visit side camera is NOT on
		if(index == IN_FRONT_OF_PLANE && node->back)
			renderBSP(node->back, cameraPos);
		if(index == BEHIND_PLANE && node->front)
			renderBSP(node->front, cameraPos);
		
		// Draw polygons if node is leaf
		if(node->hasGeometry){
			// Render geometry
			node->render();
		}

		// Visit other side camera is ON
		if(index == IN_FRONT_OF_PLANE && node->front)
			renderBSP(node->front, cameraPos);
		if(index == BEHIND_PLANE && node->back)
			renderBSP(node->back, cameraPos);
		
	}
	*/
	
	template <typename T, typename... Ts>
	struct create_componentlist_tuple;
	template <typename T, typename U, typename... Ts>
	struct create_componentlist_tuple<smpl::type_list_impl<T, U>, Ts...> {
		using type = typename create_componentlist_tuple<U, Ts..., std::vector<T*>>::type;
	};
	template <typename... Ts>
	struct create_componentlist_tuple<smpl::type_list_null, Ts...> {
		using type = std::tuple<Ts...>;
	};
	
	using ComponentTupleVectors = typename create_componentlist_tuple<typename ECS::ComponentList>::type;
};

//
// ----- Entity
//

template <typename ECS>
class Entity {
public:
	// Bitset signature
	using Bitset = typename ECS::Bitset;
	Bitset bitset;
	using FlagBitset = typename ECS::FlagBitset;
	FlagBitset flags;
	
	// Component hashmap
	using ComponentMap = typename ECS::ComponentMap;
	ComponentMap map;

	// Valid flag
	bool isValid = true;
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
		init_bitsets<ECS, Bitset, FlagBitset, typename Signature::type>::execute(bitset, flags);
	}
	// Signature for required components to operate on entity
	using Bitset = typename ECS::Bitset;
	Bitset bitset;
	using FlagBitset = typename ECS::FlagBitset;
	FlagBitset flags;
	
	// Entities to keep track of based on signature
	std::vector<uint32> entities;
};

//
// ----- Rendering System
//

template <typename ECS, typename... Ts>
class RenderingSystem{
public:
	using Signature = smpl::type_list<Ts...>;
	
	RenderingSystem(){
		// Initialize bitset signature
		std::cout << "Init bitsets rendering system" << std::endl;
		init_bitsets<ECS, Bitset, FlagBitset, typename Signature::type>::execute(bitset, flags);
	}
	// Signature for required components to operate on vector of entities
	using Bitset = typename ECS::Bitset;
	Bitset bitset;
	using FlagBitset = typename ECS::FlagBitset;
	FlagBitset flags;
	
	// Maps a key to a value
	// Key = uint32 sortable bucket
	// | 2D-3D | Translucency | ShaderPipeline | Material | Model |   ->   maps to a vector of entities (later possibly a single drawcall of geometry)
	std::multimap<uint64, uint32> entities;
};

//#include "ECS.tpp"

#endif

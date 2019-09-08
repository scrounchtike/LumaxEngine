
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

#include "ECS_Declarations.hpp"

//#include "BSPTree.hpp"

// Forward declarations
template <typename Manager>
class Entity;

//
// ---------------------------------------- 
//

template <typename ECS, typename Signature>
struct init_pipeline_bitset {
	static void execute(PipelineGroup2D<ECS>* group) noexcept {
		init_bitset<ECS, typename ECS::Bitset, typename Signature::type>::execute(group->signature);
	}
	static void execute(PipelineGroup3D<ECS>* group) noexcept {
		init_bitset<ECS, typename ECS::Bitset, typename Signature::type>::execute(group->signature);
	}
};

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

// Check if update function exists at compile time
template<typename T>
struct has_update
{
private:
	template<typename V> static void impl(decltype(typename V::update(), int()));
    template<typename V> static bool impl(char);
public:
    static const bool value = std::is_same<void, decltype(impl<T>(0))>::value;
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
		static void call(LevelType* level, uint32 entity, Function&& function)
	{
		SystemCallerImpl<LevelType, ECS, Caller, T, U, ECS::template getComponentID<T>()>::template call<Function, Ts...>(level, entity, function);
	}
	template <typename Function, typename... Ts>
		static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function)
	{
		SystemCallerImpl<LevelType, ECS, Caller, T, U, ECS::template getComponentID<T>()>::template call<Function, Ts...>(level, entities, function);
	}
};
template <typename LevelType, typename ECS, typename Caller>
struct SystemCaller <LevelType, ECS, Caller, smpl::type_list_null>
{
	template <typename Function>
	static void call(LevelType* level, uint32 entity, Function&& function)
	{
			ExpandCall<Caller, LevelType>::template call(level, entity, function);
	}
	template <typename Function>
	static void call(LevelType* level, const std::vector<uint32>& entities, Function&& function)
	{
			ExpandCall<Caller, LevelType>::template call(level, entities, function);
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

template <typename ECS, typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
class ECSManager {
public:
	
	using SystemList = typename TSystemList::type;
	using RenderingSystem2DList = typename TRenderingSystem2DList::type;
	using RenderingSystem3DList = typename TRenderingSystem3DList::type;
	using RenderPass2DList = typename TRenderPass2DList::type;
	using RenderPass3DList = typename TRenderPass3DList::type;
	
	using ThisType = ECSManager<ECS, TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>;
	
	// System storage declaration
	using SystemTupleStorage = typename smpl::create_tuple_from_list<SystemList>::type;
	using RenderSystem2DTupleStorage = typename smpl::create_tuple_from_list<RenderingSystem2DList>::type;
	using RenderSystem3DTupleStorage = typename smpl::create_tuple_from_list<RenderingSystem3DList>::type;

	using System2DBitset = std::bitset<smpl::getSize<RenderingSystem2DList>()>;
  using System3DBitset = std::bitset<smpl::getSize<RenderingSystem3DList>()>;
	
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
// ---- Render pass
//
// The targets are the systems for which we DO or DONT want to render
// depending on the bool render_systems (0 = omit only, 1 = render only)
//
// Render passes can also be Pre-render or Post-render passes
// depending on the value of level
//

// Forward declarations
//struct RenderingSystems2D;
//struct RenderingSystems3D;

namespace RenderPass_
{
struct PRE_RENDER_PASS;
struct POST_RENDER_PASS;
struct RENDER_ONLY;
struct OMIT_ONLY;

template <typename Render3DList, typename Signature, typename F>
struct ListCreator;
template <typename Render3DList, typename Signature>
struct ListCreator<Render3DList, Signature, RENDER_ONLY>
{
	template <typename T>
	struct Contains
	{
		static constexpr bool value = !(smpl::contains<typename Signature::type, T>());
	};
	using System3DListToRender = Signature;
	using System3DListToOmit = typename smpl::type_list_filter<Render3DList, Contains>::result;
};
template <typename Render3DList, typename Signature>
struct ListCreator<Render3DList, Signature, OMIT_ONLY>
{
	template <typename T>
	struct Contains
	{
		static constexpr bool value = !(smpl::contains<typename Signature::type, T>());
	};
	using System3DListToRender = typename smpl::type_list_filter<Render3DList, Contains>::result;
	using System3DListToOmit = Signature;
};
}

template <typename ECS, typename TComponentList, typename Function, typename... Targets> 
class RenderPass {
public:

	using ComponentList = typename TComponentList::type;
	
	struct PRE_RENDER_PASS{};
	struct POST_RENDER_PASS{};
	struct RENDER_ONLY{};
	struct OMIT_ONLY{};
	
	template <typename RS3D, typename S, typename F>
	using ListCreator = RenderPass_::ListCreator<RS3D, S, F>;
	
	using Signature = smpl::type_list<Targets...>;
	
	// Constructor
	RenderPass()
	{
	}
	
	using System3DListToRender = typename ListCreator<RenderingSystems3D, Signature, Function>::System3DListToRender;
	using System3DListToOmit = typename ListCreator<RenderingSystems3D, Signature, Function>::System3DListToOmit;

	static void renderScene();
};

//
// ----- System
//

template <typename ECS, typename... Ts>
class System {
public:
	using Signature = smpl::type_list<Ts...>;
	
	System()
	{
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
	
	void addEntity(uint64 hint, uint32 entity)
	{	
		// Add the entity to the system list (performs sorting at the same time)
		entities.insert(std::pair<uint64, uint32>(hint, entity));
	}

	// Useful for performing processing on the entities (ex. sorting) before rendering
	// Define this function in a render system and polymorphism will call the derived
	// one rather than this one !
	template <typename Level>
	void updateEntityVector(const Level& level, std::vector<uint32>& entities, const Camera* camera)
	{
	}
	
	// Maps a key to a value
	// Key = uint32 sortable bucket
	// | 2D-3D | Translucency | ShaderPipeline | Material | Model |   ->   maps to a vector of entities (later possibly a single drawcall of geometry)
	std::multimap<uint64, uint32> entities;
};

//#include "ECS.tpp"

#endif

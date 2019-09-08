
#ifndef ECS_DECLARATIONS_HPP
#define ECS_DECLARATIONS_HPP

#include "reactphysics3d.h"

// Include Camera and Player defintions
#include "../GL/Player.hpp"
class Renderer;
class Player;

// Groups
struct DeferredRender {};
struct ForwardRender {};

// Built-in flags
struct FlagCollided {};
struct FlagNotCollided {};

using BuiltInFlags = smpl::type_list<FlagCollided, FlagNotCollided>;

template <typename... Ts> using ComponentList = typename smpl::type_list<Ts...>;
template <typename... Ts> using GroupList = typename smpl::type_list<Ts...>;
template <typename... Ts> using FlagList = typename smpl::type_list<Ts...>;
template <typename... Ts> using SystemList = typename smpl::type_list<Ts...>;
template <typename... Ts> using RenderPassList = typename smpl::type_list<Ts...>;

//
// Static ECS Description
// Meaning, Level(s) will always use the same components, same groups, same flags
// 

// ------------------------------------------------ //
//
//  Components
//
//  * List of all components needed by the Level
//
// - ShaderPipeline
// - Material
// - Model2D and Model3D
// - Transform2D, DynamicTransform2D and PivotTransform2D
// - Transform3D, DynamicTransform3D and PivotTransform3D
// - Movement2D and Movement3D
// - PhysicsPrimitive
// - GeometryDataComponent = ???
// - MaterialPipeline = ???
// - LightComponent
// - PrintComponent
// - SimpleLightComponent
//
// ------------------------------------------------ //

class ShaderPipeline;
class Material;
class Model2D;
class Model3D;
class Transform2D;
class DynamicTransform2D;
class PivotTransform2D;
//class Transform3D;
class DynamicTransform3D;
class PivotTransform3D;
class PhysicsPrimitive;
class MaterialPipeline;
struct PrintComponent;
struct LightComponent;
struct SimpleLightComponent;
struct Movement2D;
struct Movement3D;
class Terrain;
class TerrainClipmap;

#include "../RL/lights.hpp"

using Components = ComponentList<Model2D, Transform2D, DynamicTransform2D, PivotTransform2D, Model3D, Transform3D, PivotTransform3D, Movement2D, Movement3D, PhysicsPrimitive, Material, ShaderPipeline, MaterialPipeline, PrintComponent, LightComponent, SimpleLightComponent, rp3d::RigidBody, Terrain, TerrainClipmap>;
using Groups = GroupList<ForwardRender, DeferredRender>;
using Flags = FlagList<>;
template <typename TComponentList, typename TGroupList, typename TFlagList>
class ECS;
using ECSDesc = ECS<Components, Groups, Flags>;

// ------------------------------------------------ //
//
//  Systems, Render Systems and Render Passes
//
//  * List of all systems needed by the Level
//
// - MovementSystem2D
// - MovementSystem3D
// - MovementSystemPhysics3D
// - MovementSystemPhysics3D
//
//  * List of all render systems needed by the Level
//
// - RenderSystemForwardTransform3D
// - RenderSystemForwardLight3D
// - RenderSystemForwardWater3D
// - RenderSystemForwardTerrainTransform3D
// - RenderSystemForwardTerrainLight3D
//
//  * List of all render passes needed by the Level
//
// - RenderPassWater3D
//
// ------------------------------------------------ //

// Systems descriptions for forward rendering is inside ForwardRenderer.hpp
//#include "../RL/ForwardRenderer.hpp"
// Forward declarations for systems
template <typename ECS>
struct RenderSystemForwardTransform3D;
template <typename ECS>
struct RenderSystemForwardLight3D;
template <typename ECS>
struct RenderSystemForwardWater3D;
template <typename ECS>
struct RenderSystemForwardTerrainTransform3D;
template <typename ECS>
struct RenderSystemForwardTerrainLight3D;
template <typename ECS>
struct RenderSystemForwardTerrainClipmap;

template <typename ECS, typename RL3D, typename L, typename F, typename... S>
struct RenderPassWater3D;
template <typename ECS, typename RL3D, typename L, typename F, typename... S>
struct RenderPassForward3D;

template <typename ECS>
struct MovementSystem2D;
template <typename ECS>
struct MovementSystem3D;
template <typename ECS>
struct MovementSystemPhysics3D;

// Physics Systems
template <typename ECS>
struct PhysicsRenderSystemTransform3D;

//
// Systems
//
using Systems = SystemList<MovementSystem2D<ECSDesc>, MovementSystem3D<ECSDesc>, MovementSystemPhysics3D<ECSDesc>, PhysicsRenderSystemTransform3D<ECSDesc> >;//, CollisionSystem<ECSDesc>, NotCollidingSystem<ECSDesc>>;
using RenderingSystems2D = SystemList<>;
using RenderingSystems3D = SystemList<//RenderSystemTransform3D<ECSDesc>,
																			//RenderSystemTransform3DUnity<ECSDesc>,
	RenderSystemForwardWater3D<ECSDesc>,
	RenderSystemForwardTerrainTransform3D<ECSDesc>,
	RenderSystemForwardTerrainLight3D<ECSDesc>,
	RenderSystemForwardTransform3D<ECSDesc>,
	RenderSystemForwardLight3D<ECSDesc>,
	RenderSystemForwardTerrainClipmap<ECSDesc> >;

// Define types for all systems for better usage
typedef RenderSystemForwardTransform3D<ECSDesc> TypeRenderSystemForwardTransform3D;
typedef RenderSystemForwardLight3D<ECSDesc> TypeRenderSystemForwardLight3D;
typedef RenderSystemForwardWater3D<ECSDesc> TypeRenderSystemForwardWater3D;
typedef RenderSystemForwardTerrainTransform3D<ECSDesc> TypeRenderSystemForwardTerrainTransform3D;
typedef RenderSystemForwardTerrainLight3D<ECSDesc> TypeRenderSystemForwardTerrainLight3D;
typedef RenderSystemForwardTerrainClipmap<ECSDesc> TypeRenderSystemForwardTerrainClipmap;

#endif


#ifndef ECS_DESC_HPP
#define ECS_DESC_HPP

#include "core/ECS.hpp"

// Components
class Model2D;
class Transform2D;
class DynamicTransform2D;
class PivotTransform2D;
class Model3D;
class Transform3D;
class DynamicTransform3D;
class PivotTransform3D;
class Movement2D;
class Movement3D;
class PhysicsPrimitive;
class Material;
class GeometryDataComponent;
class ShaderPipeline;
class MaterialPipeline;
class PrintComponent;
class LightComponent;
class DOOM_Floor;
class EnemyComponent;
class LevelWolfComponent;

// Groups
struct ForwardRender;
struct DeferredRender;
struct MEDKIT;
struct ENEMY;

/*
// Update Systems
//#include "CL/Movement.hpp"
template <typename ECS>
struct MovementSystem2D;
template <typename ECS>
struct MovementSystem3D;
template <typename ECS>
struct MovementSystemPhysics3D;
template <typename ECS>
struct MedkitUpdateSystem;
template <typename ECS>
struct EnemyUpdateSystem;
template <typename ECS>
struct LevelWolfUpdateSystem;

// Render 2D Systems

// Render 3D Systems
//#include "RL/RenderSystems.hpp"
template <typename ECS>
struct RenderSystemTransform3D;
template <typename ECS>
struct RenderSystemTransform3DUnity;
template <typename ECS>
struct RenderSystemWolf;
*/

#include "RL/RenderSystems.hpp"
#include "CL/Movement.hpp"
#include "GL/Enemy.hpp"
#include "GL/Medkit.hpp"

// ECS Description
using Components = ComponentList<Model2D, Transform2D, DynamicTransform2D, PivotTransform2D, Model3D, Transform3D, DynamicTransform3D, PivotTransform3D, Movement2D, Movement3D, PhysicsPrimitive, Material, GeometryDataComponent, ShaderPipeline, MaterialPipeline, PrintComponent, LightComponent, DOOM_Floor, EnemyComponent, LevelWolfComponent>;
using Groups = GroupList<ForwardRender, DeferredRender, MEDKIT, ENEMY>;
using Flags = FlagList<>;
using ECSDesc = ECS<Components, Groups, Flags>;

// Systems and ECS Manager
using Systems = SystemList<MovementSystem2D<ECSDesc>, MovementSystem3D<ECSDesc>, MovementSystemPhysics3D<ECSDesc>, MedkitUpdateSystem<ECSDesc>, EnemyUpdateSystem<ECSDesc>>;//, LevelWolfUpdateSystem<ECSDesc>>;//, CollisionSystem<ECSDesc>, NotCollidingSystem<ECSDesc>>;
using RenderingSystems2D = SystemList<>;
//using RenderingSystems3D = SystemList<RenderSystemTransform3D<ECSDesc>>;//, RenderSystemTransform3DUnity<ECSDesc>>;//, RenderSystemWolf<ECSDesc>>;//, RenderSystemTest<ECSDesc>>;//, DeferredRenderSystemTransform3D<ECSDesc>>;
using RenderingSystems3D = SystemList<UnityForwardRender<ECSDesc>::RenderingSystemTransform3DUnity<ECSDesc>, UnityForwardRender<ECSDesc>::RenderingSystemDynamicTransform3DUnity<ECSDesc>>;

using ECSLevelManager = ECSManager<ECSDesc, Systems, RenderingSystems2D, RenderingSystems3D>;

#endif

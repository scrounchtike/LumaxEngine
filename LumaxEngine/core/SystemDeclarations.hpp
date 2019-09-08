
#ifndef ECS_DECLARATIONS_HPP
#define ECS_DECLARATIONS_HPP

class ShaderPipeline;
class Material;
class Model2D;
class Model3D;
class Transform2D;
class DynamicTransform2D;
class PivotTransform2D;
class Transform3D;
class DynamicTransform3D;
class PivotTransform3D;
class PhysicsPrimitive;
class MaterialPipeline;
class TerrainClipmap;
struct PrintComponent;
struct LightComponent;
struct SimpleLightComponent;
struct Movement2D;
struct Movement3D;

#include "../RL/lights.hpp"

using Components = ComponentList<Model2D, Transform2D, DynamicTransform2D, PivotTransform2D, Model3D, Transform3D, DynamicTransform3D, PivotTransform3D, Movement2D, Movement3D, PhysicsPrimitive, Material, ShaderPipeline, MaterialPipeline, PrintComponent, LightComponent, SimpleLightComponent>;
using Groups = GroupList<ForwardRender, DeferredRender>;
using Flags = FlagList<>;
using ECSDesc = ECS<Components, Groups, Flags>;

#endif

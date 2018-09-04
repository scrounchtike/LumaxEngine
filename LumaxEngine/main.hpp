
#ifndef MAIN_HPP
#define MAIN_HPP

#include "CL/Movement.hpp"
#include "CL/GeometryData.hpp"
#include "core/ECS.hpp"

class ResourceManager;
class Window;

#include "math/types.hpp"

#include "utils/SMPL.hpp"
#include "core/ECS.hpp"

#include "RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11
#include <windows.h>
#include "RAL/RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "RAL/RenderingContextGL.hpp"
#endif

#include "RL/RenderSystems.hpp"

// Test system
/*
template <typename ECS>
class TestSystemTransform2D : public System<ECS, Transform2D>{
public:
	static void update(Transform2D* transform){
		std::cout << "hello world system transform2D" << std::endl;
	}
};
*/
/*
struct FlagTest {};

// On collision system
template <typename ECS>
class CollisionSystem : public System<ECS, Material, FlagCollided>{
public:
	static void update(Material* material){
	  material->setColorIndex(1);
	}
};
template <typename ECS>
class NotCollidingSystem : public System<ECS, Material, FlagNotCollided>{
public:
	static void update(Material* material){
		material->setColorIndex(0);
	}
};
*/

// ECS
// All components
using Components = ComponentList<Model2D, Transform2D, DynamicTransform2D, PivotTransform2D, Model3D, Transform3D, DynamicTransform3D, PivotTransform3D, Movement2D, Movement3D, PhysicsPrimitive, Material, GeometryDataComponent, ShaderPipeline, MaterialPipeline, PrintComponent>;
using Groups = GroupList<ForwardRender, DeferredRender>;
using Flags = FlagList<>;
using ECSDesc = ECS<Components, Groups, Flags>;

using Systems = SystemList<MovementSystem2D<ECSDesc>, MovementSystem3D<ECSDesc>, MovementSystemPhysics3D<ECSDesc>>;//, CollisionSystem<ECSDesc>, NotCollidingSystem<ECSDesc>>;
using RenderingSystems2D = SystemList<>;
using RenderingSystems3D = SystemList<RenderSystemTransform3D<ECSDesc>>;//, RenderSystemTest<ECSDesc>>;//, DeferredRenderSystemTransform3D<ECSDesc>>;
using ECSManagerLevelTest = ECSManager<ECSDesc, Systems, RenderingSystems2D, RenderingSystems3D>;

namespace lmx {
	extern ECSManagerLevelTest ecs;

	ECSManagerLevelTest& getECSManager();
}

#endif


#ifndef PHYSICS_WORLD_HPP
#define PHYSICS_WORLD_HPP

#include <vector>
#include <iostream>
#include <string>

#include "reactphysics3d.h"

#include "../math.hpp"
#include "../utils/Timing.hpp"

class PhysicsWorld
{
private:
	// #define a static global gravity for now
	const rp3d::Vector3 GRAVITY = rp3d::Vector3(0, -9.8, 0);
	
public:
	PhysicsWorld();
	PhysicsWorld(rp3d::Vector3 gravity);

	void update(float time_step);
	
	//
	// Allocation of primitives
	//
	rp3d::RigidBody* makeRigidBody(const rp3d::Transform& transform);
	
	rp3d::RigidBody* makeStaticRigidBody(const rp3d::Transform& transform);
	rp3d::RigidBody* makeKinematicRigidBody(const rp3d::Transform& transform);
	rp3d::RigidBody* makeDynamicRigidBody(const rp3d::Transform& transform);
	
private:
	rp3d::DynamicsWorld world;
};


//
// Physics system for interfacing between 3D rendering
// and physics simulation
//

template <typename ECS, typename... Ts>
class System;

template <typename ECS>
struct PhysicsRenderSystemTransform3D : public System<ECS, Transform3D, rp3d::RigidBody>
{
	static void update(Transform3D* transform,
										 rp3d::RigidBody* rigid_body)
	{
		// Get updated transform from physics simulation
		rp3d::Transform transform_rp3d = rigid_body->getTransform();
		
		// Update the rendering transform component
		// TODO : Add transform interpolation since simulations/rendering
		// is never at 60 Hz precise.
		transform->updateMatrix(Mat4().initTransform(transform_rp3d));
	}
};

#endif

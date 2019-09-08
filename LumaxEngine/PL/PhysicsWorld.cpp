
#include "PhysicsWorld.hpp"

#include <vector>
#include <iostream>

PhysicsWorld::PhysicsWorld()
	: world(rp3d::DynamicsWorld(GRAVITY))
{
}

PhysicsWorld::PhysicsWorld(rp3d::Vector3 gravity)
	: world(rp3d::DynamicsWorld(gravity))
{
}

void PhysicsWorld::update(float time_step)
{
	world.update(time_step);
}

rp3d::RigidBody* PhysicsWorld::makeRigidBody(const rp3d::Transform& transform)
{
	return makeDynamicRigidBody(transform);
}

rp3d::RigidBody* PhysicsWorld::makeStaticRigidBody(const rp3d::Transform& transform)
{
	rp3d::RigidBody* body = makeRigidBody(transform);
	body->setType(rp3d::BodyType::STATIC);
	return body;
}

rp3d::RigidBody* PhysicsWorld::makeKinematicRigidBody(const rp3d::Transform& transform)
{
	rp3d::RigidBody* body = makeRigidBody(transform);
	body->setType(rp3d::BodyType::KINEMATIC);
	return body;
}

rp3d::RigidBody* PhysicsWorld::makeDynamicRigidBody(const rp3d::Transform& transform)
{
	return world.createRigidBody(transform);
}

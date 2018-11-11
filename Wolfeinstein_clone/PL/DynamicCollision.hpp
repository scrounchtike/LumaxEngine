
#ifndef DYNAMIC_COLLISION_HPP
#define DYNAMIC_COLLISION_HPP

#include "PhysicsPrimitives.hpp"
#include "ContactManifold.hpp"

class DynamicCollision {
public:
	static void no_collision(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2, ContactManifold& contact) {
		return;
	}

	// Dynamic - Static Collision tests
	static void AABB_collides(const PhysicsPrimitive* aabb1, const PhysicsPrimitive* aabb2, ContactManifold& contact);
	static void AABB_collides_Sphere(const PhysicsPrimitive* aabb, const PhysicsPrimitive* sphere, ContactManifold& contact);
	static void AABB_collides_Plane(const PhysicsPrimitive* aabb, const PhysicsPrimitive* plane, ContactManifold& contact);
	static void AABB_collides_OBB(const PhysicsPrimitive* aabb, const PhysicsPrimitive* obb, ContactManifold& contact);
	// No collision response with 1D or 2D primitives

	static void Sphere_collides(const PhysicsPrimitive* sphere1, const PhysicsPrimitive* sphere2, ContactManifold& contact);
	static void Sphere_collides_Plane(const PhysicsPrimitive* sphere, const PhysicsPrimitive* plane, ContactManifold& contact);
	static void Sphere_collides_OBB(const PhysicsPrimitive* sphere, const PhysicsPrimitive* obb, ContactManifold& contact);

	static void Plane_collides(const PhysicsPrimitive* plane1, const PhysicsPrimitive* plane2, ContactManifold& contact);
	static void Plane_collides_OBB(const PhysicsPrimitive* plane, const PhysicsPrimitive* obb, ContactManifold& contact);

	static void OBB_collides(const PhysicsPrimitive* obb1, const PhysicsPrimitive* obb2, ContactManifold& contact);
};

#endif

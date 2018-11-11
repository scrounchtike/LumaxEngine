
#include "DynamicCollision.hpp"

void DynamicCollision::AABB_collides(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2, ContactManifold& contact){
	AABB* a = (AABB*)pp1;
	AABB* b = (AABB*)pp2;
	
	Vec3 amax = Vec3(a->position + a->extents);
	Vec3 amin = Vec3(a->position - a->extents);
	Vec3 bmax = Vec3(b->position + b->extents);
	Vec3 bmin = Vec3(b->position - b->extents);

	Vec3 overlap0 = bmax - amin;
	if(overlap0.x <= 0 || overlap0.y <= 0 || overlap0.z <= 0)
		return;
	Vec3 overlap1 = amax - bmin;
	if(overlap1.x <= 0 || overlap1.y <= 0 || overlap1.z <= 0)
		return;

	// We are intersecting
	Vec3 overlap = Vec3(std::min(overlap0.x, overlap1.x), std::min(overlap0.y, overlap1.y), std::min(overlap0.z, overlap1.z));
	Vec3 normal = (overlap.x < overlap.y) ? Vec3(1,0,0) : Vec3(0,1,0);
	normal = (overlap.z < overlap.x && overlap.z < overlap.y) ? Vec3(0,0,1) : normal;
	
	contact = ContactManifold();
	contact.isIntersecting = true;
	contact.normal = normal;
	contact.penetration = dot(normal, overlap);
}

void DynamicCollision::AABB_collides_Sphere(const PhysicsPrimitive* aabb, const PhysicsPrimitive* sphere, ContactManifold& contact){
	
}

void DynamicCollision::AABB_collides_Plane(const PhysicsPrimitive* aabb, const PhysicsPrimitive* plane, ContactManifold& contact){
	
}

void DynamicCollision::AABB_collides_OBB(const PhysicsPrimitive* aabb, const PhysicsPrimitive* obb, ContactManifold& contact){
	
}
// No collision response with 1D or 2D primitives

void DynamicCollision::Sphere_collides(const PhysicsPrimitive* sphere1, const PhysicsPrimitive* sphere2, ContactManifold& contact){
	
}

void DynamicCollision::Sphere_collides_Plane(const PhysicsPrimitive* sphere, const PhysicsPrimitive* plane, ContactManifold& contact){
	
}

void DynamicCollision::Sphere_collides_OBB(const PhysicsPrimitive* sphere, const PhysicsPrimitive* obb, ContactManifold& contact){
	
}

void DynamicCollision::Plane_collides(const PhysicsPrimitive* plane1, const PhysicsPrimitive* plane2, ContactManifold& contact){
	
}

void DynamicCollision::Plane_collides_OBB(const PhysicsPrimitive* plane, const PhysicsPrimitive* oob, ContactManifold& contact){
	
}

void DynamicCollision::OBB_collides(const PhysicsPrimitive* obb1, const PhysicsPrimitive* obb2, ContactManifold& contact){
	
}

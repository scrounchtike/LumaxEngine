
#include "PhysicsPrimitives.hpp"

#include "PrimitiveCollision.hpp"

#include "../RL/Renderer.hpp"

std::unordered_map<PhysicsPrimitive::keyID, PhysicsPrimitive::CollisionHandler> PhysicsPrimitive::collisionHandlers;

// Init cids at compile time
const unsigned int AABB::cid = typeid(AABB).hash_code();
const unsigned int Sphere::cid = typeid(Sphere).hash_code();
const unsigned int Plane::cid = typeid(Plane).hash_code();
const unsigned int OBB::cid = typeid(OBB).hash_code();
const unsigned int Ray::cid = typeid(Ray).hash_code();
const unsigned int Line::cid = typeid(Line).hash_code();

void PhysicsPrimitive::initialize() {
	AABB::initCases();
	Sphere::initCases();
	Plane::initCases();
	OBB::initCases();
	Ray::initCases();
	Line::initCases();
}

/*
void PhysicsPrimitive::renderAxes(const RendererGL* renderer) {
	// no op
}
*/

void AABB::initCases() {
	addCollisionHandler(cid, cid, (CollisionHandler)PrimitiveCollision::AABB_intersects);
	addCollisionHandler(cid, Sphere::cid, (CollisionHandler)PrimitiveCollision::AABB_intersects_Sphere);
	addCollisionHandler(cid, Plane::cid, (CollisionHandler)PrimitiveCollision::AABB_intersects_Plane);
	addCollisionHandler(cid, OBB::cid, (CollisionHandler)PrimitiveCollision::AABB_intersects_OBB);
	addCollisionHandler(cid, Ray::cid, (CollisionHandler)PrimitiveCollision::AABB_intersects_Ray);
	addCollisionHandler(cid, Line::cid, (CollisionHandler)PrimitiveCollision::AABB_intersects_Line);
}

/*
void AABB::renderAxes(const RendererGL* renderer) {
	static Vec3 u[3] = { Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1) };
	renderer->renderLine3D(Line3D(position, position + u[0], Vec3(1, 0, 0)));
	renderer->renderLine3D(Line3D(position, position + u[1], Vec3(0, 1, 0)));
	renderer->renderLine3D(Line3D(position, position + u[2], Vec3(1, 0, 1)));
}
*/

void Sphere::initCases() {
	addCollisionHandler(cid, cid, (CollisionHandler)PrimitiveCollision::Sphere_intersects);
	addCollisionHandler(cid, Plane::cid, (CollisionHandler)PrimitiveCollision::Sphere_intersects_Plane);
	addCollisionHandler(cid, OBB::cid, (CollisionHandler)PrimitiveCollision::Sphere_intersects_OBB);
	addCollisionHandler(cid, Ray::cid, (CollisionHandler)PrimitiveCollision::Sphere_intersects_Ray);
	addCollisionHandler(cid, Line::cid, (CollisionHandler)PrimitiveCollision::Sphere_intersects_Line);
}

/*
void Sphere::renderAxes(const RendererGL* renderer) {
	// no op
}
*/

void Plane::initCases() {
	addCollisionHandler(cid, cid, (CollisionHandler)PrimitiveCollision::Plane_intersects);
	addCollisionHandler(cid, OBB::cid, (CollisionHandler)PrimitiveCollision::Plane_intersects_OBB);
	addCollisionHandler(cid, Ray::cid, (CollisionHandler)PrimitiveCollision::Plane_intersects_Ray);
	addCollisionHandler(cid, Line::cid, (CollisionHandler)PrimitiveCollision::Plane_intersects_Line);
}

/*
void Plane::renderAxes(const RendererGL* renderer) {
	renderer->renderLine3D(Line3D(position, position + normal, Vec3(1, 0, 0)));
	renderer->renderLine3D(Line3D(position, position + tangent, Vec3(0, 1, 0)));
	Vec3 u = cross(tangent, normal);
	renderer->renderLine3D(Line3D(position, position + u, Vec3(1, 0, 1)));
}
*/

void OBB::initCases() {
	addCollisionHandler(cid, cid, (CollisionHandler)PrimitiveCollision::OBB_intersects);
	addCollisionHandler(cid, Ray::cid, (CollisionHandler)PrimitiveCollision::OBB_intersects_Ray);
	addCollisionHandler(cid, Line::cid, (CollisionHandler)PrimitiveCollision::OBB_intersects_Line);
}

/*
void OBB::renderAxes(const RendererGL* renderer) {
	renderer->renderLine3D(Line3D(position, position + u[0], Vec3(1, 0, 0)));
	renderer->renderLine3D(Line3D(position, position + u[1], Vec3(0, 1, 0)));
	renderer->renderLine3D(Line3D(position, position + u[2], Vec3(1, 0, 1)));
}
*/

void Ray::initCases() {
	// No ray-ray intersection test - Usefulness and accuracy doubtful
	// No ray-line intersection test
	addCollisionHandler(cid, cid, (CollisionHandler)PrimitiveCollision::no_intersection);
	addCollisionHandler(cid, Line::cid, (CollisionHandler)PrimitiveCollision::no_intersection);
}

/*
void Ray::renderAxes(const RendererGL* renderer) {
	// no op
}
*/

void Line::initCases() {
	// No line-line intersection test
	addCollisionHandler(cid, cid, (CollisionHandler)PrimitiveCollision::no_intersection);
}

/*
void Line::renderAxes(const RendererGL* renderer) {
	// no op
}
*/
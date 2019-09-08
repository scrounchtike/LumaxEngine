
#ifndef PRIMITIVE_COLLISION_HPP
#define PRIMITIVE_COLLISION_HPP

#include "PhysicsPrimitives.hpp"

class PrimitiveCollision {
public:
	// No intersection function
	static bool no_intersection(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
		return false;
	}

	// Intersection tests
	static bool AABB_intersects(const PhysicsPrimitive* aabb1, const PhysicsPrimitive* aabb2);
	static bool AABB_intersects_Sphere(const PhysicsPrimitive* aabb, const PhysicsPrimitive* sphere);
	static bool AABB_intersects_Plane(const PhysicsPrimitive* aabb, const PhysicsPrimitive* plane);
	static bool AABB_intersects_OBB(const PhysicsPrimitive* aabb, const PhysicsPrimitive* obb);
	static bool AABB_intersects_Ray(const PhysicsPrimitive* aabb, const PhysicsPrimitive* ray);
	static bool AABB_intersects_Line(const PhysicsPrimitive* aabb, const PhysicsPrimitive* line);
	static bool AABB_intersects_Point(const PhysicsPrimitive* aabb, const Vec3& point);

	static bool Sphere_intersects(const PhysicsPrimitive* sphere1, const PhysicsPrimitive* sphere2);
	static bool Sphere_intersects_Plane(const PhysicsPrimitive* sphere, const PhysicsPrimitive* plane);
	static bool Sphere_intersects_OBB(const PhysicsPrimitive* sphere, const PhysicsPrimitive* obb);
	static bool Sphere_intersects_Ray(const PhysicsPrimitive* sphere, const PhysicsPrimitive* ray);
	static bool Sphere_intersects_Line(const PhysicsPrimitive* sphere, const PhysicsPrimitive* line);
	static bool Sphere_intersects_Point(const PhysicsPrimitive* sphere, const Vec3& point);

	static bool Plane_intersects(const PhysicsPrimitive* plane1, const PhysicsPrimitive* plane2);
	static bool Plane_intersects_OBB(const PhysicsPrimitive* plane, const PhysicsPrimitive* obb);
	static bool Plane_intersects_Ray(const PhysicsPrimitive* plane, const PhysicsPrimitive* ray);
	static bool Plane_intersects_Line(const PhysicsPrimitive* plane, const PhysicsPrimitive* line);
	static bool Plane_intersects_Point(const PhysicsPrimitive* plane, const Vec3& point);

	static bool OBB_intersects(const PhysicsPrimitive* obb1, const PhysicsPrimitive* obb2);
	static bool OBB_intersects_Ray(const PhysicsPrimitive* obb, const PhysicsPrimitive* ray);
	static bool OBB_intersects_Line(const PhysicsPrimitive* obb, const PhysicsPrimitive* line);
	static bool OBB_intersects_Point(const PhysicsPrimitive* obb, const Vec3& point);

	// Closest point test
	static void AABB_closest_point(const AABB* aabb1, const AABB* aabb2, Vec3& pt1, Vec3& pt2);
	static void AABB_closest_point_Sphere(const AABB* aabb, const Sphere* sphere, Vec3& pt1, Vec3& pt2);
	static void AABB_closest_point_Plane(const AABB* aabb, const Plane* plane, Vec3& pt1, Vec3& pt2);
	static void AABB_closest_point_OBB(const AABB* aabb, const OBB* obb, Vec3& pt1, Vec3& pt2);
	static void AABB_closest_point_Ray(const AABB* aabb, const Ray* ray, Vec3& pt1, Vec3& pt2);
	static void AABB_closest_point_Line(const AABB* aabb, const Line* line, Vec3& pt1, Vec3& pt2);
	static void AABB_closest_point_Point(const AABB* aabb, const Vec3* point, Vec3& pt1);

	static void Sphere_closest_point(const Sphere* sphere1, const Sphere* sphere2, Vec3& pt1, Vec3& pt2);
	static void Sphere_closest_point_Plane(const Sphere* sphere, const Plane* plane, Vec3& pt1, Vec3& pt2);
	static void Sphere_closest_point_OBB(const Sphere* sphere, const OBB* obb, Vec3& pt1, Vec3& pt2);
	static void Sphere_closest_point_Ray(const Sphere* sphere, const Ray* ray, Vec3& pt1, Vec3& pt2);
	static void Sphere_closest_point_Line(const Sphere* sphere, const Line* line, Vec3& pt1, Vec3& pt2);
	static void Sphere_closest_point_Point(const Sphere* sphere, const Vec3* point, Vec3& pt1);

	static void Plane_closest_point_OBB(const Plane* plane, const OBB* obb, Vec3& pt1, Vec3& pt2);
	static void Plane_closest_point_Ray(const Plane* plane, const Ray* ray, Vec3& pt1, Vec3& pt2);
	static void Plane_closest_point_Line(const Plane* plane, const Line* line, Vec3& pt1, Vec3& pt2);
	static void Plane_closest_point_Point(const Plane* plane, const Vec3* point, Vec3& pt1);

	static void OBB_closest_point(const OBB* obb1, const OBB* obb2, Vec3& pt1, Vec3& pt2);
	static void OBB_closest_point_Ray(const OBB* obb, const Ray* ray, Vec3& pt1, Vec3& pt2);
	static void OBB_closest_point_Line(const OBB* obb, const Line* line, Vec3& pt1, Vec3& pt2);
	static void OBB_closest_point_Point(const OBB* obb, const Vec3* point, Vec3& pt1);

	static void Ray_closest_point(const Ray* ray1, const Ray* ray2, Vec3& pt1, Vec3& pt2);
	static void Ray_closest_point_Line(const Ray* ray, const Line* line, Vec3& pt1, Vec3& pt2);
	static void Ray_closest_point_Point(const Ray* ray, const Vec3& point, Vec3& pt1);

	static void Line_closest_point(const Line* line1, const Line* line2, Vec3& pt1, Vec3& pt2);
	static void Line_closest_point_Point(const Line* line, const Vec3* point, Vec3& pt1);

	// Distance computation
	static float getSquaredDistance(const AABB* aabb1, const AABB* aabb2);
	static float getSquaredDistance(const AABB* aabb, const Sphere* sphere);
	static float getSquaredDistance(const AABB* aabb, const Plane* plane);
	static float getSquaredDistance(const AABB* aabb, const OBB* obb);
	static float getSquaredDistance(const AABB* aabb, const Ray* ray);
	static float getSquaredDistance(const AABB* aabb, const Line* line);
	static float getSquaredDistance(const AABB* aabb, const Vec3& point);

	static float getSquaredDistance(const Sphere* sphere1, const Sphere* sphere);
	static float getSquaredDistance(const Sphere* sphere, const Plane* plane);
	static float getSquaredDistance(const Sphere* sphere, const OBB* obb);
	static float getSquaredDistance(const Sphere* sphere, const Ray* ray);
	static float getSquaredDistance(const Sphere* sphere, const Line* line);
	static float getSquaredDistance(const Sphere* sphere, const Vec3& point);

	static float getSquaredDistance(const Plane* plane1, const Plane* plane2);
	static float getSquaredDistance(const Plane* plane, const OBB* obb);
	static float getSquaredDistance(const Plane* plane, const Ray* ray);
	static float getSquaredDistance(const Plane* plane, const Line* line);
	static float getSquaredDistance(const Plane* plane, const Vec3& point);

	static float getSquaredDistance(const OBB* obb1, const OBB* obb2);
	static float getSquaredDistance(const OBB* obb, const Ray* ray);
	static float getSquaredDistance(const OBB* obb, const Line* line);
	static float getSquaredDistance(const OBB* obb, const Vec3& point);

	static float getSquaredDistance(const Ray* ray1, const Ray* ray2);
	static float getSquaredDistance(const Ray* ray, const Line* line);
	static float getSquaredDistance(const Ray* ray, const Vec3& point);

	static float getSquaredDistance(const Line* line1, const Line* line2);
	static float getSquaredDistance(const Line* line, const Vec3& point);

	// 2D Intersection tests
	static bool Point_intersects_Rectangle(Vec2 A, Vec2 B, Vec2 C, Vec2 D, Vec2 point);
};

#endif

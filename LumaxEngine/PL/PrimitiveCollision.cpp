
#include "PrimitiveCollision.hpp"
#include <cfloat> // for FLT_MAX and FLT_MIN

#include <algorithm>

bool PrimitiveCollision::AABB_intersects(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	AABB* aabb1 = (AABB*)pp1;
	AABB* aabb2 = (AABB*)pp2;

	if (std::abs(aabb1->position.x - aabb2->position.x) > (aabb1->extents.x + aabb2->extents.x)) return false;
	if (std::abs(aabb1->position.y - aabb2->position.y) > (aabb1->extents.y + aabb2->extents.y)) return false;
	if (std::abs(aabb1->position.z - aabb2->position.z) > (aabb1->extents.z + aabb2->extents.z)) return false;
	return true;
}

bool PrimitiveCollision::AABB_intersects_Sphere(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	AABB* aabb = (AABB*)pp1;
	Sphere* sphere = (Sphere*)pp2;

	float sqDist = getSquaredDistance(aabb, sphere->position);
	return sqDist <= sphere->radius*sphere->radius;
}

bool PrimitiveCollision::AABB_intersects_Plane(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	AABB* aabb = (AABB*)pp1;
	Plane* plane = (Plane*)pp2;

	float dist = dot(plane->normal, aabb->position) + plane->D;
	float r = aabb->extents[0] * std::abs(plane->normal[0]) + aabb->extents[1] * std::abs(plane->normal[1]) + aabb->extents[2] * std::abs(plane->normal[2]);
	return std::abs(dist) <= r;
}

bool PrimitiveCollision::AABB_intersects_OBB(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	AABB* aabb = (AABB*)pp1;
	
	static Vec3 u[3] = { Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1) };
	return OBB_intersects(new OBB(aabb->position, u, aabb->extents), pp2);
}

bool PrimitiveCollision::AABB_intersects_Ray(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	AABB* aabb = (AABB*)pp1;
	Ray* ray = (Ray*)pp2;

	float EPSILON = 0.0000001f;
	float tmin = 0.0f;
	if (ray->acceptNegativeHits)
		tmin = -FLT_MAX;
	float tmax = FLT_MAX;

	for (int i = 0; i < 3; ++i) {
		if (std::abs(ray->direction[i]) < EPSILON) {
			if (ray->position[i] < aabb->position[i] - aabb->extents[i] 
				|| ray->position[i] > aabb->position[i] + aabb->extents[i]) 
				return false;
		}
		else {
			float odd = 1.0f / ray->direction[i];
			float t1 = (aabb->position[i] - aabb->extents[i] - ray->position[i]) * odd;
			float t2 = (aabb->position[i] + aabb->extents[i] - ray->position[i]) * odd;
			if (t1 > t2)
				std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);

			if (tmin > tmax) return false;
		}
	}
	return true;
}

bool PrimitiveCollision::AABB_intersects_Line(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	AABB* aabb = (AABB*)pp1;
	Line* line = (Line*)pp2;

	float EPSILON = 0.0000001f;
	float tmin = 0.0f;
	Vec3 direction = line->end - line->start;
	float tmax = direction.getLength();
	direction /= tmax;

	for (int i = 0; i < 3; ++i) {
		if (std::abs(direction[i]) < EPSILON) {
			if (line->start[i] < aabb->position[i] - aabb->extents[i]
				|| line->start[i] > aabb->position[i] + aabb->extents[i])
				return false;
		}
		else {
			float odd = 1.0f / direction[i];
			float t1 = (aabb->position[i] - aabb->extents[i] - line->start[i]) * odd;
			float t2 = (aabb->position[i] + aabb->extents[i] - line->start[i]) * odd;
			if (t1 > t2)
				std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);

			if (tmin > tmax) return false;
		}
	}
	return true;
}

bool PrimitiveCollision::AABB_intersects_Point(const PhysicsPrimitive* pp1, const Vec3& pp2) {
	return false;
}

bool PrimitiveCollision::Sphere_intersects(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	Sphere* sphere1 = (Sphere*)pp1;
	Sphere* sphere2 = (Sphere*)pp2;

	Vec3 v = sphere1->position - sphere2->position;
	float dist2 = dot(v, v);
	float r = sphere1->radius + sphere2->radius;
	if (dist2 <= r*r)
		return true;
	return false;
}

bool PrimitiveCollision::Sphere_intersects_Plane(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	Sphere* sphere = (Sphere*)pp1;
	Plane* plane = (Plane*)pp2;

	float dist = dot(sphere->position, plane->normal) + plane->D;
	return std::abs(dist) <= sphere->radius;
}

bool PrimitiveCollision::Sphere_intersects_OBB(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	Sphere* sphere = (Sphere*)pp1;
	OBB* obb = (OBB*)pp2;

	float sqDist = getSquaredDistance(obb, sphere->position);
	return sqDist <= sphere->radius*sphere->radius;
}

bool PrimitiveCollision::Sphere_intersects_Ray(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	Sphere* sphere = (Sphere*)pp1;
	Ray* ray = (Ray*)pp2;

	Vec3 O_S = ray->position - sphere->position;
	float b = 2.0f * dot(ray->direction, O_S);
	float c = dot(O_S, O_S) - sphere->radius*sphere->radius;
	float delta = b*b - 4.0*c;
	if (delta < 0) {
		// No real solutions
		return false;
	}
	float t = (-b + sqrt(delta)) / 2.0f;
	if (ray->acceptNegativeHits)
		return t; // This is a valid real solution that intersects the ray from -inf to +inf
	if (t > 0)
		return true;
	t = (-b - sqrt(delta)) / 2.0f;
	return t > 0;
}

bool PrimitiveCollision::Sphere_intersects_Line(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	Sphere* sphere = (Sphere*)pp1;
	Line* line = (Line*)pp2;

	Vec3 direction = line->end - line->start;
	float length = direction.getLength();
	direction /= length;

	Vec3 O_S = line->start - sphere->position;
	float b = 2.0f * dot(direction, O_S);
	float c = dot(O_S, O_S) - sphere->radius*sphere->radius;
	float delta = b*b - 4.0*c;
	if (delta < 0) {
		// No real solutions
		return false;
	}
	float t = (-b + sqrt(delta)) / 2.0f;
	if (t > 0 && t < length)
		return true;
	t = (-b - sqrt(delta)) / 2.0f;
	return (t > 0 && t < length);
}

bool PrimitiveCollision::Sphere_intersects_Point(const PhysicsPrimitive* pp1, const Vec3& pp2) {
	return false;
}

bool PrimitiveCollision::Plane_intersects(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	return false;
}

bool PrimitiveCollision::Plane_intersects_OBB(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	Plane* plane = (Plane*)pp1;
	OBB* obb = (OBB*)pp2;

	float r = obb->e[0] * std::abs(dot(plane->normal, obb->u[0]))
		+ obb->e[1] * std::abs(dot(plane->normal, obb->u[1]))
		+ obb->e[2] * std::abs(dot(plane->normal, obb->u[2]));
	float s = dot(obb->position, plane->normal) - plane->D;
	return std::abs(s) <= r;
}

bool PrimitiveCollision::Plane_intersects_Ray(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	return false;
}

bool PrimitiveCollision::Plane_intersects_Line(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	return false;
}

bool PrimitiveCollision::Plane_intersects_Point(const PhysicsPrimitive* pp1, const Vec3& pp2) {
	return false;
}

bool PrimitiveCollision::OBB_intersects(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	OBB* obb1 = (OBB*)pp1;
	OBB* obb2 = (OBB*)pp2;

	Matrix3f R = Matrix3f();
	Matrix3f AbsR = Matrix3f();

	// Compute rotation matrix
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			R.set(i, j, dot(obb1->u[i], obb2->u[j]));

	// Translation of obb2 from obb1 in obb1's coord frame
	Vec3 t = obb1->position - obb2->position;
	t = R * t;

	// Absolute rotation matrix
	float epsilon = 0.000001f;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			AbsR.set(i, j, std::abs(R.get(i, j)) + epsilon);

	// Test axes A0, A2, A3
	Vector3 be_a = AbsR * obb2->e;
	for (int i = 0; i < 3; ++i)
		if (obb1->e[i] + be_a[i] < std::abs(t[i])) return false;

	// Test axes B0, B1, B2
	Vector3 ae_b = AbsR * obb1->e;
	Vector3 t_b = AbsR * t;
	for (int i = 0; i < 3; ++i)
		if (ae_b[i] + obb2->e[i] < std::abs(t_b[i])) return false;

	float ra, rb;

	// Test axis A0 x B0
	ra = obb1->e[1] * AbsR.get(2, 0) + obb1->e[2] * AbsR.get(1, 0);
	rb = obb2->e[1] * AbsR.get(0, 2) + obb2->e[2] * AbsR.get(0, 1);
	if (ra + rb < std::abs(t[2] * AbsR.get(1, 0) - t[1] * AbsR.get(2, 0))) return false;

	// Test axis A0 x B1
	ra = obb1->e[1] * AbsR.get(2, 1) + obb1->e[2] * AbsR.get(1, 1);
	rb = obb2->e[0] * AbsR.get(0, 2) + obb2->e[2] * AbsR.get(0, 0);
	if (ra + rb < std::abs(t[2] * AbsR.get(1,1) - t[1] * AbsR.get(2,1))) return false;

	// Test axis A0 * B2
	ra = obb1->e[1] * AbsR.get(2, 2) + obb1->e[2] * AbsR.get(1, 2);
	rb = obb2->e[0] * AbsR.get(0, 1) + obb2->e[1] * AbsR.get(0, 0);
	if (ra + rb < std::abs(t[2] * AbsR.get(1, 2) - t[1] * AbsR.get(2, 2))) return false;

	// Test axis A1 x B0
	ra = obb1->e[0] * AbsR.get(2, 0) + obb1->e[2] * AbsR.get(0, 0);
	rb = obb2->e[1] * AbsR.get(1, 2) + obb2->e[2] * AbsR.get(1, 1);
	if (ra + rb < std::abs(t[0] * AbsR.get(2, 0) - t[2] * AbsR.get(0, 0))) return false;

	// Test axis A1 x B1
	ra = obb1->e[0] * AbsR.get(2, 1) + obb1->e[2] * AbsR.get(0, 1);
	rb = obb2->e[0] * AbsR.get(1, 2) + obb2->e[2] * AbsR.get(1, 0);
	if (ra + rb < std::abs(t[0] * AbsR.get(2, 1) - t[2] * AbsR.get(0, 1))) return false;

	// Test axis A1 x B2
	ra = obb1->e[0] * AbsR.get(2, 2) + obb1->e[2] * AbsR.get(0, 2);
	rb = obb2->e[0] * AbsR.get(1, 1) + obb2->e[1] * AbsR.get(1, 0);
	if (ra + rb < std::abs(t[0] * AbsR.get(2, 2) - t[2] * AbsR.get(0, 2))) return false;

	// Test axis A2 x B0
	ra = obb1->e[0] * AbsR.get(1, 0) + obb1->e[1] * AbsR.get(0, 0);
	rb = obb2->e[1] * AbsR.get(2, 2) + obb2->e[2] * AbsR.get(2, 1);
	if (ra + rb < std::abs(t[1] * AbsR.get(0, 0) - t[0] * AbsR.get(1, 0))) return false;

	// Test axis A2 x B1
	ra = obb1->e[0] * AbsR.get(1, 1) + obb1->e[1] * AbsR.get(0, 1);
	rb = obb2->e[0] * AbsR.get(2, 2) + obb2->e[2] * AbsR.get(2, 0);
	if (ra + rb < std::abs(t[1] * AbsR.get(0, 1) - t[0] * AbsR.get(1, 1))) return false;

	// Test axis A2 x B2
	ra = obb1->e[0] * AbsR.get(1, 2) + obb1->e[1] * AbsR.get(0, 2);
	rb = obb2->e[0] * AbsR.get(2, 1) + obb2->e[1] * AbsR.get(2, 0);
	if (ra + rb < std::abs(t[1] * AbsR.get(0, 2) - t[0] * AbsR.get(1, 2))) return false;
	
	return true;
}

bool PrimitiveCollision::OBB_intersects_Ray(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	return false;
}

bool PrimitiveCollision::OBB_intersects_Line(const PhysicsPrimitive* pp1, const PhysicsPrimitive* pp2) {
	return false;
}

bool PrimitiveCollision::OBB_intersects_Point(const PhysicsPrimitive* pp1, const Vec3& pp2) {
	return false;
}

// Closest point tests

void PrimitiveCollision::AABB_closest_point(const AABB* aabb1, const AABB* aabb2, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::AABB_closest_point_Sphere(const AABB* aabb, const Sphere* sphere, Vec3& pt1, Vec3& pt2) {
	
}

void PrimitiveCollision::AABB_closest_point_Plane(const AABB* aabb, const Plane* plane, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::AABB_closest_point_OBB(const AABB* aabb, const OBB* obb, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::AABB_closest_point_Ray(const AABB* aabb, const Ray* ray, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::AABB_closest_point_Line(const AABB* aabb, const Line* line, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::AABB_closest_point_Point(const AABB* aabb, const Vec3* point, Vec3& pt1) {

}

void PrimitiveCollision::Sphere_closest_point(const Sphere* sphere1, const Sphere* sphere2, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Sphere_closest_point_Plane(const Sphere* sphere, const Plane* plane, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Sphere_closest_point_OBB(const Sphere* sphere, const OBB* obb, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Sphere_closest_point_Ray(const Sphere* aabb, const Ray* ray, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Sphere_closest_point_Line(const Sphere* aabb, const Line* line, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Sphere_closest_point_Point(const Sphere* sphere, const Vec3* point, Vec3& pt1) {

}

void PrimitiveCollision::Plane_closest_point_OBB(const Plane* plane, const OBB* obb, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Plane_closest_point_Ray(const Plane* aabb, const Ray* ray, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Plane_closest_point_Line(const Plane* aabb, const Line* line, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Plane_closest_point_Point(const Plane* plane, const Vec3* point, Vec3& pt1) {

}

void PrimitiveCollision::OBB_closest_point(const OBB* obb1, const OBB* obb2, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::OBB_closest_point_Ray(const OBB* aabb, const Ray* ray, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::OBB_closest_point_Line(const OBB* aabb, const Line* line, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::OBB_closest_point_Point(const OBB* obb, const Vec3* point, Vec3& pt1) {
	pt1 = obb->position;
	Vec3 cp = *point - obb->position;
	for (int i = 0; i < 3; ++i) {
		float v = dot(cp, obb->u[i]);
		if (v < -obb->e[i]) v = -obb->e[i];
		if (v > obb->e[i]) v = obb->e[i];
		pt1 += obb->u[i] * v;
	}
}

void PrimitiveCollision::Ray_closest_point(const Ray* ray1, const Ray* ray2, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Ray_closest_point_Line(const Ray* ray, const Line* line, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Ray_closest_point_Point(const Ray* ray, const Vec3& point, Vec3& pt1) {

}

void PrimitiveCollision::Line_closest_point(const Line* line1, const Line* line2, Vec3& pt1, Vec3& pt2) {

}

void PrimitiveCollision::Line_closest_point_Point(const Line* line, const Vec3* point, Vec3& pt1) {

}

// Distance computations

float PrimitiveCollision::getSquaredDistance(const AABB* aabb1, const AABB* aabb2) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const AABB* aabb, const Sphere* sphere) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const AABB* aabb, const Plane* plane) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const AABB* aabb, const OBB* obb) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const AABB* aabb, const Ray* ray) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const AABB* aabb, const Line* line) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const AABB* aabb, const Vec3& point) {
	float sqDistance = 0;
	for (int i = 0; i < 3; ++i) {
		float min = aabb->position[i] - aabb->extents[i];
		float max = aabb->position[i] + aabb->extents[i];
		float v = (point)[i];
		if (v < min) sqDistance += (min - v)*(min - v);
		if (v > max) sqDistance += (v - max)*(v - max);
	}
	return sqDistance;
}

float PrimitiveCollision::getSquaredDistance(const Sphere* sphere1, const Sphere* sphere2) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Sphere* sphere, const Plane* plane) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Sphere* sphere, const OBB* obb) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Sphere* aabb, const Ray* ray) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Sphere* aabb, const Line* line) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Sphere* sphere, const Vec3& point) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Plane* plane1, const Plane* plane2) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Plane* plane, const OBB* obb) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Plane* aabb, const Ray* ray) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Plane* aabb, const Line* line) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const Plane* plane, const Vec3& point) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const OBB* obb1, const OBB* obb2) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const OBB* aabb, const Ray* ray) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const OBB* aabb, const Line* line) {
	return 0;
}

float PrimitiveCollision::getSquaredDistance(const OBB* obb, const Vec3& point) {
	Vec3 cp = point - obb->position;
	float sqDist = 0;
	for (int i = 0; i < 3; ++i) {
		float v = dot(cp, obb->u[i]);
		if (v < -obb->e[i]) sqDist += (v + obb->e[i])*(v + obb->e[i]);
		if (v > obb->e[i]) sqDist += (v - obb->e[i])*(v - obb->e[i]);
	}
	return sqDist;
}

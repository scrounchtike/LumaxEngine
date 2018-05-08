
#ifndef MATH_HPP
#define MATH_HPP

#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Matrix4f.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3f.hpp"
#include "math/Transform2D.hpp"
#include "math/Transform3D.hpp"

static Vec3 rotate(Vec3 vector, Vec3 axis, float theta) {
	float sinTheta = sin(theta / 2.0);
	float cosTheta = cos(theta / 2.0);
	Quat4 q = Quaternion(axis.x*sinTheta, axis.y*sinTheta, axis.z*sinTheta, cosTheta);
	Quat4 q_ = q.conjugate();
	Quat4 rotation = Quaternion::mul(q, vector);
	rotation = Quaternion::mul(rotation, q_);
	return Vec3(rotation.x, rotation.y, rotation.z);
}

static int fastFloor(float value) {
	int i = (int)value;
	return i - (i > value);
}

#endif

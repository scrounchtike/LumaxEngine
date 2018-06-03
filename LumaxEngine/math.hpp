
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

static Vec3 lerp(Vec3 v1, Vec3 v2, float a){
	return Vec3(v1.x*(1.0-a) + v2.x*a,
							v1.y*(1.0-a) + v2.y*a,
							v1.z*(1.0-a) + v2.z*a);
}

static Quaternion slerp(Quaternion q1, Quaternion q2, float alpha){
	double cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y * q1.z * q2.z;
	// same quaternions, return q1
	if(fabs(cosHalfTheta) >= 1.0)
		return q1;
	
	double halfTheta = acos(cosHalfTheta);
	double sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);
	
	double ratio1 = sin((1.0 - alpha) * halfTheta) / sinHalfTheta;
	double ratio2 = sin(alpha * halfTheta) / sinHalfTheta;
	
	Quaternion qm;
	qm.w = (q1.w * ratio1 + q2.w * ratio2);
	qm.x = (q1.x * ratio1 + q2.x * ratio2);
	qm.y = (q1.y * ratio1 + q2.y * ratio2);
	qm.z = (q1.z * ratio1 + q2.z * ratio2);
	return qm;
}

#endif

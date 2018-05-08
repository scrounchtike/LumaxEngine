
#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "Vector3.hpp"

class Quaternion {
public:
	float x, y, z, w;
	Quaternion() : x(0), y(0), z(0), w(0) {}
	Quaternion(const Vector3& v, float w)
		: x(v.x), y(v.y), z(v.z), w(w)
	{ 
	}
	Quaternion(float xval, float yval, float zval, float wval)
		: x(xval), y(yval), z(zval), w(wval)
	{
	}

	Quaternion& operator*=(const Quaternion& b) {
		w = w*b.w - x*b.x - y*b.y - z*b.z;
		x = x*b.w + w*b.x + y*b.z - z*b.y;
		y = y*b.w + w*b.y + z*b.x - x*b.z; 
		z = z*b.w + w*b.z + x*b.y - y*b.x;
		return *this;
	}
	Quaternion operator*(const Quaternion& b) {
		Quaternion result;
		result.w = w*b.w - x*b.x - y*b.y - z*b.z;
		result.x = x*b.w + w*b.x + y*b.z - z*b.y;
		result.y = y*b.w + w*b.y + z*b.x - x*b.z;
		result.z = z*b.w + w*b.z + x*b.y - y*b.x;
		return result;
	}
	Quaternion& operator*=(const Vector3& b) {
		w = -x*b.x - y*b.y - z*b.z;
		x = w*b.x + y*b.z - z*b.y;
		y = w*b.y + z*b.x - x*b.z;
		z = w*b.z + x*b.y - y*b.x;
		return *this;
	}
	Quaternion operator*(const Vector3& b) {
		Quaternion result;
		result.w = x*b.x - y*b.y - z*b.z;
		result.x = w*b.x + y*b.z - z*b.y;
		result.y = w*b.y + z*b.x - x*b.z;
		result.z = w*b.z + x*b.y - y*b.x;
		return result;
	}

	Quaternion mul(const Quaternion& b) {
		Quaternion result;
		result.w = w*b.w - x*b.x - y*b.y - z*b.z;
		result.x = x*b.w + w*b.x + y*b.z - z*b.y;
		result.y = y*b.w + w*b.y + z*b.x - x*b.z;
		result.z = z*b.w + w*b.z + x*b.y - y*b.x;
		return result;
	}
	static Quaternion mul(const Quaternion& a, const Quaternion& b) {
		Quaternion result;
		result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
		result.x = a.x*b.w + a.w*b.x + a.y*b.z - a.z*b.y;
		result.y = a.y*b.w + a.w*b.y + a.z*b.x - a.x*b.z;
		result.z = a.z*b.w + a.w*b.z + a.x*b.y - a.y*b.x;
		return result;
	}
	Quaternion mul(const Vector3& b) {
		Quaternion result;
		result.w = -x*b.x - y*b.y - z*b.z;
		result.x = w*b.x + y*b.z - z*b.y;
		result.y = w*b.y + z*b.x - x*b.z;
		result.z = w*b.z + x*b.y - y*b.x;
		return result;
	}
	static Quaternion mul(Quaternion& a, Vector3& b) {
		Quaternion quaternion;
		quaternion.w = -a.x*b.x - a.y*b.y - a.z*b.z;
		quaternion.x = a.w*b.x + a.y*b.z - a.z*b.y;
		quaternion.y = a.w*b.y + a.z*b.x - a.x*b.z;
		quaternion.z = a.w*b.z + a.x*b.y - a.y*b.x;
		return quaternion;
	}
	Vector3 xyz() {
		return Vector3(x, y, z);
	}
	Quaternion conjugate() {
		return Quaternion(-x, -y, -z, w);
	}
};

typedef Quaternion Quat4;
typedef Quaternion Quat4f;

#endif

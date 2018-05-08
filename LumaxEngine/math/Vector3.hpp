
#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <math.h>
#include "Vector2.hpp"

class Vector3 {
public:
	float x, y, z;
	Vector3() : x(0), y(0), z(0) { }
	Vector3(float x) : x(x), y(x), z(x) { }
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {
	}
	bool equals(Vector3 other) {
		return x == other.x && y == other.y && z == other.z;
	}
	float getLength() const {
		return sqrt(x*x + y*y + z*z);
	}
	Vector3& normalize() {
		float length = getLength();
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
	Vector3 getNormalized() {
		Vector3 result;
		float length = getLength();
		result.x = x / length;
		result.y = y / length;
		result.z = z / length;
		return result;
	}
	Vector3 getAbs() {
		Vector3 result;
		result.x = fabs(x);
		result.y = fabs(y);
		result.z = fabs(z);
		return result;
	}
	Vector2 xy() const { return Vector2(x, y); }
	Vector2 xz() const { return Vector2(x, z); }
	Vector2 yz() const { return Vector2(y, z); }
	float operator[](int index) const {
		if (index == 0)
			return x;
		else if (index == 1)
			return y;
		else if (index == 2)
			return z;
		return x;
	}
	float& operator[](int index) {
		if (index == 0)
			return x;
		else if (index == 1)
			return y;
		else if (index == 2)
			return z;
		return x;
	}
	Vector3& operator+=(Vector3 v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator-=(Vector3 v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator+=(float s) { x += s; y += s; z += s; return *this; }
	Vector3& operator-=(float s) { x -= s; y -= s; z -= s; return *this; }
	Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Vector3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
	Vector3 operator-() {
		Vector3 temp;
		temp.x = -x;
		temp.y = -y;
		temp.z = -z;
		return temp;
	}
	Vector3 operator+(Vector3 v) const { Vector3 temp; temp.x = x + v.x; temp.y = y + v.y; temp.z = z + v.z; return temp; }
	Vector3 operator-(Vector3 v) const { Vector3 temp; temp.x = x - v.x; temp.y = y - v.y; temp.z = z - v.z; return temp; }
	//Element-wise proudct
	Vector3 mul(Vector3 v) {
		Vector3 temp;
		temp.x = x * v.x;
		temp.y = y * v.y;
		temp.z = z * v.z;
		return temp;
	}
	Vector3 div(Vector3 v) {
		Vector3 temp;
		temp.x = x / v.x;
		temp.y = y / v.y;
		temp.z = z / v.z;
		return temp;
	}

	Vector3 operator+(float s) const { Vector3 temp; temp.x = x + s; temp.y = y + s; temp.z = z + s; return temp; }
	Vector3 operator-(float s) const { Vector3 temp; temp.x = x - s; temp.y = y - s; temp.z = z - s; return temp; }
	Vector3 operator*(float s) const { Vector3 temp; temp.x = x * s; temp.y = y * s; temp.z = z * s; return temp; }
	Vector3 operator/(float s) const { Vector3 temp; temp.x = x / s; temp.y = y / s; temp.z = z / s; return temp; }
	friend float dot(const Vector3& v1, const Vector3& v2) {
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}
	friend Vector3 cross(const Vector3& v1, const Vector3& v2) {
		Vector3 temp;
		temp.x = v1.y*v2.z - v1.z*v2.y;
		temp.y = v1.z*v2.x - v1.x*v2.z;
		temp.z = v1.x*v2.y - v1.y*v2.x;
		return temp;
	}
	bool isNull() {
		return x == y && y == z && z == 0;
	}
};

typedef Vector3 Vec3;

#endif
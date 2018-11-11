
#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include <iostream>

#include <math.h>

#include "../math.hpp"

class Vector4 {
public:
	float x, y, z, w;
	Vector4() : x(0), y(0), z(0), w(0) { }
	Vector4(float x) : x(x), y(x), z(x), w(x) { }
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {

	}
	float getLength() {
		return sqrt(x*x + y*y + z*z + w*w);
	}
	Vector4& normalize() {
		float length = getLength();
		x /= length;
		y /= length;
		z /= length;
		w /= length;
		return *this;
	}
	Vector4& operator+=(Vector4 v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	Vector4& operator-=(Vector4 v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	Vector4& operator+=(float s) { x += s; y += s; z += s; w += s; return *this; }
	Vector4& operator-=(float s) { x -= s; y -= s; z -= s; w -= s; return *this; }
	Vector4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
	Vector4& operator/=(float s) { x /= s; y /= s; z /= s; w /= s; return *this; }
	Vector4  operator+(Vector4 v) { Vector4 temp; temp.x = x + v.x; temp.y = y + v.y; temp.z = z + v.z; temp.w = w + v.w; return temp; }
	Vector4  operator-(Vector4 v) { Vector4 temp; temp.x = x - v.x; temp.y = y - v.y; temp.z = z - v.z; temp.w = w - v.w; return temp; }
	Vector4  operator+(float s) { Vector4 temp; temp.x = x + s; temp.y = y + s; temp.z = z + s; temp.w = w + s; return temp; }
	Vector4  operator-(float s) { Vector4 temp; temp.x = x - s; temp.y = y - s; temp.z = z - s; temp.w = w - s; return temp; }
	Vector4  operator*(float s) { Vector4 temp; temp.x = x * s; temp.y = y * s; temp.z = z * s; temp.w = w * s; return temp; }
	Vector4  operator/(float s) { Vector4 temp; temp.x = x / s; temp.y = y / s; temp.z = z / s; temp.w = w / s; return temp; }
	static float dot(Vector4 v1, Vector4 v2) {
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	}

	float& operator[](int index) {
		if (index == 0)
			return x;
		else if (index == 1)
			return y;
		else if (index == 2)
			return z;
		else if (index == 3)
			return w;
		return x;
	}

	Vec3 xyz() {
		return Vec3(x, y, z);
	}

	//Vec4 applyMatrix(Mat4 matrix){
	//Vec4 result;
	//for(int i = 0; i < 4; ++i)
	//	result[i] = matrix.get(i, 0)*x + matrix.get(i,1)*y + matrix.get(i,2)*w + matrix.get(i,3);
	//return result;
	//}

	//Apply a Mat4 to a vector
	Vector4 applyMat4(Mat4 matrix) {
		Vector4 result;
		for (int i = 0; i < 4; ++i) {
			Vector4 row(matrix.get(i, 0), matrix.get(i, 1), matrix.get(i, 2), matrix.get(i, 3));
			result.operator[](i) = dot(*this, row);
			std::cout << row.x << " " << row.y << " " << row.z << " " << row.w << std::endl;
			std::cout << x << " " << y << " " << z << " " << w << std::endl;
			std::cout << i << " = " << operator[](i) << std::endl;
		}
		return result;
		//return *this;
	}

	//Perform perspective division
	void performPerspectiveDivision(Vec3* point) {
		point->x = x / w;
		point->y = y / w;
		point->z = z / w;
	}
private:
};

typedef Vector4 Vec4;
typedef Vector4 Vec4f;
typedef Vector4 Vector4f;

#endif


#ifndef MATRIX_3F_HPP
#define MATRIX_3F_HPP

#include "Vector2.hpp"
#include "Vector3.hpp"

class Matrix3f {
public:
	Matrix3f();
	~Matrix3f();

	Matrix3f& initIdentity();
	Matrix3f& initTranslation(float x, float y);
	Matrix3f& initTranslation(Vec2 translation);
	Matrix3f& initRotation(float angle);
	Matrix3f& initSkewedRotation(float angle, float xSkewing);
	Matrix3f& initScale(float x, float y);
	Matrix3f& initScale(Vec2 scale);

	float* getHeadPointer() const {
		float* matrix = new float[9];
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				matrix[i * 3 + j] = m[i][j];
		return matrix;
	}

	void print() const;

	void set(int i, int j, float value) { m[i][j] = value; }
	float get(int i, int j) { return m[i][j]; }

	// Operators
	Matrix3f operator*(const Matrix3f& other);
	Vector3 operator*(const Vector3& other);

	// Self operators
	Matrix3f& operator*=(const Matrix3f& other);

	Matrix3f mul(const Matrix3f& other) const;
	Vector3 mul(const Vector3& other) const;
	static Matrix3f mul(const Matrix3f& m1, const Matrix3f& m2);
private:
	float m[3][3];
};

typedef Matrix3f Mat3x3;
typedef Matrix3f Mat3f;
typedef Matrix3f Mat3;

#endif
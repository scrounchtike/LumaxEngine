
#ifndef MATRIX_4F_HPP
#define MATRIX_4F_HPP

#include "Vector3.hpp"
#include "Quaternion.hpp"

#define PI 3.1415926535

class Matrix4f {
public:
	Matrix4f();
	Matrix4f(float* ptrMatrix);
	
	Matrix4f& initIdentity();
	Matrix4f& initTranslation(float x, float y, float z);
	Matrix4f& initTranslation(Vec3 translation);

	Matrix4f& initCamera(Vec3 forward, Vec3 up, Vec3 right);
	Matrix4f& lookAt(Vec3 cameraPos, Vec3 up, Vec3 target);

	Matrix4f& initProjection(float FOV, float zNear, float zFar);
	Matrix4f& initProjectionDX11(float FOV, float ar, float zNear, float zFar);

	Matrix4f& initOrthographic(float width, float height, float zNear, float zFar);

	Matrix4f& initRotation(float x, float y, float z);
	Matrix4f& initRotation(Vec3 rotation);
	Matrix4f& initRotationDegrees(float x, float y, float z);
	Matrix4f& initRotationDegrees(Vec3 rotation);
	Matrix4f& initRotationX(float angle);
	Matrix4f& initRotationY(float angle);
	Matrix4f& initRotationZ(float angle);
	Matrix4f& initSkewedRotationZ(float angle, float xSkewing);

	// From quaternion
	Matrix4f& initRotation(const Quaternion& q);

	// Special Rotation
	Matrix4f& initSpecialRotation(const Vec3& right, const Vec3& up, const Vec3& forward);
	Matrix4f& initSpecialRotation2(const Vec3& right, const Vec3& up, const Vec3& forward);

	// Special rotations
	Matrix4f& initRotationTargetNormal(const Vec3& n0, const Vec3& nt);

	Matrix4f& initScale(float x, float y, float z);
	Matrix4f& initScale(Vec3 scale);

	Matrix4f& initPerspectiveHemicube(float zNear, float zFar);

	Matrix4f& transpose();

	Vec3 getTranslationComponent();
	Vec3 getRotationComponent();
	Quaternion getRotationQuaternionComponent();
	Vec3 getScaleComponent();

	void print() const;

	float get(int row, int col) const { return m[row][col]; }
	void set(int row, int col, float value) { m[row][col] = value; }

	float* getHeadPointer() const {
		float* matrix = new float[16];
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				matrix[i * 4 + j] = m[i][j];
			}
		}
		return matrix;
	}

	Matrix4f operator*(const Matrix4f& other) const;
	Matrix4f& operator*=(const Matrix4f& other);
	Matrix4f mul(const Matrix4f& other) const;
	static Matrix4f mul(const Matrix4f& m1, const Matrix4f& m2);

	Vector3 mulPoint(const Vector3& vec) const;
	Vector3 mulDirection(const Vector3& vec) const;
	static Vec3 mulPoint(const Matrix4f& m, const Vector3& v);
	static Vec3 mulDirection(const Matrix4f& m, const Vector3& v);

private:
	float m[4][4];
};

typedef Matrix4f Mat4;

#endif


#include "Matrix3f.hpp"

#include <cmath>

#include "../RAL/Log.hpp"

Matrix3f::Matrix3f() {

}

Matrix3f::~Matrix3f() {

}

Matrix3f& Matrix3f::initIdentity() {
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;
	return *this;
}

Matrix3f& Matrix3f::initTranslation(float x, float y) {
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = x;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = y;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;
	return *this;
}

Matrix3f& Matrix3f::initTranslation(Vec2 translation) {
	return initTranslation(translation.x, translation.y);
}

Matrix3f& Matrix3f::initRotation(float angle) {
	m[0][0] = cos(angle);	m[0][1] = -sin(angle);	m[0][2] = 0;
	m[1][0] = sin(angle);	m[1][1] = cos(angle);	m[1][2] = 0;
	m[2][0] = 0;			m[2][1] = 0;			m[2][2] = 1;
	return *this;
}

Matrix3f& Matrix3f::initSkewedRotation(float angle, float ar) {
	m[0][0] = ar*cos(angle);	m[0][1] = ar*-sin(angle);	m[0][2] = 0;
	m[1][0] = sin(angle);		m[1][1] = cos(angle);		m[1][2] = 0;
	m[2][0] = 0;				m[2][1] = 0;				m[2][2] = 1;
	return *this;
}

Matrix3f& Matrix3f::initScale(float x, float y) {
	m[0][0] = x;	m[0][1] = 0;	m[0][2] = 0;
	m[1][0] = 0;	m[1][1] = y;	m[1][2] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;
	return *this;
}

Matrix3f& Matrix3f::initScale(Vec2 scale) {
	return initScale(scale.x, scale.y);
}

void Matrix3f::print() const {
	Log::println("Matrix printing");
	Log::println("[-------------------]");
	Log::println("[ " + std::to_string(m[0][0]) + " " + std::to_string(m[0][1]) + " " + std::to_string(m[0][2]) + " ]");
	Log::println("[ " + std::to_string(m[1][0]) + " " + std::to_string(m[1][1]) + " " + std::to_string(m[1][2]) + " ]");
	Log::println("[ " + std::to_string(m[2][0]) + " " + std::to_string(m[2][1]) + " " + std::to_string(m[2][2]) + " ]");
	Log::println("[------------------]");
}

Matrix3f Matrix3f::operator*(const Matrix3f& other) {
	Matrix3f result;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 3; ++k)
				result.m[i][j] += m[i][k] * other.m[k][j];
		}
	}
	return result;
}

Vector3 Matrix3f::operator*(const Vector3& v) {
	Vector3 result(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += m[i][j] * v[j];
		}
	}
	return result;
}

Matrix3f& Matrix3f::operator*=(const Matrix3f& other) {
	float row[3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k)
				row[j] += m[i][k] * other.m[k][j];
		}
		for (int j = 0; j < 3; ++j)
			m[i][j] = row[j];
	}
	return *this;
}

Matrix3f Matrix3f::mul(const Matrix3f& other) const {
	Matrix3f result;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			int dotresult = 0;
			for (int k = 0; k < 3; ++k)
				dotresult += m[i][k] * other.m[k][j];
			result.m[i][j] = dotresult;
		}
	}
	return result;
}

Vector3 Matrix3f::mul(const Vector3& v) const {
	Vector3 result(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += m[i][j] * v[j];
		}
	}
	return result;
}

Matrix3f Matrix3f::mul(const Matrix3f& m1, const Matrix3f& m2) {
	Matrix3f result;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			int dotresult = 0;
			for (int k = 0; k < 3; ++k)
				dotresult += m2.m[i][k] * m1.m[k][j];
			result.m[i][j] = dotresult;
		}
	}
	return result;
}

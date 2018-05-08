
#include "Matrix4f.hpp"

#include <string>

#include "../RAL/Log.hpp"

Matrix4f::Matrix4f() {
	
}

Matrix4f& Matrix4f::initIdentity() {
	m[0][0] = 1;    m[0][1] = 0;    m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = 0;    m[1][1] = 1;    m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;    m[2][1] = 0;    m[2][2] = 1;    m[2][3] = 0;
	m[3][0] = 0;    m[3][1] = 0;    m[3][2] = 0;    m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initTranslation(float x, float y, float z) {
	m[0][0] = 1;    m[0][1] = 0;    m[0][2] = 0;    m[0][3] = x;
	m[1][0] = 0;    m[1][1] = 1;    m[1][2] = 0;    m[1][3] = y;
	m[2][0] = 0;    m[2][1] = 0;    m[2][2] = 1;    m[2][3] = z;
	m[3][0] = 0;    m[3][1] = 0;    m[3][2] = 0;    m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initTranslation(Vec3 translation) {
	return initTranslation(translation.x, translation.y, translation.z);
}

Matrix4f& Matrix4f::initRotation(float x, float y, float z) {
	Matrix4f rotX = Matrix4f().initRotationX(x);
	Matrix4f rotY = Matrix4f().initRotationY(y);
	Matrix4f rotZ = Matrix4f().initRotationZ(z);
	return rotZ * rotY * rotX;
}

Matrix4f& Matrix4f::initRotation(Vec3 rotation) {
	return initRotation(rotation.x, rotation.y, rotation.z);
}

Matrix4f& Matrix4f::initRotationDegrees(float x, float y, float z) {
	Matrix4f rotX = Matrix4f().initRotationX(x * PI / 180.0f);
	Matrix4f rotY = Matrix4f().initRotationY(y * PI / 180.0f);
	Matrix4f rotZ = Matrix4f().initRotationZ(z * PI / 180.0f);
	return rotZ * rotY * rotX;
}

Matrix4f& Matrix4f::initRotationDegrees(Vec3 rotation) {
	return initRotation(rotation.x, rotation.y, rotation.z);
}

Matrix4f& Matrix4f::initRotationX(float theta) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = 1;    m[0][1] = 0;           m[0][2] = 0;            m[0][3] = 0;
	m[1][0] = 0;    m[1][1] = cosTheta;    m[1][2] = -sinTheta;    m[1][3] = 0;
	m[2][0] = 0;    m[2][1] = sinTheta;    m[2][2] = cosTheta;     m[2][3] = 0;
	m[3][0] = 0;    m[3][1] = 0;           m[3][2] = 0;            m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initRotationY(float theta) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;    m[0][1] = 0;    m[0][2] = -sinTheta;    m[0][3] = 0;
	m[1][0] = 0;           m[1][1] = 1;    m[1][2] = 0;            m[1][3] = 0;
	m[2][0] = sinTheta;    m[2][1] = 0;    m[2][2] = cosTheta;     m[2][3] = 0;
	m[3][0] = 0;           m[3][1] = 0;    m[3][2] = 0;            m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initRotationZ(float theta) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;    m[0][1] = -sinTheta;    m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = sinTheta;    m[1][1] = cosTheta;     m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;           m[2][1] = 0;            m[2][2] = 1;    m[2][3] = 0;
	m[3][0] = 0;           m[3][1] = 0;            m[3][2] = 0;    m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initSkewedRotationZ(float theta, float ar) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = ar*cosTheta;	 m[0][1] = -ar*sinTheta;   m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = sinTheta;		 m[1][1] = cosTheta;       m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;			 m[2][1] = 0;              m[2][2] = 1;    m[2][3] = 0;
	m[3][0] = 0;             m[3][1] = 0;			   m[3][2] = 0;    m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initRotationTargetNormal(const Vec3& n0, const Vec3& nt) {
	/*Vec3 v = cross(n0, nt);
	float cos = dot(n0, nt);
	float d = 1 + cos;
	float a = v.x*v.y / d;
	float b = v.x*v.z / d;
	float c = v.y*v.z / d;
	m[0][0] = 1 - (v.z*v.z - v.y*v.y)/d;    m[0][1] = a - v.z;						m[0][2] = b + v.y;						m[0][3] = 0;
	m[1][0] = a + v.z;						m[1][1] = 1 - (v.z*v.z - v.x*v.x)/d;    m[1][2] = c - v.x;						m[1][3] = 0;
	m[2][0] = b - v.y;						m[2][1] = c + v.x;						m[2][2] = 1 - (v.y*v.y - v.x*v.x)/d;    m[2][3] = 0;
	m[3][0] = 0;							m[3][1] = 0;							m[3][2] = 0;							m[3][3] = 1;
	return *this;
	*/

	Vec3 v = cross(n0, nt);
	float s = sqrt(dot(v, v));
	float c = dot(n0, nt);
	float theta = acos(c) / 2.0;

	float x = sin(theta) * v.x;
	float y = sin(theta) * v.y;
	float z = sin(theta) * v.z;
	float w = cos(theta);

	float qxqy = 2 * x*y;
	float qxqz = 2 * x*z;
	float qxqw = 2 * x*w;
	float qyqz = 2 * y*z;
	float qyqw = 2 * y*w;
	float qzqw = 2 * z*w;

	m[0][0] = 1 - 2*y*y - 2*z*z;    m[0][1] = qxqy - qzqw;			m[0][2] = qxqz + qyqw;			m[0][3] = 0;
	m[1][0] = qxqy + qzqw;			m[1][1] = 1 - 2*x*x - 2*z*z;    m[1][2] = qyqz - qxqw;			m[1][3] = 0;
	m[2][0] = qxqz - qyqw;			m[2][1] = qyqz + qxqw;		    m[2][2] = 1 - 2*x*x - 2*y*y;    m[2][3] = 0;
	m[3][0] = 0;					m[3][1] = 0;					m[3][2] = 0;					m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initSpecialRotation(const Vec3& right, const Vec3& up, const Vec3& forward) {
	m[0][0] = right.x;		m[0][1] = -right.y;		m[0][2] = -right.z;		m[0][3] = 0;
	m[1][0] = -up.x;		m[1][1] = up.y;			m[1][2] = -up.z;		m[1][3] = 0;
	m[2][0] = -forward.x;   m[2][1] = -forward.y;	m[2][2] = forward.z;    m[2][3] = 0;
	m[3][0] = 0;			m[3][1] = 0;			m[3][2] = 0;			m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initSpecialRotation2(const Vec3& right, const Vec3& up, const Vec3& forward) {
	m[0][0] = right.x;		m[0][1] = up.x;			m[0][2] = forward.x;	m[0][3] = 0;
	m[1][0] = right.y;		m[1][1] = up.y;			m[1][2] = forward.y;	m[1][3] = 0;
	m[2][0] = right.z;		m[2][1] = up.z;			m[2][2] = forward.z;    m[2][3] = 0;
	m[3][0] = 0;			m[3][1] = 0;			m[3][2] = 0;			m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initScale(float x, float y, float z) {
	m[0][0] = x;    m[0][1] = 0;    m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = 0;    m[1][1] = y;    m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;    m[2][1] = 0;    m[2][2] = z;    m[2][3] = 0;
	m[3][0] = 0;    m[3][1] = 0;    m[3][2] = 0;    m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initScale(Vec3 scale) {
	return initScale(scale.x, scale.y, scale.z);
}

Matrix4f& Matrix4f::initCamera(Vec3 forward, Vec3 up, Vec3 right) {
	m[0][0] = right.x;    m[0][1] = right.y;    m[0][2] = right.z;    m[0][3] = 0;
	m[1][0] = up.x;       m[1][1] = up.y;       m[1][2] = up.z;       m[1][3] = 0;
	m[2][0] = forward.x;  m[2][1] = forward.y;  m[2][2] = forward.z;  m[2][3] = 0;
	m[3][0] = 0;          m[3][1] = 0;          m[3][2] = 0;          m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::lookAt(Vec3 cameraPos, Vec3 up, Vec3 target) {
	Vec3 zaxis = (target - cameraPos).normalize();
	Vec3 xaxis = cross(up, zaxis).normalize();
	Vec3 yaxis = cross(zaxis, xaxis);

	m[0][0] = xaxis.x;    m[0][1] = xaxis.y;    m[0][2] = xaxis.z;    m[0][3] = -dot(xaxis, cameraPos);
	m[1][0] = yaxis.x;    m[1][1] = yaxis.y;    m[1][2] = yaxis.z;    m[1][3] = -dot(yaxis, cameraPos);
	m[2][0] = zaxis.x;    m[2][1] = zaxis.y;    m[2][2] = zaxis.z;    m[2][3] = -dot(zaxis, cameraPos);
	m[3][0] = 0;		  m[3][1] = 0;		    m[3][2] = 0;		  m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initProjection(float FOV, float zNear, float zFar) {
	float tanHalfFOV = tan((FOV * PI / 180.0) / 2.0);
	float ar = 600.0f / 800.0f;
	float zRange = zFar - zNear;
	m[0][0] = ar / tanHalfFOV;        m[0][1] = 0;     m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = 0;    m[1][1] = 1.0 / tanHalfFOV;        m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;    m[2][1] = 0;    m[2][2] = (-zNear - zFar) / zRange;  m[2][3] = -2.0*zNear*zFar / zRange;
	m[3][0] = 0;    m[3][1] = 0;    m[3][2] = -1;    m[3][3] = 0;
	return *this;
}

Matrix4f& Matrix4f::initProjectionDX11(float FOV, float ar, float zNear, float zFar) {
	float tanHalfFOV = tan((FOV * PI / 180.0) / 2.0);
	float zRange = zFar - zNear;
	m[0][0] = ar / tanHalfFOV;        m[0][1] = 0;     m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = 0;    m[1][1] = 1.0 / tanHalfFOV;        m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;    m[2][1] = 0;    m[2][2] = zFar / zRange;  m[2][3] = -zFar * zNear / zRange;
	m[3][0] = 0;    m[3][1] = 0;    m[3][2] = 1;     m[3][3] = 0;
	return *this;
}

Matrix4f& Matrix4f::initOrthographic(float width, float height, float zNear, float zFar) {
	float ar = 600.0f / 800.0f;
	m[0][0] = 2.0f * ar / width;    m[0][1] = 0;                m[0][2] = 0;                     m[0][3] = 0;
	m[1][0] = 0;	           m[1][1] = 2.0f / height;    m[1][2] = 0;                     m[1][3] = 0;
	m[2][0] = 0;               m[2][1] = 0;                m[2][2] = 1.0 / (zFar - zNear);    m[2][3] = zNear / (zNear - zFar);
	m[3][0] = 0;               m[3][1] = 0;                m[3][2] = 0;                     m[3][3] = 1;
	return *this;
}

Matrix4f& Matrix4f::initPerspectiveHemicube(float zNear, float zFar) {
	float zRange = zFar - zNear;
	float a = (zFar + zNear) / zRange;
	float b = (2.0*zNear*zFar) / zRange;
	m[0][0] = 1;    m[0][1] = 0;    m[0][2] = 0;    m[0][3] = 0;
	m[1][0] = 0;    m[1][1] = 1;    m[1][2] = 0;    m[1][3] = 0;
	m[2][0] = 0;    m[2][1] = 0;    m[2][2] = 1;    m[2][3] = 3;
	m[3][0] = 0;    m[3][1] = 0;    m[3][2] = 1;   m[3][3] = 0;
	return *this;
}

Matrix4f& Matrix4f::transpose() {
	float temp[4][4];
	memcpy(temp, m, sizeof(float) * 16);
	for (int i = 0; i < 4; ++i) {
		m[i][0] = temp[0][i];
		m[i][1] = temp[1][i];
		m[i][2] = temp[2][i];
		m[i][3] = temp[3][i];
	}
	return *this;
}

void Matrix4f::print() const {
	Log::println("[-----------------]");
	Log::println("[ " + std::to_string(m[0][0]) + " " + std::to_string(m[0][1]) + " " + std::to_string(m[0][2]) + " " + std::to_string(m[0][3]) + " ]");
	Log::println("[ " + std::to_string(m[1][0]) + " " + std::to_string(m[1][1]) + " " + std::to_string(m[1][2]) + " " + std::to_string(m[1][3]) + " ]");
	Log::println("[ " + std::to_string(m[2][0]) + " " + std::to_string(m[2][1]) + " " + std::to_string(m[2][2]) + " " + std::to_string(m[2][3]) + " ]");
	Log::println("[ " + std::to_string(m[3][0]) + " " + std::to_string(m[3][1]) + " " + std::to_string(m[3][2]) + " " + std::to_string(m[3][3]) + " ]");
	Log::println("[-----------------]");
}

Matrix4f Matrix4f::operator*(const Matrix4f& other) const {
	Matrix4f result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; ++k)
				result.m[i][j] += m[i][k] * other.m[k][j];
		}
	}
	return result;
}

Matrix4f& Matrix4f::operator*=(const Matrix4f& other) {
	float row[4];
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k)
				row[j] += m[i][k] * other.m[k][j];
		}
		for (int j = 0; j < 4; ++j)
			m[i][j] = row[j];
	}
	return *this;
}

Matrix4f Matrix4f::mul(const Matrix4f& other) const {
	Matrix4f result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; ++k)
				result.m[i][j] += m[i][k] * other.m[k][j];
		}
	}
	return result;
}

Matrix4f Matrix4f::mul(const Matrix4f& m1, const Matrix4f& m2) {
	Matrix4f result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; ++k)
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
		}
	}
	return result;
}

Vector3 Matrix4f::mulPoint(const Vector3& v) const {
	Vector3 result(0,0,0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += m[i][j] * v[j];
		}
		result[i] += m[i][3];
	}
	return result;
}

Vector3 Matrix4f::mulDirection(const Vector3& v) const {
	Vector3 result(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += m[i][j] * v[j];
		}
	}
	return result;
}

Vector3 Matrix4f::mulPoint(const Matrix4f& m, const Vector3& v) {
	Vector3 result(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += m.get(i, j) * v[j];
		}
		result[i] += m.get(i, 3);
	}
	return result;
}

Vector3 Matrix4f::mulDirection(const Matrix4f& m, const Vector3& v) {
	Vector3 result(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += m.get(i,j) * v[j];
		}
	}
	return result;
}

// TODO: Decompose transformation matrices into their main components
Vec3 Matrix4f::getTranslationComponent() {
	return Vec3(m[0][3], m[1][3], m[2][3]);
}

Vec3 Matrix4f::getRotationComponent() {
	return Vec3(0, 0, 0);
}

Quaternion Matrix4f::getRotationQuaternionComponent() {
	return Quaternion(0, 0, 0, 1);
}

Vec3 Matrix4f::getScaleComponent() {
	float sx = Vec3(m[0][0], m[1][0], m[2][0]).getLength();
	float sy = Vec3(m[0][1], m[1][1], m[2][1]).getLength();
	float sz = Vec3(m[0][2], m[1][2], m[2][2]).getLength();

	return Vec3(sx, sy, sz);
}
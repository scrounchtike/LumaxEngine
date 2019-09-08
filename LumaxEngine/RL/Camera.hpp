
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../math.hpp"

#include "../RAL/input.hpp"

class Camera {
	friend class Player;
	Vec3 forward;
	Vec3 right;
	Vec3 up;
	Vec3 walk;
	
public:
#ifdef _USE_OPENGL
	Camera(Vec3 position = Vec3(0,0,0), Vec3 forward = Vec3(0,0,1), Vec3 up = Vec3(0,1,0), float fov = 70.0f, float zNear = 0.1f, float zFar = 1000.0f);
#elif defined _USE_DIRECTX11
	Camera(Vec3 position = Vec3(0, 0, 0), Vec3 forward = Vec3(0, 0, -1), Vec3 up = Vec3(0, 1, 0), float fov = 70.0f, float zNear = 0.1f, float zFar = 1000.0f);
#endif
	~Camera();

	void update();

	const Vec3& getPosition() const { return position; }

	const Mat4& getViewMatrix() const { return viewMatrix; }
	const Mat4& getProjectionMatrix() const { return projectionMatrix; }
private:
	void initialize();

	const Vec3 yAxis = Vec3(0,1,0);
	// Rotation functions
	void rotateY(float angle);
	void rotateX(float angle);

	// Mouse locking
	bool mouseLocked = false;

	// Movement variables
	float movSpeed = 0.05f;
	float sensitivity = 0.003f;
	
	Vec3 position;

	// View variables
	float fov;
	float zNear;
	float zFar;

	Mat4 cameraMatrix;
	Mat4 viewMatrix;
	Mat4 projectionMatrix;
};

#endif
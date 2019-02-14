
#ifndef LOOK_AT_CAMERA_HPP
#define LOOK_AT_CAMERA_HPP

#include "Camera.hpp"

class LookAtCamera : public Camera{
public:
	LookAtCamera(Vec3 position, Vec3 lookAt, float fov = 70.0f, float zNear = 0.1f, float zFar = 1000.0f);

	void update(Vec3 deltaMovement = Vec3(0,0,0));
	void updateViewMatrix();

	void addMovement(Vec3 movement);
	void invertPitch();
private:
	Vec3 lookAt;
};

#endif

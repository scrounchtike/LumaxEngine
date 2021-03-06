
#include "Camera.hpp"
#include "../main.hpp"

#include "../RAL/Window.hpp"

#include "../GL/Player.hpp"

Camera::Camera(Vec3 position, Vec3 forward, Vec3 up, float fov, float zNear, float zFar) 
	: position(position), forward(forward.normalize()), up(up.normalize()), fov(fov), zNear(zNear), zFar(zFar)
{
	initialize();
}

void Camera::initialize() {
#ifdef _USE_OPENGL
	right = cross(forward, up).normalize();
#elif defined _USE_DIRECTX11
	right = cross(up, forward).normalize();
	Log::println(std::to_string(right.x));
#endif

	walk = (Vec3(forward.x, 0, forward.z)).normalize();
#ifdef _USE_OPENGL
	cameraMatrix = Mat4().initCamera(-forward, up, right);
#elif defined _USE_DIRECTX11
	cameraMatrix = Mat4().initCamera(forward, up, right);
#endif
	viewMatrix = cameraMatrix * Mat4().initTranslation(-position);
#ifdef _USE_OPENGL
	projectionMatrix = Mat4().initProjection(fov, zNear, zFar);
#elif defined _USE_DIRECTX11
	float ar = lmx::getStaticWindowHeight() / lmx::getStaticWindowWidth();
	projectionMatrix = Mat4().initProjectionDX11(fov, ar, zNear, zFar);
#endif
}

void Camera::update(Vec3 deltaMovement) {
	if(Input::keyDown(LMX_KEY_H))
		return;
	//position += deltaMovement;
	float speed = 0.7f;
	if(Input::keyDown(LMX_KEY_W))
		position += walk * speed;
	if(Input::keyDown(LMX_KEY_S))
		position += walk * -speed;
	if(Input::keyDown(LMX_KEY_A))
		position += cross(up, walk) * speed;
	if(Input::keyDown(LMX_KEY_D))
		position += cross(walk, up) * speed;
	if(Input::keyDown(LMX_KEY_SPACE))
		position += yAxis * speed;
	if(Input::keyDown(LMX_KEY_LSHIFT))
		position += yAxis * -speed;
	
	Window* w = lmx::getStaticWindow();
	bool mouseMovement = false;
	
	// Mouse locking mechanism
	if (!mouseLocked && Input::mouseDownOnce(0)) {
		mouseLocked = true;
		w->setCursorPosition(lmx::getStaticWindowWidth() / 2, lmx::getStaticWindowHeight() / 2);
		w->showCursor(false);
	}
	if (mouseLocked && Input::keyDownOnce(LMX_KEY_ESCAPE)) {
		mouseLocked = false;
		w->setCursorPosition(lmx::getStaticWindowWidth() / 2, lmx::getStaticWindowHeight() / 2);
		w->showCursor(true);
	}
	if (mouseLocked) {
		// Mouse movement detection
		int mouseX, mouseY;
		Input::getMousePosition(mouseX, mouseY);
		float deltaX = mouseX - lmx::getStaticWindowWidth() / 2.0;
		float deltaY = mouseY - lmx::getStaticWindowHeight() / 2.0;

		// Rotate vectors accordingly
		if (deltaX)
			rotateY(sensitivity * deltaX);
		if (deltaY)
			rotateX(sensitivity * deltaY);

		// Re-center mouse cursor
		w->setCursorPosition(lmx::getStaticWindowWidth() / 2.0, lmx::getStaticWindowHeight() / 2.0);

		if (deltaX || deltaY)
			mouseMovement = true;
	}

	// Update Camera Matrix
	if (mouseMovement) {
#ifdef _USE_OPENGL
		cameraMatrix = Mat4().initCamera(-forward, up, right);
#elif defined _USE_DIRECTX11
		cameraMatrix = Mat4().initCamera(forward, up, right);
#endif
	}

	// Update View Matrix
	viewMatrix = cameraMatrix * Mat4().initTranslation(-position);
}

void Camera::updateViewMatrix(){
	cameraMatrix = Mat4().initCamera(-forward, up, right);
	viewMatrix = cameraMatrix * Mat4().initTranslation(-position);
}

void Camera::addMovement(Vec3 movement){
	position += movement;
}

void Camera::invertPitch(){
	Vec3 right = cross(up, forward);
	forward.y = -forward.y;
	up = cross(forward, right);
}

void Camera::rotateX(float angle) {
	float sinTheta = sin(angle / 2.0);
	float cosTheta = cos(angle / 2.0);
	Quaternion q(right * sinTheta, cosTheta);
	forward = (q.conjugate() * forward * q).xyz();
	forward.normalize();

	up = cross(right, forward).normalize();
	right = cross(forward, up).normalize();
}

void Camera::rotateY(float angle) {
	float sinTheta = sin(angle / 2.0);
	float cosTheta = cos(angle / 2.0);
	Quaternion q(0, sinTheta, 0, cosTheta);
	forward = (q.conjugate() * forward * q).xyz();
	forward.normalize();

	right = cross(forward, yAxis).normalize();
	up = cross(right, forward).normalize();
	walk = (Vec3(forward.x, 0, forward.z)).normalize();
}

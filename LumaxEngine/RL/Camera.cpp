
#include "Camera.hpp"
#include "../main.hpp"

#include "../RAL/WindowGLFW.hpp"
#include "../RAL/WindowWinAPI.hpp"

Camera::Camera(Vec3 position, Vec3 forward, Vec3 up, float fov, float zNear, float zFar) 
	: position(position), forward(forward.normalize()), up(up.normalize()), fov(fov), zNear(zNear), zFar(zFar)
{
	initialize();
}

Camera::~Camera() {

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
	projectionMatrix = Mat4().initProjection(70.0f, 0.1f, 1000.0f);
#elif defined _USE_DIRECTX11
	float ar = getStaticWindowHeight() / getStaticWindowWidth();
	projectionMatrix = Mat4().initProjectionDX11(35.0f, ar, 0.1f, 1000.0f);
#endif
}

void Camera::update() {
	Vec3 deltaPos(0, 0, 0);
	bool mouseMovement = false;

#ifdef _USE_GLFW
	WindowGLFW* w = (WindowGLFW*)getStaticWindow();
#elif defined _USE_WINAPI
	WindowWinAPI* w = (WindowWinAPI*)getStaticWindow();
#endif
	if (w->wasKeyPressed(LMX_KEY_W))
		deltaPos += walk *  movSpeed;
	else if (w->wasKeyPressed(LMX_KEY_S))
		deltaPos -= walk *  movSpeed;
	if (w->wasKeyPressed(LMX_KEY_A))
		deltaPos -= right * movSpeed;
	else if (w->wasKeyPressed(LMX_KEY_D))
		deltaPos += right * movSpeed;
	if (w->wasKeyPressed(LMX_KEY_SPACE))
		deltaPos += yAxis * movSpeed;
	else if (w->wasKeyPressed(LMX_KEY_LSHIFT))
		deltaPos -= yAxis * movSpeed;

	position += deltaPos;

	// Mouse locking mechanism
	if (!mouseLocked && w->wasButtonJustPressed(0)) {
		mouseLocked = true;
		w->setCursorPosition(getStaticWindowWidth() / 2, getStaticWindowHeight() / 2);
		w->showCursor(false);
	}
	if (mouseLocked && w->wasKeyJustPressed(LMX_KEY_ESCAPE)) {
		mouseLocked = false;
		w->setCursorPosition(getStaticWindowWidth() / 2, getStaticWindowHeight() / 2);
		w->showCursor(true);
	}
	if (mouseLocked) {
		// Mouse movement detection
		int mouseX, mouseY;
		w->getMousePosition(mouseX, mouseY);
		float deltaX = mouseX - getStaticWindowWidth() / 2.0;
		float deltaY = mouseY - getStaticWindowHeight() / 2.0;

		// Rotate vectors accordingly
		if (deltaX)
			rotateY(sensitivity * deltaX);
		if (deltaY)
			rotateX(sensitivity * deltaY);

		// Recenter mouse cursor
		w->setCursorPosition(getStaticWindowWidth() / 2.0, getStaticWindowHeight() / 2.0);

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
	if (!deltaPos.isNull() || mouseMovement)
		viewMatrix = cameraMatrix * Mat4().initTranslation(-position);
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
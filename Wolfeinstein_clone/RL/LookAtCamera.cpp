
#include "LookAtCamera.hpp"

#include <cassert>

LookAtCamera::LookAtCamera(Vec3 position, Vec3 lookAt, float fov, float zNear, float zFar) : lookAt(lookAt), Camera(position, Vec3(0,0,-1), Vec3(0,1,0), fov, zNear, zFar) {
	
}

void LookAtCamera::update(Vec3 deltaMovement){
	static float angleX = 0;
	static float angleZ = 0;
	
	if(Input::keyDown(LMX_KEY_H))
		return;
	//position += deltaMovement;
	
	if(Input::keyDown(LMX_KEY_D)){
		//rotateY(-0.1);
		angleX -= 0.01;
	}
	if(Input::keyDown(LMX_KEY_A)){
		//rotateY(0.1);
		angleX += 0.01;
	}
	
	static Vec3 V = position - lookAt;
	static float distance = std::sqrt(dot(V,V));
	//position.x = lookAt.x + distance * std::sin(angleX);
	//position.z = lookAt.z + distance * std::cos(angleX) - 15;
	position.x = 1;
	position.z = 1;
	//position.x = lookAt.x + std::sin(angle);
	//position.z = lookAt.z + std::cos(angle);

	forward = (position - lookAt).normalize();
	up = Vec3(0,1,0);
	right = cross(forward, up).normalize();
	
	//std::cout << "TEST" << std::endl;
	//rotateY(0.1);
	
	//std::cout << "test" << std::endl;
	bool mouseMovement = true;
	
	/*
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
	*/

	// Update Camera Matrix
	if (mouseMovement) {
#ifdef _USE_OPENGL
		cameraMatrix = Mat4().initCamera(-forward, up, right);
#elif defined _USE_DIRECTX11
		cameraMatrix = Mat4().initCamera(forward, up, right);
#endif
	}

	// Update View Matrix
	//viewMatrix = cameraMatrix * Mat4().initTranslation(-position);
	viewMatrix = Mat4().lookAt(position, up, lookAt);
}

void LookAtCamera::updateViewMatrix(){
	cameraMatrix = Mat4().initCamera(-forward, up, right);
	viewMatrix = cameraMatrix * Mat4().initTranslation(-position);
}

void LookAtCamera::addMovement(Vec3 movement){
	position += movement;
}

void LookAtCamera::invertPitch(){
	assert(false);
}

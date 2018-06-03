
#include "DirectInput.hpp"

#ifdef _USE_DIRECTINPUT

#include <cassert>
#include <string>

#define PRESSING_KEY 0x80

DirectInput::DirectInput(HINSTANCE hInstance, HWND hwnd, unsigned int width, unsigned int height) 
	: directInput(nullptr), keyboard(nullptr), mouse(nullptr), mouseX(0), mouseY(0), width(width), height(height)
{
	bool success = initialize(hInstance, hwnd, width, height);
	assert(success);
}

DirectInput::~DirectInput() {
	bool success = cleanUp();
	assert(success);
}

bool DirectInput::input() {
	bool result;
	result = readKeyboard();
	assert(result);
	result = readMouse();
	assert(result);
	
	processInput();

	return true;
}

bool DirectInput::initialize(HINSTANCE hInstance, HWND hwnd, unsigned int width, unsigned int height) {
	HRESULT result;

	// Direct Input state initialization
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create Direct Input instance during window initialization\n");
		assert(false);
		return false;
	}

	// Keyboard initialization
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not initialize keyboard input during creation of Direct Input context\n");
		assert(false);
		return false;
	}
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not set data format of keyboard during Direct Input initialization\n");
		assert(false);
		return false;
	}

	// Set cooperative level for keyboard to not share inputs with others
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to set cooperative level of keyboard durin Direct Input initialization\n");
		assert(false);
		return false;
	}

	// finally acquire the keyboard
	result = keyboard->Acquire();
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to acquire keyboard resource during Direct Input initialization\n");
		assert(false);
		return false;
	}

	// Mouse initialization
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not initialize mouse instance during Direct Input initialization\n");
		assert(false);
		return false;
	}
	result = mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		OutputDebugString("Error : Could not set data format of mouse during Direct Input initialization\n");
		assert(false);
		return false;
	}

	// Set cooperative level for mouse to not share inputs with others
	result = mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to set cooperative level of mouse during Direct Input initialization\n");
		assert(false);
		return false;
	}

	// Acquire mouse resource
	result = mouse->Acquire();
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to acquire mouse resource during Direct Input initialization\n");
		assert(false);
		return false;
	}

	return true;
}

bool DirectInput::cleanUp() {
	// Release the mouse resource
	if (mouse) {
		mouse->Unacquire();
		mouse->Release();
		mouse = 0;
	}

	// Release the keyboard resource
	if (keyboard) {
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}

	// Release the interface to Direct Input
	if (directInput) {
		directInput->Release();
		directInput = 0;
	}

	return true;
}

bool DirectInput::readKeyboard() {
	HRESULT result;

	result = keyboard->GetDeviceState(NUM_KEYS, (LPVOID)&Input::keyboard); // Has to be 256 keys, no more.
	if (FAILED(result)) {
		// Check if keybaord lost focus
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			keyboard->Acquire();
		else {
			assert(false);
			return false;
		}
	}

	return true;
}


bool DirectInput::readMouse() {
	HRESULT result;

	result = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&Input::mouse);
	if (FAILED(result)) {
		// Check if mouse lost focus
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			mouse->Acquire();
		else
			return false;
	}

	return true;
}

void DirectInput::processInput() {
	// Update mouse position based on change
	Input::mouseX += mouseState.lX;
	Input::mouseY += mouseState.lY;

	// Clamp mouse location within the window
	if (Input::mouseX < 0) Input::mouseX = 0;
	if (Input::mouseY < 0) Input::mouseY = 0;
	if (Input::mouseX > width) Input::mouseX = width;
	if (Input::mouseY > height) Input::mouseY = height;
}

#endif

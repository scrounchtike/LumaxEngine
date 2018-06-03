
#ifndef DIRECT_INPUT_HPP
#define DIRECT_INPUT_HPP

#include "BuildDesc.hpp"

#ifdef _USE_DIRECTINPUT

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include "input.hpp"

class DirectInput {
public:
	DirectInput(HINSTANCE, HWND, unsigned int, unsigned int);
	~DirectInput();

	bool input();
private:
	bool initialize(HINSTANCE, HWND, unsigned int, unsigned int);
	bool cleanUp();

	bool readKeyboard();
	bool readMouse();
	void processInput();

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;
	DIMOUSESTATE mouseState;

	int width, height;
};

#endif
#endif

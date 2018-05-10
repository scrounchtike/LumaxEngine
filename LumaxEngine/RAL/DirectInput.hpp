
#include "BuildDesc.hpp"

#ifdef _USE_DIRECTX11

#ifndef DIRECT_INPUT_HPP
#define DIRECT_INPUT_HPP

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class DirectInput {
public:
	DirectInput(HINSTANCE, HWND, unsigned int, unsigned int);
	~DirectInput();

	bool frame();

	bool wasKeyPressed(int key);
	bool wasKeyJustPressed(int key);
	bool wasButtonPressed(int button);
	bool wasButtonJustPressed(int button);

	void getMouseLocation(int& posX, int& posY) { posX = mouseX; posY = mouseY; }
private:
	bool initialize(HINSTANCE, HWND, unsigned int, unsigned int);
	bool cleanUp();

	bool readKeyboard();
	bool readMouse();
	void processInput();

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char previousKeyboardState[NUM_KEYS];
	unsigned char keyboardState[NUM_KEYS];
	unsigned char justPressedKeyboardState[NUM_KEYS];

	unsigned char previousMouseState[4];
	unsigned char justPressedMouse[4];
	DIMOUSESTATE mouseState;

	int width, height;
	int mouseX, mouseY;
};

#endif
#endif

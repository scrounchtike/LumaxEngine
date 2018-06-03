
#include "input.hpp"

#include <iostream>

#include "windows/WindowGLFW.hpp"
#include "windows/WindowWinAPI.hpp"
#include "DirectInput.hpp"

std::function<void(void)> Input::getInputs = NULL;

int Input::mouseX, Input::mouseY;
unsigned char Input::keyboard[NUM_KEYS];
unsigned char Input::keyboardOnce[NUM_KEYS];
unsigned char Input::mouse[NUM_BUTTONS];
unsigned char Input::mouseOnce[NUM_BUTTONS];

void Input::input(){
	int previousKeyboard[NUM_KEYS];
	for (int i = 0; i < NUM_KEYS; ++i)
		previousKeyboard[i] = keyboard[i];
	int previousMouse[NUM_BUTTONS];
	for (int i = 0; i < NUM_BUTTONS; ++i)
		previousMouse[i] = mouse[i];
	
	// API/System dependent input call
	assert(getInputs);
	getInputs();
	
	for (int i = 0; i < NUM_KEYS; ++i)
		keyboardOnce[i] = !previousKeyboard[i] && keyboard[i];
	for (int i = 0; i < NUM_BUTTONS; ++i)
		mouseOnce[i] = !previousMouse[i] && mouse[i];
}

void Input::getMousePosition(int &x, int &y){
	x = mouseX, y = mouseY;
}

bool Input::keyDown(unsigned int lmx_key){
	return keyboard[lmx_key];
}

bool Input::keyDownOnce(unsigned int lmx_key){
	return keyboardOnce[lmx_key];
}

bool Input::mouseDown(unsigned int button){
	return mouse[button];
}

bool Input::mouseDownOnce(unsigned int button){
	return mouseOnce[button];
}

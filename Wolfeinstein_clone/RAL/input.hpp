
#ifndef INPUT_HPP
#define INPUT_HPP

#include "Keys.hpp"
#include "buildDesc.hpp"

#include <functional>

#ifdef _USE_GLFW
#include "windows/WindowGLFW.hpp"
#elif defined _USE_DIRECTINPUT
#include "DirectInput.hpp"
#endif

#define NUM_KEYS 512
#define NUM_BUTTONS 16

class Input {
public:
	friend class WindowGLFW;
	friend class DirectInput;
	friend class WindowWinAPI;
	
	static void input();

	static void getMousePosition(int& x, int& y);

	static bool keyDown(unsigned int lmx_key);
	static bool keyDownOnce(unsigned int lmx_key);
	static bool mouseDown(unsigned int button);
	static bool mouseDownOnce(unsigned int button);

	static std::function<void(void)> getInputs;
private:
	static int mouseX, mouseY;
	
	static unsigned char keyboard[NUM_KEYS];
	static unsigned char keyboardOnce[NUM_KEYS];
	static unsigned char mouse[NUM_BUTTONS];
	static unsigned char mouseOnce[NUM_BUTTONS];
};

#endif

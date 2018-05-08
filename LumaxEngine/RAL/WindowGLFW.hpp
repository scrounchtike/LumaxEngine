
#ifndef WINDOW_GLFW_HPP
#define WINDOW_GLFW_HPP

#include "Window.hpp"

#ifdef _USE_OPENGL

#ifdef _USE_DIRECTX11
#include "UsingDX11.hpp"
#include "RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "UsingOpenGL.hpp"
#include "RenderingContextGL.hpp"
#endif

class WindowGLFW : public Window {
public:
	WindowGLFW(int style, const std::string& title, unsigned int width, unsigned int height);
	~WindowGLFW();

	bool shouldClose();
	void clear();
	void input();
	void update();

	void setTitle(const std::string& title);

	void showCursor(bool show);
	void setCursorPosition(int, int);

	// Input abstraction
	bool wasKeyPressed(int key);
	bool wasKeyJustPressed(int key);
	bool wasButtonPressed(int button);
	bool wasButtonJustPressed(int button);

	void getMousePosition(int& posX, int& posY);

	// Rendering Context
	bool initOpenGL();
	bool cleanUpOpenGL();

	bool initDirectX11();
	bool cleanUpDirectX11();

	bool initGLEW();
	bool cleanUpGLEW();
private:
	bool initialize(int style);
	bool cleanUp();

	GLFWwindow* window;

	// Input
	struct Input {
		static double cursorX, cursorY;
		static char keys[NUM_KEYS];
		static char keys_once[NUM_KEYS];
		static char buttons[NUM_BUTTONS];
		static char buttons_once[NUM_BUTTONS];
	};

	static void key_callback(GLFWwindow*, int, int, int, int);
	static void mouse_callback(GLFWwindow*, double, double);
	static void button_callback(GLFWwindow*, int, int, int);
};

#endif
#endif
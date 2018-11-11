
#ifndef WINDOW_GLFW_HPP
#define WINDOW_GLFW_HPP

#include "../buildDesc.hpp"
#include <string>

#ifdef _USE_GLFW

#include "../UsingOpenGL.hpp"
#include "../RenderingState.hpp"

#include "../input.hpp"

class WindowGLFW {
public:
	std::string title;
	unsigned int width, height;
	
	WindowGLFW(int style, const std::string& title, unsigned int width, unsigned int height);
	~WindowGLFW();
	
	bool shouldClose();
	void clear();
	void input();
	void update();
	
	void setTitle(const std::string& title);
	void showCursor(bool show);
	void setCursorPosition(int, int);
	
	// Rendering Context
	bool initOpenGL(const RenderingState& state);
	bool cleanUpOpenGL();
	
	bool initDirectX11(const RenderingState& state);
	bool cleanUpDirectX11();
private:
	bool initialize(int style);
	bool cleanUp();
	
	GLFWwindow* window;
	
	static void key_callback(GLFWwindow*, int, int, int, int);
	static void mouse_callback(GLFWwindow*, double, double);
	static void button_callback(GLFWwindow*, int, int, int);
};

#endif
#endif

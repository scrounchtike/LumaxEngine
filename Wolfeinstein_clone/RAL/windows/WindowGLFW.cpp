
#include "WindowGLFW.hpp"

#include <iostream>
#include "../Log.hpp"
#include <cassert>

#include "../RenderingContext.hpp"
#include "../../main.hpp"

#ifdef _USE_GLFW

WindowGLFW::WindowGLFW(int style, const std::string& title, unsigned int width, unsigned int height)
	: title(title), width(width), height(height)
{
	initialize(style);
}

WindowGLFW::~WindowGLFW() {
	cleanUp();
}

bool WindowGLFW::shouldClose() {
	return glfwWindowShouldClose(window);
}

void WindowGLFW::clear() {
	lmx::clearBuffers();
}

void WindowGLFW::input() {
	glfwPollEvents();
}

void WindowGLFW::update() {
	glfwSwapBuffers(window);
}

void WindowGLFW::setTitle(const std::string& title) {
	glfwSetWindowTitle(window, title.c_str());
}

void WindowGLFW::showCursor(bool show) {
	if (show)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowGLFW::setCursorPosition(int posX, int posY) {
	glfwSetCursorPos(window, posX, posY);
	Input::mouseX = posX;
	Input::mouseY = posY;
}

bool WindowGLFW::initialize(int style) {
	if (!glfwInit()) {
		Log::println("Error: could not initialize GLFW window");
		assert(false);
		return false;
	}

	// GLFW hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window) {
		// Add log statement
		glfwTerminate();
		assert(false);
		return false;
	}

	// Window user pointer
	glfwSetWindowUserPointer(window, this);

	// Setting the callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, button_callback);

	// Making the context current
	glfwMakeContextCurrent(window);
	// Vsync
	glfwSwapInterval(1); // FPS locks at 57-59 with it...
	
	// Have been bugs where if this is not included weird stuff happens.
	glfwPollEvents();

	return true;
}

bool WindowGLFW::cleanUp() {
	glfwTerminate();

	return true;
}

bool WindowGLFW::initOpenGL(const RenderingState& state) {
	lmx::initialize(state);
	return true;
}

bool WindowGLFW::cleanUpOpenGL() {
	lmx::cleanUp();
	return true;
}

bool WindowGLFW::initDirectX11(const RenderingState& state) {
	Log::println("Error: Cannot instantiate DX11 context in GLFW window");
	assert(false);
}

bool WindowGLFW::cleanUpDirectX11() {
	Log::println("Error: Cannot destroy non-existent DX11 context in GLFW window");
	assert(false);
}

// Callbacks
void WindowGLFW::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Input::keyboard[key] = (action != GLFW_RELEASE);
}

void WindowGLFW::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	Input::mouseX = xpos;
	Input::mouseY = ypos;
}

void WindowGLFW::button_callback(GLFWwindow* window, int button, int action, int mods) {
	Input::mouse[button] = (action != GLFW_RELEASE);
}

#endif

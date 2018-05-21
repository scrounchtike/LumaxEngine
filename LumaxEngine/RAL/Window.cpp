
#include "Window.hpp"

Window::Window(int style, const std::string& title, unsigned width, unsigned height) {
#ifdef _USE_GLFW
	window = new WindowGLFW(style, title, width, height);
#elif defined _USE_WINAPI
	window = new WindowWinAPI(style, title, width, height);
#endif
}

Window::~Window() {
	delete window;
}

bool Window::initOpenGL() {
	return window->initOpenGL();
}

bool Window::cleanUpOpenGL() {
	return window->cleanUpOpenGL();
}

bool Window::initDirectX11() {
	return window->initDirectX11();
}

bool Window::cleanUpDirectX11() {
	return window->cleanUpDirectX11();
}
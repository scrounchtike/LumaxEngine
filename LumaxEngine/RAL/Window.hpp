
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "buildDesc.hpp"
#include "Keys.hpp"

#ifdef _USE_GLFW
#include "windows/WindowGLFW.hpp"
#elif defined _USE_WINAPI
#include "windows/WindowWinAPI.hpp"
#endif

#include <cassert>
#include <string>

class Window {
public:
	Window(int style, const std::string& title, unsigned width, unsigned height);
	~Window();

	bool shouldClose() { return window->shouldClose(); }
	void clear() { window->clear(); }
	void input() { window->input(); }
	void update() { window->update(); }
	
	void setTitle(const std::string& title) { window->setTitle(title); }
	void showCursor(bool show) { window->showCursor(show); }
	void setCursorPosition(int x, int y) { window->setCursorPosition(x, y); }
	
	unsigned int getWidth() { return window->width; }
	unsigned int getHeight() { return window->height; }
	std::string& getTitle() { return window->title; }

	// Rendering Context
	bool initOpenGL(const RenderingState& state);
	bool cleanUpOpenGL();

	bool initDirectX11(const RenderingState& state);
	bool cleanUpDirectX11();
private:
	// Window API dependent implementation
#ifdef _USE_GLFW
	WindowGLFW* window;
#elif defined _USE_WINAPI
	WindowWinAPI* window;
#endif
};

#endif

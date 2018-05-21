
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

#define NUM_KEYS 512
#define NUM_BUTTONS 8

class Window {
public:
	Window(int style, const std::string& title, unsigned width, unsigned height);
	~Window();

	std::string title;
	unsigned int width, height;

	bool shouldClose() { return window->shouldClose(); }
	void clear() { window->clear(); }
	void input() { window->input(); }
	void update() { window->update(); }

	void setTitle(const std::string& title) { window->setTitle(title); }

	void showCursor(bool show) { window->showCursor(show); }
	void setCursorPosition(int x, int y) { window->setCursorPosition(x, y); }

	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return height; }
	std::string& getTitle() { return title; }

	// Input abstraction
	// TODO: Move Input layer out of Window class -> Own Input.hpp abstraction
	bool wasKeyPressed(int key) { return window->wasKeyPressed(key); }
	bool wasKeyJustPressed(int key) { return window->wasKeyJustPressed(key); }
	bool wasButtonPressed(int button) { return window->wasButtonPressed(button); }
	bool wasButtonJustPressed(int button) { return window->wasButtonJustPressed(button); }

	void getCursorPosition(int& posX, int& posY) { window->getCursorPosition(posX, posY); }

	// Rendering Context
	bool initOpenGL();
	bool cleanUpOpenGL();

	bool initDirectX11();
	bool cleanUpDirectX11();
	
	RenderingContext* getRenderingContext() { return window->getRenderingContext(); }
private:
	// Window API dependent implementation
#ifdef _USE_GLFW
	WindowGLFW* window;
#elif defined _USE_WINAPI
	WindowWinAPI* window;
#endif
};

#endif

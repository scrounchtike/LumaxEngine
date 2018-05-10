
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "buildDesc.hpp"
#include "RenderingContext.hpp"
#include "Keys.hpp"

#include <cassert>
#include <string>

#define NUM_KEYS 512
#define NUM_BUTTONS 8

class Window {
public:
	virtual bool shouldClose() = 0;
	virtual void clear() = 0;
	virtual void input() = 0;
	virtual void update() = 0;

	virtual void setTitle(const std::string& title) = 0;

	virtual void showCursor(bool) = 0;
	virtual void setCursorPosition(int, int) = 0;

	virtual unsigned int getWidth() { return width; }
	virtual unsigned int getHeight() { return height; }
	virtual std::string& getTitle() { return title; }

	// Input abstraction
	virtual bool wasKeyPressed(int key) = 0;
	virtual bool wasKeyJustPressed(int key) = 0;
	virtual bool wasButtonPressed(int button) = 0;
	virtual bool wasButtonJustPressed(int button) = 0;

	virtual void getMousePosition(int& posX, int& posY) = 0;

	// Rendering Context
	virtual bool initOpenGL() = 0;
	virtual bool cleanUpOpenGL() = 0;

	virtual bool initDirectX11() = 0;
	virtual bool cleanUpDirectX11() = 0;
	
	RenderingContext* getRenderingContext() { return renderContext; }

	virtual ~Window() { };
protected:
	Window(const std::string& title, unsigned int width, unsigned int height)
		: title(title), width(width), height(height) { }

	std::string title;
	unsigned int width, height;

	RenderingContext* renderContext;
};

#endif

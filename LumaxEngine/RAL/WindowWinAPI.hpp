
#ifndef WINDOW_WIN_API_HPP
#define WINDOW_WIN_API_HPP

#include "Window.hpp"
#include "DirectInput.hpp"

#ifdef _USE_DIRECTX11
#include "UsingDX11.hpp"
#include "RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "UsingOpenGL.hpp"
#include "RenderingContextGL.hpp"
#endif

#include <windows.h>

class WindowWinAPI : public Window {
public:
	WindowWinAPI(int style, const std::string& title, unsigned int width, unsigned int height);
	~WindowWinAPI();

	bool shouldClose();
	void clear();
	void input();
	void update();

	void setTitle(const std::string& title);

	void showCursor(bool showCursor);
	void setCursorPosition(int posX, int posY);

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
private:
	bool initialize(int nCmdShow);
	bool cleanUp();

	// Status variables
	static bool exitRequested;

	// DirectInput
	DirectInput* directInput;

	// Window handles
	HWND hwnd;
	HINSTANCE hInstance;

	// OpenGL context
	HGLRC opengl;

	static void processKey(UINT umsg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK windowProc(HWND, UINT umsg, WPARAM wparam, LPARAM lparam);
};

#endif
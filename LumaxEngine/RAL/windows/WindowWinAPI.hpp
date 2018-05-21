
#ifndef WINDOW_WIN_API_HPP
#define WINDOW_WIN_API_HPP

#include "../buildDesc.hpp"
#include <string>

#ifdef _USE_WINAPI

#include "../DirectInput.hpp"

#ifdef _USE_DIRECTX11
#include "../UsingDX11.hpp"
#include "../RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "../UsingOpenGL.hpp"
#include "../RenderingContextGL.hpp"
#endif

#include <windows.h>

class WindowWinAPI {
public:
	std::string title;
	unsigned int width, height;

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

	void getCursorPosition(int& posX, int& posY);

	// Rendering Context
	bool initOpenGL();
	bool cleanUpOpenGL();

	bool initDirectX11();
	bool cleanUpDirectX11();

	RenderingContext* getRenderingContext() { return renderContext; }
private:
	RenderingContext* renderContext;

	bool initialize(int nCmdShow);
	bool cleanUp();

	// Status variables
	static bool exitRequested;

	// DirectInput
#ifdef _USE_DIRECTINPUT
	DirectInput* directInput;
#endif

	// Window handles
	HWND hwnd;
	HINSTANCE hInstance;

	// OpenGL context
	HGLRC opengl;

	static void processKey(UINT umsg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK windowProc(HWND, UINT umsg, WPARAM wparam, LPARAM lparam);
};

#endif
#endif

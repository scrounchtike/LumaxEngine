
#include "WindowWinAPI.hpp"

#ifdef _USE_DIRECTX11

// Static variables
bool WindowWinAPI::exitRequested = false;

WindowWinAPI::WindowWinAPI(int style, const std::string& title, unsigned int width, unsigned int height)
	: Window(title, width, height)
{
	bool success = initialize(style);
	assert(success);
}

WindowWinAPI::~WindowWinAPI() {
	bool success = cleanUp();
	assert(success);
}

bool WindowWinAPI::shouldClose() {
	return exitRequested;
}

void WindowWinAPI::clear() {
	float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	renderContext->clearBuffers(color);
}

void WindowWinAPI::input() {
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Direct Input
	directInput->frame();
}

void WindowWinAPI::update() {
#ifdef _USE_DIRECTX11
	renderContext->swapBuffers();
#elif defined _USE_OPENGL
	static HDC hdc = GetDC(hwnd);
	SwapBuffers(hdc);
#endif
}

bool WindowWinAPI::wasKeyPressed(int key) {
	return directInput->wasKeyPressed(key);
}

bool WindowWinAPI::wasKeyJustPressed(int key) {
	return directInput->wasKeyJustPressed(key);
}

bool WindowWinAPI::wasButtonPressed(int button) {
	return directInput->wasButtonPressed(button);
}

bool WindowWinAPI::wasButtonJustPressed(int button) {
	return directInput->wasButtonJustPressed(button);
}

void WindowWinAPI::getMousePosition(int& posX, int& posY) {
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	posX = pt.x;
	posY = pt.y;
}

void WindowWinAPI::setTitle(const std::string& title) {
	SetWindowText(hwnd, title.c_str());
}

void WindowWinAPI::showCursor(bool show) {
	// Happy little Win32 bug: Need to call ShowCursor twice
	// for effect to take place
	ShowCursor(show);
	ShowCursor(show);
}

void WindowWinAPI::setCursorPosition(int posX, int posY) {
	POINT pt;
	pt.x = posX;
	pt.y = posY;
	ClientToScreen(hwnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

bool WindowWinAPI::initialize(int nCmdShow) {
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));

	hInstance = GetModuleHandle(NULL);

	wc.lpfnWndProc = windowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = (LPCSTR)title.c_str();

	RegisterClass(&wc);

	int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	// TODO: Style of CS_OWNDC is only for setting our openGL with WGL in the window
	// TODO: Check if we use OpenGL or DirectX for the style
	unsigned int style = 0;
#ifdef _USE_OPENGL
	style |= CS_OWNDC;
#endif
	hwnd = CreateWindowEx(0, (LPCSTR)title.c_str(), (LPCSTR)title.c_str(), style | WS_OVERLAPPEDWINDOW,
		posX, posY, width, height, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) {
		OutputDebugString("Error: Could not create Win API window\n");
		assert(false);
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowCursor(true);

	// Direct Input initialization
	directInput = new DirectInput(hInstance, hwnd, width, height);

	return true;
}

bool WindowWinAPI::cleanUp() {
	// DirectInput cleanUp
	delete directInput;

	ShowCursor(true);
	ShowCursor(true);

	// Deleting Window
	DestroyWindow(hwnd);
	hwnd = 0;

	// Unregister window class with title name
	UnregisterClass((LPCSTR)title.c_str(), hInstance);
	hInstance = NULL;

	return true;
}

bool WindowWinAPI::initOpenGL() {
	// Get Device Context
	HDC hdc = GetDC(hwnd);

	// Create Pixel format descriptor struct to find pixel format matching
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		8,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	// Find format matching descriptor and set it
	unsigned int format = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, format, &pfd);

	// Create OpenGL context
	opengl = wglCreateContext(hdc);
	// Make context current
	wglMakeCurrent(hdc, opengl);

	// Init GL rendering context
#ifdef _USE_OPENGL
	ContextDescription stateDesc;
	renderContext = new RenderingContextGL(stateDesc);
#endif

	return true;
}

bool WindowWinAPI::cleanUpOpenGL() {
	delete renderContext;
	renderContext = nullptr;

	// Get Device Context
	HDC hdc = GetDC(hwnd);

	// Make current context null so that we dont delete a working context
	wglMakeCurrent(hdc, NULL);

	// Delete our context
	assert(opengl);
	wglDeleteContext(opengl);

	return true;
}

bool WindowWinAPI::initDirectX11() {
#ifdef _USE_DIRECTX11
	ContextDescription stateDesc;
	renderContext = new RenderingContextDX11(stateDesc, hwnd);
#endif

	return true;
}

bool WindowWinAPI::cleanUpDirectX11() {
	delete renderContext;
	renderContext = nullptr;
	
	return true;
}

// Callbacks / State functions for Win32 window
void WindowWinAPI::processKey(UINT umsg, WPARAM wparam, LPARAM lparam) {
	// Used only if DirectInput is not in use
}

LRESULT CALLBACK WindowWinAPI::windowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		exitRequested = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exitRequested = true;
		break;
	case WM_QUIT:
		exitRequested = true;
		break;
	default:
		DefWindowProc(hwnd, umsg, wparam, lparam);
		break;
	}

	return 1;
}

#endif

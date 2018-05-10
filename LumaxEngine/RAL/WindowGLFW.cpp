
#include "WindowGLFW.hpp"

#include "Log.hpp"

#ifdef _USE_OPENGL

double WindowGLFW::Input::cursorX, WindowGLFW::Input::cursorY;
char WindowGLFW::Input::keys[NUM_KEYS];
char WindowGLFW::Input::keys_once[NUM_KEYS];
char WindowGLFW::Input::buttons[NUM_BUTTONS];
char WindowGLFW::Input::buttons_once[NUM_BUTTONS];

WindowGLFW::WindowGLFW(int style, const std::string& title, unsigned int width, unsigned int height)
	: Window(title, width, height)
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
	float color[4] = { 0, 0, 1, 1 };
	renderContext->clearBuffers(color);
}

void WindowGLFW::input() {
	int previous_keys[NUM_KEYS];
	for (int i = 0; i < NUM_KEYS; ++i)
		previous_keys[i] = Input::keys[i];
	int previous_buttons[NUM_BUTTONS];
	for (int i = 0; i < NUM_BUTTONS; ++i)
		previous_buttons[i] = Input::buttons[i];

	glfwPollEvents();

	for (int i = 0; i < NUM_KEYS; ++i)
		Input::keys_once[i] = !previous_keys[i] && Input::keys[i];
	for (int i = 0; i < NUM_BUTTONS; ++i)
		Input::buttons_once[i] = !previous_buttons[i] && Input::buttons[i];
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

bool WindowGLFW::wasKeyPressed(int key) {
	return Input::keys[key];
}

bool WindowGLFW::wasKeyJustPressed(int key) {
	return Input::keys_once[key];
}

bool WindowGLFW::wasButtonPressed(int button) {
	return Input::buttons[button];
}

bool WindowGLFW::wasButtonJustPressed(int button) {
	return Input::buttons_once[button];
}

void WindowGLFW::setCursorPosition(int posX, int posY) {
	glfwSetCursorPos(window, posX, posY);
	Input::cursorX = posX;
	Input::cursorY = posY;
}

void WindowGLFW::getMousePosition(int& posX, int& posY) {
	double cursorX, cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	posX = (int)cursorX;
	posY = (int)cursorY;
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
	glfwSwapInterval(1); // FPS stays at 57-59 with it...

	// Have been bugs where if this is not included weird stuff happens.  .. really? ..
	glfwPollEvents();

	return true;
}

bool WindowGLFW::cleanUp() {
	glfwTerminate();

	return true;
}

bool WindowGLFW::initOpenGL() {
	ContextDescription stateDesc;
	renderContext = new RenderingContextGL(stateDesc);
	return true;
}

bool WindowGLFW::cleanUpOpenGL() {
	delete renderContext;
	renderContext = 0;

	return true;
}

bool WindowGLFW::initDirectX11() {
	return true;
}

bool WindowGLFW::cleanUpDirectX11() {
	return true;
}

bool WindowGLFW::initGLEW() {
	return true;
}

bool WindowGLFW::cleanUpGLEW() {
	return true;
}

// Callbacks
void WindowGLFW::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	WindowGLFW::Input::keys[key] = (action == GLFW_PRESS);
}

void WindowGLFW::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	WindowGLFW::Input::cursorX = xpos;
	WindowGLFW::Input::cursorY = ypos;
}

void WindowGLFW::button_callback(GLFWwindow* window, int button, int action, int mods) {
	WindowGLFW::Input::buttons[button] = (action == GLFW_PRESS);
}

#endif

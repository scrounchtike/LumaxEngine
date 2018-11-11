
//#define GLEW_STATIC
#include "buildDesc.hpp"

#ifdef _WINDOWS
#include <GL/glew.h>
#elif defined _UNIX
#include "OpenGL/gl3.h"
#endif

#ifdef _UNIX
#define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#ifdef _WINDOWS
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "OpenGL32.lib")
#endif

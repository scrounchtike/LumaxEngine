
#ifndef BUILD_DESC_HPP
#define BUILD_DESC_HPP

/// System description

#if defined _WIN32 || defined _WIN64
#define _WINDOWS
#endif
#ifdef __unix
#define _UNIX
#endif

#define _UNIX

/// System independent dependencies

// Assimp
#ifdef _WINDOWS
#pragma comment(lib, "assimp-vc140-mt.lib")
#endif

/// Build description

//#define _USE_WINAPI
#define _USE_GLFW

//#define _USE_DIRECTX11
#define _USE_OPENGL

// Input definitions
#ifdef _USE_WINAPI
#define _USE_DIRECTINPUT
#elif defined _USE_GLFW
#define _USE_GLFWINPUT
#endif

#endif

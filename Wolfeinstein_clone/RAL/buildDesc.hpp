
#ifndef BUILD_DESC_HPP
#define BUILD_DESC_HPP

/// System description

#if defined _WIN32 || defined _WIN64
#define _WINDOWS
#endif
#if defined __FreeBSD__ || defined __OpenBSD__ || defined __NetBSD__ || defined __bsdi__
#define _BSD
#endif
#if defined __gnu_linux__ || defined __linux__ || defined linux || defined __linux
#define _LINUX
#endif
#if defined macintosh || defined Macintosh || defined __APPLE__
#define _MACOSX
#endif
#if defined sun || defined __sun
#define _SOLARIS
#endif

#if defined unix || defined __unix__ || defined __unix || defined _BSD || defined _LINUX || defined _MACOSX || defined _SOLARIS
#define _UNIX
#endif

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

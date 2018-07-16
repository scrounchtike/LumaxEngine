
#ifndef MAIN_HPP
#define MAIN_HPP

#include "RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11
#include <windows.h>
#include "RAL/RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "RAL/RenderingContextGL.hpp"
#endif

#include "RML/ResourceManager.hpp"

#include "math/types.hpp"

Window* getStaticWindow();

ResourceManager* getStaticResourceManager();

// Width and height getters
float getStaticWindowWidth();
float getStaticWindowHeight();

#endif

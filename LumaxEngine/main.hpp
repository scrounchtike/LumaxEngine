
#ifndef MAIN_HPP
#define MAIN_HPP

#include <windows.h>

#include "RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11
#include "RAL/RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "RAL/RenderingContextGL.hpp"
#endif

#include "RML/ResourceManager.hpp"

#include "math/types.hpp"

#ifdef _USE_DIRECTX11
RenderingContextDX11* getStaticRenderingContextDX11();
#elif defined _USE_OPENGL
RenderingContextGL* getStaticRenderingContextGL();
#endif

Window* getStaticWindow();

ResourceManager* getStaticResourceManager();

// Width and height getters
float getStaticWindowWidth();
float getStaticWindowHeight();

#endif
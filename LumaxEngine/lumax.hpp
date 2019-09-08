
#ifndef LUMAX_HPP
#define LUMAX_HPP

#include "core/Level.hpp"
#include "RML/ResourceManager.hpp"
#include "RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11
#include <windows.h>
#include "RAL/RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "RAL/RenderingContextGL.hpp"
#endif

class Window;
class ResourceManager;

namespace lmx{
	// Window
	extern Window* window;
	// Resource Manager
	extern ResourceManager* resManager;
	
	Window* getStaticWindow();
	
	ResourceManager* getStaticResourceManager();
	
	// Width and height getters
	float getStaticWindowWidth();
	float getStaticWindowHeight();
	
	// Level
	extern LevelType* level;
	LevelType* getStaticLevel();
}



#endif

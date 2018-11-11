
#ifndef LUMAX_HPP
#define LUMAX_HPP

#include "math/types.hpp"

#include "utils/SMPL.hpp"
#include "core/ECS.hpp"

#include "RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11
#include <windows.h>
#include "RAL/RenderingContextDX11.hpp"
#elif defined _USE_OPENGL
#include "RAL/RenderingContextGL.hpp"
#endif

//#include "core/Level.hpp"
//#include "main.hpp"

class Window;
class ResourceManager;
class Camera;

//typedef Level<Systems, RenderingSystems2D, RenderingSystems3D> LevelType;

namespace lmx{
	// Window
	extern Window* window;
	// Resource Manager
	extern ResourceManager* resManager;
	// Camera
	extern Camera* camera;
	// Current Level
	//extern LevelType* level;
	
	Window* getStaticWindow();
	ResourceManager* getStaticResourceManager();
	Camera* getStaticCamera();
	//LevelType* getCurrentLevel();
	
	// Width and height getters
	float getStaticWindowWidth();
	float getStaticWindowHeight();
}

#endif

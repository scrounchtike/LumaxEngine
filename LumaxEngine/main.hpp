
#ifndef MAIN_HPP
#define MAIN_HPP

#include "CL/Movement.hpp"
#include "CL/GeometryData.hpp"
#include "core/ECS.hpp"

class ResourceManager;
class Window;

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

// Render Systems include paths
//#include "RL/RenderSystems.hpp"

#include "../RML/ResourceManager.hpp"
#include "../CL/LightComponent.hpp"
#include "RL/ForwardRenderer.hpp"

#include "CL/LightComponent.hpp"
#include "CL/DOOMComponents.hpp"

#endif

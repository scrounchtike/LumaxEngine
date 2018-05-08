
#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include <vector>
#include <cassert>
#include <string>
#include <fstream>

#include "../RL/Model3DGL.hpp"
#include "../RL/Model3DDX11.hpp"
#include "../utils/StringUtils.hpp"

class OBJLoader {
public:
#ifdef _USE_OPENGL
	static Model3DGL* loadFileOBJ(const std::string& filename);
#elif defined _USE_DIRECTX11
	static Model3DDX11* loadFileOBJ(const std::string& filename);
#endif
};

#endif
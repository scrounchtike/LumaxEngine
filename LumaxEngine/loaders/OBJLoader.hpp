
#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include <vector>
#include <cassert>
#include <string>
#include <fstream>

#include "../RL/Model3D.hpp"
#include "../utils/StringUtils.hpp"

class OBJLoader {
public:
	static Model3D* loadFileOBJ(const std::string& filename);
};

#endif
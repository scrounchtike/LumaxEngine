
#ifndef BONE_HPP
#define BONE_HPP

#include <string>
#include "../../math.hpp"

class Bone {
public:
	Bone(const std::string& name)
		: name(name) { }
	
	std::string name;
	Mat4 localBoneSpaceTransform;
	Mat4 parentSpaceTransform;
	Bone* parent;
	std::vector<Bone*> children;
	unsigned int index;
};

#endif

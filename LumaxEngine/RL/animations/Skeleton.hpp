
#ifndef SKELETON_HPP
#define SKELETON_HPP

#include <vector>

#include "Bone.hpp"

class Skeleton {
public:
	std::vector<Bone*> bones;
	Mat4 globalInverseTransform;
	
	Skeleton(unsigned numBones) {
		bones.resize(numBones);
	}
	
	Bone* root;
};

#endif

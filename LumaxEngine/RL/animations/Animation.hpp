
#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <vector>
#include <string>

#include "../../math.hpp"

#include "Skeleton.hpp"

struct BoneTransformation {
	Vec3 position;
	Quaternion rotation;

	BoneTransformation() { }
	BoneTransformation(const Vec3& position, const Quaternion& rotation) : position(position), rotation(rotation) { }
};

struct Keyframe {
	std::vector<BoneTransformation> boneTransformations;
	
	Keyframe() { }
	Keyframe(unsigned numBones){
		boneTransformations.resize(numBones);
	}
};

class Animation {
public:
	double duration;
	double ticksPerSecond;
	unsigned numBones;
	std::vector<Keyframe> keyframes;
	std::vector<double> timeStamps;

	Animation(double duration, double ticksPerSecond, unsigned numBones);

	void updateBones(Skeleton* skeleton, std::vector<Mat4>& boneTransforms, double time);
	void getFinalBoneTransformations(Skeleton* skeleton, std::vector<Mat4>& boneTransforms, Bone* bone, const Mat4& parentTransform);
};

#endif

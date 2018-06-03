
#include "Animation.hpp"

#include <algorithm>

Animation::Animation(double duration, double ticksPerSecond, unsigned numBones)
	: duration(duration), ticksPerSecond(ticksPerSecond), numBones(numBones)
{
	//TODO: Resize keyframes size when we know its size?
}

void Animation::updateBones(Skeleton* skeleton, std::vector<Mat4>& boneTransforms, double time){
	// Looping so that lower bound never reaches end()
	if(time >= duration)
		time -= duration;
	
	// Get bounding keyframes
	std::vector<double>::iterator upperTimeStamp = std::upper_bound(timeStamps.begin(), timeStamps.end(), time);

	// Time is before the animation - Fill transforms with identity
	if(upperTimeStamp == timeStamps.begin()){
		for(int i = 0; i < numBones; ++i)
			boneTransforms[i] = Mat4().initIdentity();
		return;
	}
	unsigned index = std::distance(timeStamps.begin(), upperTimeStamp);
	Keyframe& key1 = keyframes[index - 1];
	double time1 = timeStamps[index - 1];
	Keyframe& key2 = keyframes[index];
	double time2 = timeStamps[index];
	
	// For every bone, update its transform matrix
	for(int i = 0; i < numBones; ++i){
		Bone* bone = skeleton->bones[i];
		float alpha = (time - time1) / (time2 - time1);

		// Get interpolated global transformation
		Vec3 _position = lerp(key1.boneTransformations[i].position, key2.boneTransformations[i].position, alpha);
		Mat4 translation = Mat4().initTranslation(_position);

		Quaternion _rotation = slerp(key1.boneTransformations[i].rotation, key2.boneTransformations[i].rotation, alpha);
		Mat4 rotation = Mat4().initRotation(_rotation);
		
		Mat4 transformation = translation * rotation;
		boneTransforms[i] = transformation;
	}

	// Final skeleton pass to update bone with parent transform
	getFinalBoneTransformations(skeleton, boneTransforms, skeleton->root, Mat4().initIdentity());
}

void Animation::getFinalBoneTransformations(Skeleton* skeleton, std::vector<Mat4>& boneTransforms, Bone* bone, const Mat4& parentTransform){
	unsigned boneIndex = bone->index;
	
	// Get Global transformation
	Mat4 globalTransformation = parentTransform * boneTransforms[boneIndex];

	Mat4 finalTransformation = skeleton->globalInverseTransform * globalTransformation * bone->localBoneSpaceTransform;
	boneTransforms[boneIndex] = finalTransformation;
	
	// Update transformations for children
	for(Bone* child : bone->children)
		getFinalBoneTransformations(skeleton, boneTransforms, child, globalTransformation);
}

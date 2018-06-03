
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 boneIDs;
layout (location = 4) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

uniform mat4 boneTransforms[32];

out vec2 texCoord0;

void main(){
	mat4 boneTransform = boneTransforms[int(boneIDs.x)] * weights.x;
	boneTransform += boneTransforms[int(boneIDs.y)] * weights.y;
	boneTransform += boneTransforms[int(boneIDs.z)] * weights.z;
	boneTransform += boneTransforms[int(boneIDs.w)] * weights.w;
	
	gl_Position = projection * view * transform * vec4(position, 1.0);
	texCoord0 = texCoord;
}

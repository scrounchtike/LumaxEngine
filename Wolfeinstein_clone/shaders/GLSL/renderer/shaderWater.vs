
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (std140, row_major) uniform uboMVP3D{
	mat4 projection;
	mat4 view;
	mat4 transform;
};

out vec4 clipSpace0;
out vec3 worldPos0;
out vec2 texCoord0;
out vec3 normal0;
out vec3 toCameraVector;

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	worldPos0 = worldPos.xyz;
	clipSpace0 = projection * view * worldPos;
	gl_Position = clipSpace0;

	toCameraVector = normalize((inverse(view) * vec4(0,0,0,1)).xyz - worldPos.xyz);

	texCoord0 = texCoord;
	normal0 = normal;
}

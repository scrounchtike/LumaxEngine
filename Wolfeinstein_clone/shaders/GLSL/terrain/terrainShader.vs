
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 6) in float slope;

layout (std140, row_major) uniform uboMVP3D {
	mat4 projection;
	mat4 view;
	mat4 transform;
};

out vec3 worldPos0;
out vec2 texCoord0;
out vec3 normal0;
out float slope0;
out float rand0;

uniform vec4 plane;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	
	worldPos0 = worldPos.xyz;
	texCoord0 = texCoord;
	normal0 = normal;
	slope0 = slope;

	gl_ClipDistance[0] = dot(worldPos, plane);

	rand0 = rand(position.xz);
}


#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos0;

layout(row_major, std140) uniform uboMVP3D {
	mat4 projection;
	mat4 view;
	mat4 transform;
};

layout (row_major, std140) uniform uboClipPlanes {
	vec4 planes[6];
};

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	worldPos0 = worldPos.xyz;
	
	gl_Position = projection * view * worldPos;

	gl_ClipDistance[0] = dot(planes[0], worldPos);

	texCoord0 = texCoord;
	normal0 = normal;
}

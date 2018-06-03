
#version 330

layout (location = 0) in vec3 position;
//layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

out vec3 worldPos0;
out vec3 normal0;

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	worldPos0 = worldPos.xyz;
	normal0 = normal;
}

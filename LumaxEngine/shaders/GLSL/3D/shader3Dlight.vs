
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 texCoord0;
out vec3 normal0;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

void main(){
	gl_Position = projection * view * transform * vec4(position, 1.0);

	texCoord0 = texCoord;
	normal0 = normal;
}
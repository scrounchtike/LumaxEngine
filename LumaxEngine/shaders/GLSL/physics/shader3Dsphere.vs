
#version 330

layout (location = 0) in vec3 geometry;
layout (location = 1) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 position;
uniform float radius;

out vec2 texCoord0;

void main(){
	gl_Position = projection * view * vec4(geometry * radius + position, 1.0);

	texCoord0 = texCoord;
}
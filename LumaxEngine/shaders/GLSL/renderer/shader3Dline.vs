
#version 330

layout (location = 0) in vec3 geometry;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 start;
uniform vec3 end;

void main() {
	gl_Position = projection * view * vec4(start*(1.0 - geometry) + end*geometry, 1.0);
}
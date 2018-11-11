
#version 330

layout (location = 0) in vec3 geometry;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

void main(){
	gl_Position = projection * view * transform * vec4(geometry, 1.0);
}
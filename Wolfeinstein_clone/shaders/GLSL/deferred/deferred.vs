
#version 330

layout (location = 0) in vec3 geometry;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 position;
uniform float radius;

void main(){
	gl_Position = projection * view * vec4(geometry * vec3(radius) + position, 1.0);
}

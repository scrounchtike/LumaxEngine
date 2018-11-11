
#version 330

layout (location = 0) in vec2 geometry;

uniform vec2 position;
uniform vec2 extents;

void main(){
	gl_Position = vec4(geometry * extents + position, 0.0, 1.0);
}
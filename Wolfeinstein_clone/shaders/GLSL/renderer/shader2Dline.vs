
#version 330

layout (location = 0) in vec2 position;

uniform vec2 A;
uniform vec2 B;

void main(){
	gl_Position = vec4(B*position + A*(vec2(1.0) - position), 0.0, 1.0);
}
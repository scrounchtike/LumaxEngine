
#version 330

uniform vec3 color;

in vec2 texCoord0;

out vec4 FragColor;

void main(){
	FragColor = vec4(color, 1.0);
}
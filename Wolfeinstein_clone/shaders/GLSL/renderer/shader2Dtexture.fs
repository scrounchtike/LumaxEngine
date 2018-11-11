
#version 330

in vec2 texCoord0;

uniform sampler2D sample;

out vec4 FragColor;

void main(){
	FragColor = texture(sample, texCoord0);
}


#version 330

uniform sampler2D sample_;

in vec2 texCoord0;

out vec4 FragColor;

void main(){
	FragColor = texture(sample_, texCoord0);
}

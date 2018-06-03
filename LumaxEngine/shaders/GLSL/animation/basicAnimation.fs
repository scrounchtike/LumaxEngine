
#version 330

uniform sampler2D sampler;

in vec2 texCoord0;

out vec4 FragColor;

void main(){
	FragColor = texture(sampler, texCoord0);
}

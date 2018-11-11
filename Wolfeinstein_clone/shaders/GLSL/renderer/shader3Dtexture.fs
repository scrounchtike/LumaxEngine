
#version 330

uniform sampler2D sample;

in vec2 texCoord0;
in vec3 normal0;

out vec4 FragColor;

struct DirectionalLight{
	vec3 direction;
	float padding;
	vec4 color;
	float intensity;
};


void main(){
	FragColor = texture(sample, texCoord0);
}

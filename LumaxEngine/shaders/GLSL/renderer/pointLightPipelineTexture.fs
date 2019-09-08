
#version 330

in vec3 worldPos0;
in vec2 texCoord0;
in vec3 normal0;

out vec4 FragColor;

struct PointLight {
	vec3 position;
	float padding1;
	vec3 color;
	float padding2;
	float exponent;
	float linear;
	float constant;
	float padding3;
};
layout (std140) uniform uboPointLight {
	PointLight plight;
};

void main(){
	vec3 brightness = vec3(0,0,0);
	
	vec3 direction = plight.position - worldPos0;
	float dist = length(direction);
	direction = normalize(direction);
	float dot1 = max(dot(normal0, direction), 0);

	float attenuation = plight.exponent*dist*dist + plight.linear*dist + plight.constant + 0.00001;
	brightness += plight.color * dot1 / attenuation;
	
	FragColor = vec4(brightness, 1.0);
}

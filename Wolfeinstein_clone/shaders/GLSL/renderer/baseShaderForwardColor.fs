
#version 330

layout (std140) uniform uboColor {
	vec4 color;
};

in vec2 texCoord0;
in vec3 normal0;

out vec4 FragColor;

const float ambient = 0.3;

struct DirectionalLight{
	vec3 direction;
	float padding;
	vec3 color;
	float intensity;
};
layout (std140) uniform uboDirectionalLight{
	DirectionalLight dlight;
};

void main(){
	vec3 brightness = vec3(ambient,ambient,ambient);

	float dot1 = max(dot(normal0, dlight.direction), 0);
	brightness += dlight.color * dlight.intensity * dot1;
	
	FragColor = color * vec4(brightness, 1.0);
}

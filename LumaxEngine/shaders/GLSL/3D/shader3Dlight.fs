
#version 330

uniform sampler2D sample;

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};
uniform DirectionalLight dLight;

in vec2 texCoord0;
in vec3 normal0;

out vec4 FragColor;

void main(){
	vec4 textureColor = texture(sample, texCoord0); // Texture color

	vec3 brightness = vec3(0,0,0); // Ambient lighting

	vec3 direction = normalize(vec3(1,1,1));
	vec3 color = vec3(1,1,1);
	float intensity = 1.0;

	//float dot1 = max(dot(normal0, -direction), 0.0);
	//brightness += intensity * dot1 * color; // Directional Lighting

	FragColor = textureColor * vec4(brightness, 1.0); // Texture color * lighting
	//FragColor = vec4(1,0,1,1);
}
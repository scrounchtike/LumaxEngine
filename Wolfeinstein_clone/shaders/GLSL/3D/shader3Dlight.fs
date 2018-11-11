
#version 330

uniform sampler2D sample;

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};
struct PointLight {
	vec3 position;
	vec3 color;
	// Attenuation
	float constant;
	float linear;
	float exponent;
};
struct SpotLight {
	PointLight pointlight;
	vec3 direction;
	float cutoff;
};
uniform DirectionalLight dlights[4];
uniform PointLight plights[4];
uniform SpotLight slights[4];

uniform vec3 cameraPos;

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;

out vec4 FragColor;

// constanst - Will be replaced by uniforms later
const float specularIntensity = 2.0;
const float specularPower = 32.0;

vec3 calcDirectionalLightBrightness(DirectionalLight light, vec3 viewDir){
	vec3 brightness = vec3(0,0,0);

	float diffuse_factor = max(dot(light.direction, normal0), 0.0);
	brightness += diffuse_factor * light.color * light.intensity;
	
	float specular_factor = pow(max(dot(-viewDir, light.direction), 0.0), specularPower);
	brightness += specular_factor * light.color * specularIntensity;
	
	return brightness;
}

vec3 calcPointLightBrightness(PointLight light, vec3 viewDir){
	vec3 brightness = vec3(0,0,0);

	vec3 lightDir = light.position - worldPos0;
	float sqDist = dot(lightDir, lightDir);
	float diffuse_factor = max(dot(normalize(lightDir), normal0), 0.0);
	float attenuation = 1.0f / (light.exponent*light.exponent*sqDist + light.linear*sqDist + light.constant + 0.00001);
	brightness += diffuse_factor * light.color * attenuation;
	
	return brightness;
}

vec3 calcSpotLightBrightness(SpotLight light, vec3 viewDir){
	vec3 brightness = vec3(0,0,0);
	
  brightness += calcPointLightBrightness(light.pointlight, viewDir);
	vec3 lightDir = light.pointlight.position - worldPos0;
	float theta = dot(normalize(lightDir), light.direction);
	float epsilon = light.cutoff - 0.6;
	float intensity = clamp((theta - 0.6) / epsilon, 0.0, 1.0);
	brightness *= intensity;
	
	return brightness;
}

void main(){
	vec4 textureColor = texture(sample, texCoord0); // Texture color
	vec3 brightness = vec3(0.3, 0.3, 0.3);

	vec3 viewDir = cameraPos - worldPos0;
	viewDir = reflect(normalize(viewDir), normal0);
	
	for(int i = 0; i < 4; ++i){
		brightness += calcDirectionalLightBrightness(dlights[i], viewDir);
	}
	for(int i = 0; i < 4; ++i){
		brightness += calcPointLightBrightness(plights[i], viewDir);
	}
	for(int i = 0; i < 4; ++i){
		brightness += calcSpotLightBrightness(slights[i], viewDir);
	}
	
	FragColor = textureColor * vec4(brightness, 1.0); // Texture color * lighting
}

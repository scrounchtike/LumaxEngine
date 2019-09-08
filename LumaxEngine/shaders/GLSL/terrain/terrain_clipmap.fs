
#version 410

in vec3 worldPos0;
in vec2 texCoord0;
out vec4 FragColor;

uniform vec3 cameraPos;

//
// Texturing
//

uniform sampler2D texture0;

//
// Lighting
//

uniform sampler2D normalMap;

struct DirectionalLight {
	vec3 direction;
	float padding;
	vec3 color;
	float intensity;
};
struct PointLight {
	vec3 position;
	float padding1;
	vec3 color;
	float padding2;
	float constant;
	float linear;
	float exponent;
	float radius;
};
struct SpotLight {
	PointLight pointlight;
	vec3 direction;
	float cutoff;
};

// Light UBOs
layout (std140) uniform uboDirectionalLight
{
	DirectionalLight dlight;
};
layout (std140) uniform uboPointLight
{
	PointLight plights[4];
};
layout (std140) uniform uboSpotLight
{
	SpotLight slights[4];
};

// Lighting functions

const float ambient = 0.3;
const float specularIntensity = 1.0;
const float specularPower = 32.0;

vec3 calcDirectionalLightBrightness(DirectionalLight light, vec3 viewDir, vec3 normal){
	vec3 brightness = vec3(0,0,0);

	float diffuse_factor = max(dot(light.direction, normal), 0.0);
	brightness += diffuse_factor * light.color * light.intensity;
	
	float specular_factor = pow(max(dot(-viewDir, light.direction), 0.0), specularPower);
	brightness += specular_factor * light.color * specularIntensity;
	
	return brightness;
}

vec3 calcPointLightBrightness(PointLight light, vec3 viewDir, vec3 normal){
	vec3 lightDir = light.position - worldPos0;
	float sqDist = dot(lightDir, lightDir);
	float diffuse_factor = max(dot(normalize(lightDir), normal), 0.0);
	float attenuation = 1.0f / (light.exponent*light.exponent*sqDist + light.linear*sqDist + light.constant + 0.00001);
	vec3 brightness = diffuse_factor * light.color * attenuation;
	
	return brightness;
}

vec3 calcSpotLightBrightness(SpotLight light, vec3 viewDir, vec3 normal){
	vec3 brightness = vec3(0,0,0);
	
  brightness += calcPointLightBrightness(light.pointlight, viewDir, normal);
	vec3 lightDir = light.pointlight.position - worldPos0;
	float theta = dot(normalize(lightDir), -light.direction);
	float epsilon = light.cutoff - 0.6;
	float intensity = clamp((theta - 0.6) / epsilon, 0.0, 1.0);
	
	return brightness * intensity;
}

void main()
{
	vec4 albedo = texture(texture0, texCoord0);
	vec3 brightness = vec3(ambient);

	// Sample normal map
	vec3 normal = normalize(vec3(0,1,1));
	
	vec3 viewDir = cameraPos - worldPos0;
	viewDir = reflect(normalize(viewDir), normal);

	// Directional light
	brightness += calcDirectionalLightBrightness(dlight, viewDir, normal);
	
	// Point lights
	for(int i = 0; i < 4; ++i)
	{
		if(plights[i].radius != 0.0)
			brightness += calcPointLightBrightness(plights[i], viewDir, normal);
	}
	
	// Spot lights
	for(int i = 0; i < 4; ++i)
	{
		if(slights[i].pointlight.radius != 0.0)
			brightness += calcSpotLightBrightness(slights[i], viewDir, normal);
	}
	
	FragColor = albedo * vec4(brightness, 1.0);
}

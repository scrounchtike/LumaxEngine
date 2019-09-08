
#version 410

in vec3 worldPos0;
in vec2 texCoord0;
in vec3 normal0;
in float slope0;
in float rand0;

uniform sampler2D grass;
uniform sampler2D dirt;
uniform sampler2D rocks;
uniform sampler2D sand;

uniform vec3 cameraPos;

const float rockiness = 2.0;

out vec4 FragColor;

// Lighting
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
layout (std140) uniform uboDirectionalLight {
	DirectionalLight dlight;
};
layout (std140) uniform uboPointLight {
	PointLight plights[4];
};
layout (std140) uniform uboSpotLight {
	SpotLight slights[4];
};

//
// Lighting functions
//

const float ambient = 0.3;
const float specularIntensity = 1.0;
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
	vec3 lightDir = light.position - worldPos0;
	float sqDist = dot(lightDir, lightDir);
	float diffuse_factor = max(dot(normalize(lightDir), normal0), 0.0);
	float attenuation = 1.0f / (light.exponent*light.exponent*sqDist + light.linear*sqDist + light.constant + 0.00001);
	vec3 brightness = diffuse_factor * light.color * attenuation;
	
	return brightness;
}

vec3 calcSpotLightBrightness(SpotLight light, vec3 viewDir){
	vec3 brightness = vec3(0,0,0);
	
  brightness += calcPointLightBrightness(light.pointlight, viewDir);
	vec3 lightDir = light.pointlight.position - worldPos0;
	float theta = dot(normalize(lightDir), -light.direction);
	float epsilon = light.cutoff - 0.6;
	float intensity = clamp((theta - 0.6) / epsilon, 0.0, 1.0);
	
	return brightness * intensity;
}


void main(){
	vec4 grassColor = texture(grass, texCoord0);
	vec4 dirtColor = texture(dirt, texCoord0);
	
	float depth = 0;
	if(worldPos0.y < 0){
		depth = -worldPos0.y / 10.0;
	}
	vec4 textureColor = mix(grassColor, dirtColor, clamp(depth, 0.0, 1.0));
	
	vec4 rockColor = texture(rocks, texCoord0);
	textureColor = mix(textureColor, rockColor, clamp(slope0 * rockiness, 0.0, 1.0));
	//textureColor = vec4(slope0, slope0, slope0, 1.0);
	
	vec3 brightness = vec3(0.1, 0.1, 0.1);

	vec3 viewDir = cameraPos - worldPos0;
	viewDir = reflect(normalize(viewDir), normal0);

	// Directional light
	brightness += calcDirectionalLightBrightness(dlight, viewDir);
	
	// Point lights
	for(int i = 0; i < 4; ++i)
	{
		if(plights[i].radius != 0.0)
			brightness += calcPointLightBrightness(plights[i], viewDir);
	}
	
	// Spot lights
	for(int i = 0; i < 4; ++i)
	{
		if(slights[i].pointlight.radius != 0.0)
			brightness += calcSpotLightBrightness(slights[i], viewDir);
	}
	
	FragColor = textureColor * vec4(brightness, 1.0);
}

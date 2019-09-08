
#version 410

uniform sampler2D texSample;
uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

layout (std140) uniform uboColor {
	vec4 color;
};
uniform float blend;

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;

out vec4 FragColor;

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

uniform vec3 cameraPos;

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

//
// Subroutine for getting albedo color of fragment (color, texture or both)
//

subroutine vec4 subroutineFinalColor();

subroutine(subroutineFinalColor) vec4 getColor()
{
	return color;
}

subroutine(subroutineFinalColor) vec4 getTextureColor()
{
	return texture(texSample, texCoord0);
}

subroutine(subroutineFinalColor) vec4 getBlendedColor()
{
	return mix(texture(texSample, texCoord0), color, blend);
}

subroutine uniform subroutineFinalColor getFinalColor;


void main(){
	vec4 albedo = getFinalColor();
	albedo = texture(refraction_texture, texCoord0);
	vec3 brightness = vec3(ambient);
	
	vec3 viewDir = cameraPos - worldPos0;
	viewDir = reflect(normalize(viewDir), normal0);
	
	//
	// Brightness calculation
	//

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
	
	FragColor = albedo * vec4(brightness, 1.0);
}

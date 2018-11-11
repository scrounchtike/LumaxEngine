
#version 410

in vec2 texCoord0;
in vec3 normal0;
in vec3 brightness0;
in float ambient0;
in vec3 worldPos0;

uniform sampler2D texSample;
layout (std140) uniform uboColor {
	vec3 color;
	float alpha;
};
uniform float blend;

uniform vec3 cameraPos;

struct DirectionalLight {
	vec3 direction;
	float padding1;
	vec3 color;
	float intensity;
};
struct PointLight {
	vec3 position;
	float padding1;
	vec3 color;
	float padding2;
	float exponent;
	float linear;
	float constant;
	float radius;
};
struct SpotLight {
	PointLight plight;
	vec3 direction;
	float cutoff;
};
layout (std140) uniform uboLights {
	DirectionalLight dlight;
	PointLight plights[4];
	SpotLight slights[4];
};

subroutine vec4 FinalColorSubroutine();
subroutine uniform FinalColorSubroutine getAlbedo;

subroutine(FinalColorSubroutine)
vec4 getBlendedColor(){
	vec4 textureColor = texture(texSample, texCoord0);
	return textureColor*blend + vec4(color,1.0)*(1.0-blend);
}
subroutine(FinalColorSubroutine)
vec4 getTexturedColor(){
	return texture(texSample, texCoord0);
}
subroutine(FinalColorSubroutine)
vec4 getColor(){
	return vec4(color, 1.0);
}
																 
out vec4 FragColor;

const float specularFactor = 0.8;

void main(){
	vec4 albedo = getAlbedo();

	vec3 brightness = vec3(0,0,0);

	vec3 toCamera = cameraPos - worldPos0;
	toCamera = normalize(toCamera);
	
	// Directional Light
	float diffuse = max(dot(normal0, -dlight.direction), 0.0);
	
	vec3 reflected = reflect(dlight.direction, normal0);
	float specular = specularFactor * pow(max(dot(reflected, toCamera), 0.0), 32);
	float lightFactor = diffuse + specular;

	brightness += dlight.color * dlight.intensity * lightFactor;
	
	// Point Lights
	for(int i = 0; i < 4; ++i){
		vec3 toLight = plights[i].position - worldPos0;
		float dist = length(toLight);
		toLight = normalize(toLight);
		// Diffuse
		float diffuse = max(dot(normal0, toLight), 0.0);
		// Specular
		vec3 reflected = reflect(-toLight, normal0);
		float specular = specularFactor * pow(max(dot(reflected, toCamera), 0.0), 32);
		float lightFactor = diffuse + specular;
		
		float attenuation = plights[i].exponent*dist*dist + plights[i].linear*dist + plights[i].constant + 0.000001;
		brightness += plights[i].color * lightFactor / attenuation;
	}
	
	// Spot Lights
	
	
	FragColor = albedo * vec4(brightness0 + brightness, 1.0);
}

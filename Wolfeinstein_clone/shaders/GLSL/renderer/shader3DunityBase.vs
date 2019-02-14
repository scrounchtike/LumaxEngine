
#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform vec3 cameraPos;

layout (std140, row_major) uniform uboMVP3D{
	mat4 projection;
	mat4 view;
	mat4 transform;
};

struct PointLight{
	vec3 position;
	float padding1;
	vec3 color;
	float padding2;
	float exponent;
	float linear;
	float constant;
	float radius;
};
struct SpotLight{
	PointLight plight;
	vec3 direction;
	float cutoff;
};
layout (std140, row_major) uniform uboLightsVertex{
	PointLight pointLights[4];
	SpotLight spotLights[4];
};

out vec3 brightness;
out vec3 toCameraVector;
out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos0;

const float ambient = 0.3;

vec3 calcPointLight(vec3 normal, vec3 worldPos, PointLight plight){
	vec3 brightness = vec3(0,0,0);
	
	vec3 toLight = plight.position - worldPos;
	float dist = length(toLight);
	toLight /= dist;
	float attenuation = dist*dist*plight.exponent + dist*plight.linear + plight.constant + 0.0001;
	
	// Diffuse
	float dot1 = max(dot(normal, toLight), 0.0);
	// Specular
	vec3 reflected = reflect(-toLight, normal);
	float dot2 = max(dot(toCameraVector, toLight), 0.0);
	float specular = pow(dot2, 32.0);
	brightness += plight.color * (dot1 + specular) / attenuation;
	
	return brightness;
	
}

vec3 calcSpotLight(vec3 normal, vec3 worldPos, SpotLight slight){
	vec3 brightness = vec3(0,0,0);
	
	return brightness;
}

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	worldPos0 = worldPos.xyz;
	
	toCameraVector = normalize((inverse(view) * vec4(0,0,0,1)).xyz - worldPos.xyz);

	brightness = vec3(ambient);
	
	// Per-pixel lighting
	for(int i = 0; i < 4; ++i){
		PointLight plight = pointLights[i];
		vec3 brightness0 = calcPointLight(normal, worldPos0, plight);
		brightness += brightness0;
	}
	for(int i = 0; i < 4; ++i){
		SpotLight slight = spotLights[i];
		vec3 brightness0 = calcSpotLight(normal, worldPos0, slight);
		brightness += brightness0;
	}

	texCoord0 = texCoord;
	normal0 = normal;
}


#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (row_major,std140) uniform uboMVP3D {
	mat4 projection;
	mat4 view;
	mat4 transform;
};

uniform vec3 cameraPos;

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
layout (std140) uniform uboLightsVertex {
	PointLight plights[4];
	SpotLight slights[4];
};

out vec2 texCoord0;
out vec3 normal0;
out vec3 brightness0;
out float ambient0;
out vec3 worldPos0;

const float ambient = 0.3;

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	worldPos0 = worldPos.xyz;

	texCoord0 = texCoord;
	normal0 = normal;

	vec3 brightness = vec3(ambient,ambient,ambient);

	// Common variables
	vec3 toCamera = cameraPos - worldPos0;
	toCamera = normalize(toCamera);
	
	// Point Lights
	for(int i = 0; i < 4; ++i){
		vec3 toLight = plights[i].position - worldPos0;
		float dist = length(toLight);
		toLight = normalize(toLight);
		// Diffuse
		float diffuse = max(dot(normal, toLight), 0.0);
		// Specular
		vec3 reflected = reflect(-toLight, normal);
		float specular = 0.8 * pow(max(dot(reflected, toCamera), 0.0), 32);
		float lightFactor = diffuse + specular;
		
		float attenuation = plights[i].exponent*dist*dist + plights[i].linear*dist + plights[i].constant + 0.000001;
		brightness += plights[i].color * lightFactor / attenuation;
	}

	// Spot Lights
	for(int i = 0; i < 4; ++i){
		
	}

	brightness0 = brightness;
	ambient0 = ambient;
}

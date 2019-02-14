
#version 410

uniform sampler2D sampleTexture;
uniform float blend;
uniform vec3 cameraPos;

out vec4 FragColor;

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;
in vec3 toCameraVector;
in vec3 brightness;

struct DirectionalLight{
	vec3 direction;
	float padding;
	vec3 color;
	float intensity;
};
layout (std140, row_major) uniform uboDirectionalLight{
	DirectionalLight dlight;
};

layout (std140) uniform uboColor{
	vec3 color;
	float alpha;
};

// Subroutines for albedo
subroutine vec4 subroutineFinalColor();
subroutine(subroutineFinalColor) vec4 getColor(){
	return vec4(color, 1.0);
}
subroutine(subroutineFinalColor) vec4 getTextureColor(){
	return texture(sampleTexture, texCoord0);
}
subroutine(subroutineFinalColor) vec4 getBlendedColor(){
	return texture(sampleTexture, texCoord0) * blend + vec4(color, 1.0) * (1.0 - blend);
}
// Actual subroutine uniform
subroutine uniform subroutineFinalColor getAlbedo;

void main(){
	vec4 albedo = getAlbedo();

	// Directional light
	float dot1 = max(dot(normal0, dlight.direction), 0.0);
	vec3 reflected = reflect(-dlight.direction, normal0);
	float dot2 = max(dot(reflected, toCameraVector), 0.0);
	float specular = pow(dot2, 32.0);
	vec3 lighting = brightness + dlight.color * dlight.intensity * (dot1 + specular);

	FragColor = albedo * vec4(lighting, 1.0);
}

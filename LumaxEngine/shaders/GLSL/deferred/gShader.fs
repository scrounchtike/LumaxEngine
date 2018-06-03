
#version 330

layout (location = 0) out vec3 gAlbedo;
layout (location = 1) out vec3 gNormal;

uniform sampler2D sampler;

in vec2 texCoord0;
in vec3 normal0;

void main(){
	vec4 textureColor = texture(sampler, texCoord0);
	
	gAlbedo = textureColor.xyz;
	gNormal = normal0;
}

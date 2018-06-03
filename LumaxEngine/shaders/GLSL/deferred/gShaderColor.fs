
#version 330

layout (location = 0) out vec3 gWorldPos;
layout (location = 1) out vec3 gAlbedo;
layout (location = 2) out vec3 gNormal;

in vec3 worldPos0;
in vec3 normal0;

uniform vec3 color;

void main(){
	gWorldPos = worldPos0;
	gAlbedo = color;
	gNormal = normal0;
}

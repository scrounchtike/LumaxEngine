
#version 330

uniform samplerCube cubeMap;

in vec3 vecCoord0;

out vec4 FragColor;

void main(){
	FragColor = texture(cubeMap, vecCoord0);
	//FragColor = vec4(1,0,0,1);
}

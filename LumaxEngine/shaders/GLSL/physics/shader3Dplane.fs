
#version 330

uniform vec3 color;
//uniform sampler2D sample;
//uniform float isTextured;

in vec2 texCoord0;

out vec4 FragColor;

void main(){
	vec4 finalColor = vec4(color, 1.0);
	//if(isTextured){
	//	finalColor = texture(sample, texCoord0);
	//}
	FragColor = finalColor;
}
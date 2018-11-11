
#version 330

uniform sampler2D depth;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D position;

in vec2 texCoord0;

out vec4 FragColor;

void main(){
	FragColor = vec4(1,0,0,1);
}

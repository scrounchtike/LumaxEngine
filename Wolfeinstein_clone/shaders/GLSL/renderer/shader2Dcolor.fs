
#version 330

layout (std140) uniform uboColor {
	vec4 color;
};

out vec4 FragColor;

void main(){
	FragColor = color;
}

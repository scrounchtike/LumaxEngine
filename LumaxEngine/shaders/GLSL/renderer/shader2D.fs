
#version 410

layout (std140) uniform uboColor {
	vec4 color;
};

in vec2 texCoord0;

out vec4 FragColor;

uniform sampler2D tex;
uniform float blend;

subroutine vec4 subroutineFinalColor();

subroutine(subroutineFinalColor) vec4 getColor() {
	return color;
}

subroutine(subroutineFinalColor) vec4 getTextureColor() {
	return texture(tex, texCoord0);
}

subroutine(subroutineFinalColor) vec4 getBlendedColor() {
	return texture(tex, texCoord0) * blend + color * (1.0 - blend);
}

subroutine uniform subroutineFinalColor getFinalColor;

void main(){
	FragColor = getFinalColor();
}


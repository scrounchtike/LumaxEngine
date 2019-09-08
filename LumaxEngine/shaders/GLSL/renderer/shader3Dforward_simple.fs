
#version 410

uniform sampler2D texSample;

layout (std140) uniform uboColor {
	vec4 color;
};
uniform float blend;

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;

out vec4 FragColor;

uniform vec3 cameraPos;

subroutine vec4 subroutineFinalColor();

subroutine(subroutineFinalColor) vec4 getColor()
{
	return color;
}
subroutine(subroutineFinalColor) vec4 getBlendedColor()
{
	return mix(texture(texSample, texCoord0), color, blend);
}
subroutine(subroutineFinalColor) vec4 getTextureColor()
{
	return texture(texSample, texCoord0);
}

subroutine uniform subroutineFinalColor getFinalColor;

void main()
{
	vec4 albedo = getFinalColor();
	
	FragColor = albedo;
}

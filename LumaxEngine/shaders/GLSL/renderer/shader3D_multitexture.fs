
#version 410

uniform sampler2D texSample;
uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

in vec2 texCoord0;

out vec4 FragColor;

void main()
{
	FragColor = texture(texSample, texCoord0);
}

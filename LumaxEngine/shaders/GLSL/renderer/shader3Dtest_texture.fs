
#version 330

uniform sampler2D texSample;
uniform sampler2D texSample2;
uniform sampler2D texSample3;

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;

out vec4 FragColor;

uniform vec3 cameraPos;

void main()
{
	FragColor = texture(texSample, texCoord0);
}


#version 330

in vec3 worldPos0;
in vec2 texCoord0;
in vec3 normal0;
in float slope0;
in float rand0;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

layout (std140, row_major) uniform uboColor {
	vec4 color;
};

uniform vec3 cameraPos;

const float rockiness = 2.0;

out vec4 FragColor;

void main(){
	vec4 grassColor = texture(texture1, texCoord0);
	vec4 dirtColor = texture(texture2, texCoord0);
	
	float depth = 0;
	if(worldPos0.y < 0){
		depth = -worldPos0.y / 10.0;
	}
	vec4 textureColor = mix(grassColor, dirtColor, clamp(depth, 0.0, 1.0));
	
	vec4 rockColor = texture(texture3, texCoord0);
	textureColor = mix(textureColor, rockColor, clamp(slope0 * rockiness, 0.0, 1.0));
	//textureColor = vec4(slope0, slope0, slope0, 1.0);
	
	vec3 brightness = vec3(0.3, 0.3, 0.3);
	
	float dot1 = max(dot(normal0, normalize(vec3(-2,1,-1))), 0.0);
	brightness += vec3(1,1,1) * 1 * dot1;
	
	vec3 light = vec3(50,10,50);
	vec3 toLight = light - worldPos0;
	float dist = length(toLight);
	toLight = normalize(toLight);
	float dot2 = max(dot(normal0, toLight), 0.0);
	float exponent = 0.05;
	float linear = 0.1;
	float constant = 0.2;
	float attenuation = 1.0 / (dist*exponent*exponent + dist*linear + constant + 0.0001);
	//brightness += vec3(1,0,0) * 1.0 * dot2 * attenuation;
	
	FragColor = textureColor * vec4(brightness, 1.0);
	//FragColor = vec4(0,1,0,1);

	//FragColor = color;
}

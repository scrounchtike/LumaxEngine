
#version 330

uniform sampler2D texAlbedo;
uniform sampler2D texNormal;
uniform sampler2D texDisp;

in vec3 worldPos0;
in vec2 texCoord0;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 cameraPos;

const float reflectivity = 0.8f;
const int   dampFactor = 4;

const float exponent = 0.5f;
const float linear = 0.05f;
const float constant = 0.01f;

vec3 calcDirectionalLight(vec3 lightColor, vec3 normal, vec3 direction, vec3 toCameraVector, float intensity){
	float dot1 = dot(normal, direction);
	vec3 brightness = vec3(0,0,0);
	if(dot1 > 0){
		brightness += lightColor * dot1 * intensity;
		vec3 reflected = reflect(-direction, normal);
		float dot2 = dot(reflected, toCameraVector);
		if(dot2 > 0){
			float specularFactor = pow(dot2, dampFactor) * reflectivity;
			brightness += lightColor * specularFactor * intensity;
		}
	}
	return brightness;
}

in vec3 toCameraVector;

void main(){
	//vec3 normal = normalize(TBN * (255.0 / 128.0 * texture(texNormal, texCoord0).xyz - 1));
	//vec3 normal = vec3(0,1,0);
	
	vec2 texCoords = texCoord0 + (toCameraVector * TBN).xy * (texture(texDisp, texCoord0).r * 0.1);

	//float height = texture(texDisp, texCoord0).r;
	//vec2 p = (toCameraVector * TBN).xy * (height * 0.1);
	//vec2 texCoords = texCoord0 - p;

	vec3 normal = normalize(TBN * (255.0 / 128.0 * texture(texNormal, texCoords).xyz - 1));
	vec4 textureColor = texture(texAlbedo, texCoords);
	
	vec3 brightness = vec3(0.3, 0.3, 0.3);
	brightness += calcDirectionalLight(vec3(1,1,1), normal, normalize(vec3(-1,1,-1)), toCameraVector, 0.5);

	FragColor = textureColor * vec4(brightness, 1.0);
}

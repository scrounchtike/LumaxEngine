
#version 330

out vec4 FragColor;

in vec4 clipSpace0;
in vec3 worldPos0;
in vec2 texCoord0;
in vec3 normal0;

in vec3 toCameraVector;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

layout (std140, row_major) uniform uboWater{
	float waveStrength;
	float moveFactor;
	float shineDamper;
	float reflectivity;
};

void main(){
	vec2 ndc = (clipSpace0.xy / clipSpace0.w);
	ndc = ndc/2.0 + 0.5;
	
	vec2 refractTexCoords = ndc.xy;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	
	vec2 distortedTexCoords = texture(dudvMap, vec2(texCoord0.x + moveFactor, texCoord0.y)).xy * 0.1;
	distortedTexCoords = texCoord0 + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 distortion = (texture(dudvMap, distortedTexCoords).xy * 2.0 - 1.0) * waveStrength;// * clamp(waterDepth / 20.0, 0.0, 1.0);

	distortion = distortion / 10.0;
	
	refractTexCoords += distortion;
	reflectTexCoords += distortion;
	
	vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColor = texture(refractionTexture, refractTexCoords);
	
	vec4 normalMapColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
	normalize(normal);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, normalize(vec3(normal.x, normal.y * 3.0, normal.z)));
	refractiveFactor = pow(refractiveFactor, 1);
	refractiveFactor = clamp(refractiveFactor, 0.0, 1.0) / 1.3;
	
	vec3 reflectedLight = reflect(normalize(vec3(-1,1,-1)), normal);
	float specular = dot(reflectedLight, viewVector);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = vec3(1,1,1) * specular * reflectivity;// * clamp(waterDepth / 5.0, 0.0, 1.0);
	
	//float refractiveFactor = 0.5;
	//FragColor = mix(reflectionColor, refractionColor, refractiveFactor);
	FragColor = mix(reflectionColor, refractionColor, refractiveFactor);
	FragColor = mix(FragColor, vec4(0, 0.3, 0.6, 1.0), 0.3) + vec4(specularHighlights, 1.0);
	
	FragColor.a = 1.0;
}

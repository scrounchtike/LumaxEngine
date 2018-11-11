#version 330
//game

const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 4;

in vec2 texCoord0;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;
in float visibility;
in vec4 worldPos0;

uniform sampler2D sample;

uniform float ambient;
uniform vec3 directionalColor;

uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColor;

out vec4 FragColor;

struct Attenuation{
	float constant;
	float linear;
	float exponent;

};

struct PointLight{
	vec3 color;
	vec3 position;
	mat4 translation;
	float range;
	Attenuation attenuation;

};

struct SpotLight{
	PointLight pointLight;
	vec3 direction;
	float cutoff;

};

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec3 calcLight(vec3 color, vec3 normal, vec3 direction){
	vec3 diffuseColor = vec3(0, 0, 0);
	vec3 specularColor = vec3(0, 0, 0);
	float nDot1 = dot(normal, direction);
	
	if(nDot1 > 0){
		diffuseColor = color * nDot1;
	
		vec3 reflectedDirection = reflect(direction, normal);
		float nDot2 = dot(toCameraVector, reflectedDirection);
		float specularFactor = pow(nDot2, shineDamper);
	
		if(specularFactor > 0){
			specularColor = color * reflectivity * specularFactor;
	
		}
		
	}
	
	return diffuseColor + specularColor;

}

vec3 calcPointLight(vec3 normal, PointLight pointLight){
	vec3 color = vec3(0, 0, 0);

	vec3 translatedPosition = (vec4(pointLight.position, 1.0) * pointLight.translation).xyz;
	vec3 toPointLight = translatedPosition - worldPos0.xyz;
	float distanceToPoint = length(toPointLight);
	
	if(distanceToPoint > pointLight.range){
		return vec3(0, 0, 0);
	
	}
	
	toPointLight = normalize(toPointLight);
		
	color = calcLight(pointLight.color, normal, toPointLight);
	
	float attenuation = pointLight.attenuation.constant +
						pointLight.attenuation.linear * distanceToPoint +
						pointLight.attenuation.exponent * 
						distanceToPoint * distanceToPoint + 0.0001;
						
	return color / attenuation;

}

vec3 calcSpotLight(vec3 normal, SpotLight spotLight){
	vec3 lightDirection = spotLight.pointLight.position - worldPos0.xyz;
	lightDirection = normalize(lightDirection);
	
	float spotFactor = dot(lightDirection, spotLight.direction);
	//float spotFactor = dot(lightDirection, normalize(vec3(0, 0, 1)));
	
	vec3 color = vec3(0, 0, 0);
	
	if(spotFactor > spotLight.cutoff){
		//pixel within the spotlight's cone
		color = calcPointLight(normal, spotLight.pointLight) * 
							   (1.0 - (1.0 - spotFactor) / (1.0 - spotLight.cutoff));
	
	}
	
	return color;

}

void main(){
	//Calculate lighting	
	vec3 brightness = vec3(ambient, ambient, ambient);
	
	//Directional light
	brightness += calcLight(directionalColor, surfaceNormal, toLightVector);
	
	//Point lights
	for(int i = 0; i < MAX_POINT_LIGHTS; i++){
		if(pointLights[i].color != vec3(0, 0, 0)){
			brightness += calcPointLight(surfaceNormal, pointLights[i]);
		
		}
	
	}
	
	//Spot lights
	for(int i = 0; i < MAX_SPOT_LIGHTS; i++){
		if(spotLights[i].pointLight.color != vec3(0, 0, 0)){
			brightness += calcSpotLight(surfaceNormal, spotLights[i]);
		
		}
	
	}
	
	vec4 textureColor = texture(sample, texCoord0);
	if(textureColor.a < 0.5){
		discard;
	
	}
	
	//FragColor = vec4(diffuse, 1.0) * textureColor + vec4(finalSpecular, 1.0);
	//FragColor = mix(vec4(skyColor, 1.0), FragColor, visibility);
	FragColor = textureColor * vec4(brightness, 1.0);

}

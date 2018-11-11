
#version 330

uniform sampler2D texture1;

in vec3 worldPos0;
in vec2 texCoord0;
in vec3 normal0;

in vec3 toCameraVector;

out vec4 FragColor;

void main(){
	vec4 textureColor = texture(texture1, texCoord0);
	
	vec3 brightness = vec3(0.3,0.3,0.3);
	
	vec3 direction = normalize(vec3(0,1,1));
	float dot1 = max(dot(normal0, direction), 0.0);
	brightness += vec3(1,1,1) * dot1 * 0.05;
	vec3 reflected = reflect(-direction, normal0);
	float dot2 = max(dot(toCameraVector, reflected), 0.0);
	float specular = pow(dot2, 32);
	brightness += vec3(1,1,1) * specular * 0.1;
	
	{
		vec3 plight = vec3(-3,25,2);
		vec3 direction = plight - worldPos0;
		float dist = length(direction);
		direction = normalize(direction);
		float dot1 = max(dot(normal0, direction), 0.0);
		vec3 lighting = vec3(1,0,1) * dot1 * 0.5;
		vec3 reflected = reflect(-direction, normal0);
		float dot2 = max(dot(toCameraVector, reflected), 0.0);
		float specular = pow(dot2, 32);
		lighting += vec3(1,0,1) * specular * 0.6;
		float exponent = 0.1;
		float linear = 0.1;
		float constant = 0.15;
		float attenuation = exponent*dist*dist + linear*dist + constant + 0.0001;
		lighting = lighting / attenuation;
		brightness += lighting;
	}

	{
		vec3 slight = vec3(2,26,2);
		vec3 sdir = normalize(vec3(0,0,-1));
		vec3 direction = slight - worldPos0;
		float dist = length(direction);
		direction = normalize(direction);
		
		float spotFactor = dot(sdir, -direction);

		float cutoff = 0.8;
		float spotCoeff = 0.0;
		if(spotFactor > cutoff)
			spotCoeff = min(2.0 * (1.0 - (1.0 - spotFactor) / (1.0 - cutoff)), 1.0);
		
		float dot1 = max(dot(normal0, direction), 0.0);
		vec3 lighting = vec3(0,1,0) * dot1 * 0.5;
		vec3 reflected = reflect(-direction, normal0);
		float dot2 = max(dot(toCameraVector, reflected), 0.0);
		float specular = pow(dot2, 32);
		lighting += vec3(0,1,0) * specular * 0.6;
		float exponent = 0.1;
		float linear = 0.1;
		float constant = 0.15;
		float attenuation = exponent*dist*dist + linear*dist + constant + 0.0001;
		lighting = lighting / attenuation;
		brightness += spotCoeff * lighting;
	}
	
	FragColor = vec4(brightness * textureColor.xyz, 1.0);
	//FragColor = vec4(normal0, 1.0);
	//FragColor = vec4(worldPos0, 1.0);
}


#version 330

uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;

out vec4 FragColor;

// Lighting uniforms
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float constant;
uniform float linear;
uniform float exponent;

uniform vec2 viewport;

uniform mat4 projection;
uniform mat4 view;

void main(){
	vec2 windowPos = gl_FragCoord.xy / viewport;
	
	float depth = texture(gDepth, windowPos).r;
	float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;
	vec3 ndc;
	ndc.xy = (2.0 * windowPos) - 1;
	ndc.z = (2.0 * depth - far - near) / (far - near);

	vec4 clip;
	clip.w = projection[3][2] / (ndc.z - (projection[2][2] / projection[2][3])); // Big scary math to derive!
	clip.xyz = ndc * clip.w;
	vec4 eye = inverse(projection) * clip;
	vec4 worldPos = inverse(view) * eye;
	
	vec4 _albedo = texture(gAlbedo, windowPos);
	vec4 _normal = texture(gNormal, windowPos);
	vec3 albedo = _albedo.xyz;
	vec3 normal = _normal.xyz;

	// Perform lighting
	vec3 brightness = vec3(0, 0, 0);

	vec3 lightDir = lightPos - worldPos.xyz;
	float sqDist = dot(lightDir, lightDir);
	float diffuse = max(dot(normalize(lightDir), normal), 0.0);
	float attenuation = 1.0f / (exponent*exponent*sqDist + linear*sqDist + constant + 0.00001);
	brightness += diffuse * lightColor * attenuation;
	
	FragColor = vec4(albedo * brightness, 1.0);
}

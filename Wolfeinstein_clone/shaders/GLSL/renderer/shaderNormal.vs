
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out vec3 worldPos0;
out vec2 texCoord0;
out mat3 TBN;

layout (std140, row_major) uniform uboMVP3D{
	mat4 projection;
	mat4 view;
	mat4 transform;
};

out vec3 toCameraVector;

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	worldPos0 = worldPos.xyz;
	gl_Position = projection * view * worldPos;

	texCoord0 = texCoord;
	//vec3 bitangent = cross(tangent, normal);

	toCameraVector = normalize((inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz);

	mat4 normalTransform = transpose(inverse(transform));
	
	vec3 n = normalize((normalTransform * vec4(normal, 0.0)).xyz);
	
	//tbn Matrix:
	vec3 t = normalize((normalTransform * vec4(tangent, 0.0)).xyz);
	vec3 biTangent = cross(t, n);
	t = normalize(t - dot(t, n) * n);
	
	TBN = mat3(t, biTangent, n);

}

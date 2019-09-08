
#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

// Tangent space information
layout (location = 3) in vec3 tangent;

out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos0;
out mat3 TBN;

layout(row_major, std140) uniform uboMVP3D {
	mat4 projection;
	mat4 view;
	mat4 transform;
};

void main(){
	vec4 worldPos = transform * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	
	texCoord0 = texCoord;
	normal0 = normal;
	
	// TBN calculation
	vec3 bitangent = cross(normal, tangent);
	vec3 T = normalize(vec3(transform * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(transform * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(transform * vec4(normal, 0.0)));
	//vec3 T = tangent;
	//vec3 B = bitangent;
	//vec3 N = normal;

	//TBN = transpose(mat3(T, B, N));
	TBN = mat3(T, B, N);

	worldPos0 = worldPos.xyz;
}

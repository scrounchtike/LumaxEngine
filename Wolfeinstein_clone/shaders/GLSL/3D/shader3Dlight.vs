
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos0;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

void main(){
  vec4 worldPos = transform * vec4(position, 1.0);
	worldPos0 = worldPos.xyz;
	gl_Position = projection * view * worldPos;

	texCoord0 = texCoord;
	// TODO: Apply rotation transformation to normal
	normal0 = normal;
}

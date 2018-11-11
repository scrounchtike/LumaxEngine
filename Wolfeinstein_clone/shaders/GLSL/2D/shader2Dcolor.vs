
#version 330

layout (location = 0) in vec2 position;

uniform mat3 transform;

void main(){
	gl_Position = vec4((transform * vec3(position, 1.0)).xy, 0.0, 1.0);
}
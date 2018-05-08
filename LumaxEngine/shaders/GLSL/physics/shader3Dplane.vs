
#version 330

layout (location = 0) in vec3 geometry;
layout (location = 1) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 rotation;

uniform vec3 position;
uniform float scale;

out vec2 texCoord0;

void main(){
	gl_Position = projection * view * (vec4(position, 0.0) + (rotation * vec4(geometry * scale, 1.0)));
	//gl_Position = pos + vec4(position, 0.0);

	texCoord0 = texCoord;
}
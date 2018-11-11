
#version 330

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 texCoord0;

layout (std140, row_major) uniform uboTransform2D {
	mat4 transform;
};

void main(){
	gl_Position = transform * vec4(position, 0.0, 1.0);

	texCoord0 = texCoord;
}

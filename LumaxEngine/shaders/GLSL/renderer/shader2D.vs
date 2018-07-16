
#version 410

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

layout (row_major,std140) uniform uboTransform2D {
	mat4 transform;
};

out vec2 texCoord0;

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
};

void main(){
	gl_Position = transform * vec4(position, 0.0, 1.0);

	texCoord0 = texCoord;
}

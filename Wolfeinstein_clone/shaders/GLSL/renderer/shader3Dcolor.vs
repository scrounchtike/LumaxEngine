
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout(row_major,std140) uniform uboMVP3D {
	mat4 projection;
	mat4 view;
	mat4 transform;
};

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
};

void main(){
	gl_Position = projection * view * transform * vec4(position, 1.0);
}

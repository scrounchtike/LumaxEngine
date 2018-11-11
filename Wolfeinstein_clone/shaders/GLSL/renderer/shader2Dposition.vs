
#version 330

layout (location = 0) in vec2 position;

layout (row_major, std140) uniform uboTransform2D {
	mat4 transform;
};

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
};

void main(){
	gl_Position = transform * vec4(position, 0.0, 1.0);
}

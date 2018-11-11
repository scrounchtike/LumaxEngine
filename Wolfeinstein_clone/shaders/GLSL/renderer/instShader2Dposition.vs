
#version 410

layout (location = 0) in vec2 position;

layout (std140, row_major) uniform uboInstancedTransforms2D {
	vec4 transforms[2000];
};

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
};

void main(){
	gl_Position = vec4(position + transforms[gl_InstanceID].xy, 0.0, 1.0);
}

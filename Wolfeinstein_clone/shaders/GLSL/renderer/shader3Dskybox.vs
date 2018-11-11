
#version 330

layout (location = 0) in vec3 position;

layout (std140, row_major) uniform uboMVP3D{
	mat4 projection;
	mat4 view;
	mat4 transform;
};

out vec3 vecCoord0;

void main(){
	gl_Position = projection * view * transform * vec4(position, 1.0);
	vecCoord0 = position;
}

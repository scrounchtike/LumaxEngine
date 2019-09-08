
#version 410

layout (location = 0) in vec3 position;

uniform vec3 cameraPos;

uniform sampler2D heightmap;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

/*
layout (std140, row_major) uniform uboMVP3D
{
	mat4 projection;
	mat4 view;
	mat4 transform;
};*/

out vec3 worldPos0;
out vec2 texCoord0;

void main()
{
	vec2 xy = (transform * vec4(position, 1.0)).xz;
	float height = texelFetch(heightmap, ivec2(xy), 0).r * 100;

	vec2 uv = (xy + 0.5); // Sample in the middle of the tiles
	texCoord0 = uv / textureSize(heightmap, 0); // So that textures wrap each tile
	
	vec4 worldPos = transform * vec4(position.x, height, position.z, 1.0);
	worldPos0 = worldPos.xyz;
	gl_Position = projection * view * worldPos;
}

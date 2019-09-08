
#ifndef TERRAIN_CLIPMAP_HPP
#define TERRAIN_CLIPMAP_HPP

#include "../RL/Model3D.hpp"

#include "../math.hpp"

class TerrainClipmap
{
public:

	void initialize();

	const uint32 num_levels = 6;
	const uint32 tile_resolution = 48;
	const uint32 patch_vert_resolution = tile_resolution + 1;
	const uint32 clipmap_resolution = tile_resolution * 4 + 1;
	const uint32 clipmap_vert_resolution = clipmap_resolution + 1;
	
	Model3D* tile_mesh;
	Model3D* cross_mesh;
	Model3D* filler_mesh;
	Model3D* trim_mesh;
	Model3D* seam_mesh;

private:
	uint32 patch2D(uint32 x, uint32 z)
	{
		return z * patch_vert_resolution + x;
	}
};

#endif

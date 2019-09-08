
#include "TerrainClipmap.hpp"

void TerrainClipmap::initialize()
{
	//
	// Initialize the meshes needed for the clipmap geometry
	//

	// Code taken from :
	//http://git.mikejsavage.co.uk/medfall/file/clipmap.cc.html#l197
	// Blog : https://mikejsavage.co.uk/blog/geometry-clipmaps.html
	
	// Generate square tile mesh
	{
		std::vector<float> vertices;
		vertices.resize(patch_vert_resolution * patch_vert_resolution * 3);
		std::vector<int> indices;
		indices.resize(tile_resolution * tile_resolution * 6);
		
		uint32 n = 0;
		for(int z = 0; z < patch_vert_resolution; ++z)
		{
			for(int x = 0; x < patch_vert_resolution; ++x)
			{
				vertices[n*3+0] = x;
				vertices[n*3+1] = 0;
				vertices[n*3+2] = z;
				++n;
			}
		}

		n = 0;
		for(int z = 0; z < tile_resolution; ++z)
		{
			for(int x = 0; x < tile_resolution; ++x)
			{
				indices[n++] = patch2D(x, z);
				indices[n++] = patch2D(x+1, z+1);
				indices[n++] = patch2D(x, z+1);

				indices[n++] = patch2D(x, z);
				indices[n++] = patch2D(x+1, z);
				indices[n++] = patch2D(x+1, z+1);
			}
		}

		tile_mesh = new Model3D(vertices, indices);
	}

	// Generate empty tile mesh
	{
	}

	// Generate filler mesh
	{
		std::vector<float> vertices;
		vertices.resize(patch_vert_resolution * 8 * 3);
		std::vector<int> indices;
		indices.resize(tile_resolution * 24);
		uint32 n = 0;
		uint32 offset = tile_resolution;

		for(int i = 0; i < patch_vert_resolution; ++i)
		{
			vertices[n*3+0] = offset + i + 1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 0;
			++n;

			vertices[n*3+0] = offset + i + 1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 1;
			++n;
		}

		for(int i = 0; i < patch_vert_resolution; ++i)
		{
			vertices[n*3+0] = 1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = offset + i + 1;
			++n;

			vertices[n*3+0] = 0;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = offset + i + 1;
			++n;
		}

		for(int i = 0; i < patch_vert_resolution; ++i)
		{
			vertices[n*3+0] = -(float)(offset + i);
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 1;
			++n;

			vertices[n*3+0] = -(float)(offset + i);
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 0;
			++n;
		}

		for(int i = 0; i < patch_vert_resolution; ++i)
		{
			vertices[n*3+0] = 0;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = -(float)(offset + i);
			++n;

			vertices[n*3+0] = 1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = -(float)(offset + i);
			++n;
		}

		// Generate indices
		n = 0;
		for(int i = 0; i < tile_resolution * 4; ++i)
		{
			uint32 arm = i / tile_resolution;

			uint32 bl = (arm + i) * 2 + 0;
			uint32 br = (arm + i) * 2 + 1;
			uint32 tl = (arm + i) * 2 + 2;
			uint32 tr = (arm + i) * 2 + 3;

			if(arm % 2 == 0)
			{
				indices[n++] = br;
				indices[n++] = bl;
				indices[n++] = tr;
				indices[n++] = bl;
				indices[n++] = tl;
				indices[n++] = tr;
			}
			else
			{
				indices[n++] = br;
				indices[n++] = bl;
				indices[n++] = tl;
				indices[n++] = br;
				indices[n++] = tl;
				indices[n++] = tr;
			}
		}

		filler_mesh = new Model3D(vertices, indices);
	}

	// Generate trim mesh
	{
		std::vector<float> vertices;
		vertices.resize((clipmap_vert_resolution * 2 + 1) * 2 * 3);
		std::vector<int> indices;
		indices.resize((clipmap_vert_resolution * 2 - 1) * 6);
		uint32 n = 0;

		// Vertical part of L
		for(int i = 0; i < clipmap_vert_resolution + 1; ++i)
		{
			vertices[n*3+0] = 0;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = clipmap_vert_resolution - i;
			++n;

			vertices[n*3+0] = 1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = clipmap_vert_resolution - i;
			++n;
		}

		uint32 start_of_horizontal = n;
		// Horizontal part of L
		for(int i = 0; i < clipmap_vert_resolution; ++i)
		{
			vertices[n*3+0] = i+1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 0;
			++n;

			vertices[n*3+0] = i+1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 1;
			++n;
		}

		for(int i = 0; i < vertices.size() / 3; ++i)
		{
			vertices[i*3+0] -= 0.5f * (clipmap_vert_resolution + 1);
			vertices[i*3+2] -= 0.5f * (clipmap_vert_resolution + 1);
		}

		// Indices
		n = 0;
		for(int i = 0; i < clipmap_vert_resolution; ++i)
		{
			indices[n++] = (i + 0) * 2 + 1;
			indices[n++] = (i + 0) * 2 + 0;
			indices[n++] = (i + 1) * 2 + 0;

			indices[n++] = (i + 1) * 2 + 1;
			indices[n++] = (i + 0) * 2 + 1;
			indices[n++] = (i + 1) * 2 + 0;
		}
		for(int i = 0; i < clipmap_vert_resolution - 1; ++i)
		{
			indices[n++] = start_of_horizontal + (i + 0) * 2 + 1;
			indices[n++] = start_of_horizontal + (i + 0) * 2 + 0;
			indices[n++] = start_of_horizontal + (i + 1) * 2 + 0;

			indices[n++] = start_of_horizontal + (i + 1) * 2 + 1;
			indices[n++] = start_of_horizontal + (i + 0) * 2 + 1;
			indices[n++] = start_of_horizontal + (i + 1) * 2 + 0;
		}

		trim_mesh = new Model3D(vertices, indices);
	}

	// Generate cross mesh
	{
		std::vector<float> vertices;
		vertices.resize(patch_vert_resolution * 8 * 3);
		std::vector<int> indices;
		indices.resize(tile_resolution * 24 + 6);
		uint32_t n = 0;

		// Horizontal vertices
		for(int i = 0; i < patch_vert_resolution * 2; ++i)
		{
			vertices[n*3+0] = i - (float)(tile_resolution);
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 0;
			++n;

			vertices[n*3+0] = i - (float)(tile_resolution);
			vertices[n*3+1] = 0;
			vertices[n*3+2] = 1;
			++n;
		}

		uint32 start_of_vertical = n;

		// Vertical vertices
		for(int i = 0; i < patch_vert_resolution * 2; ++i)
		{
			vertices[n*3+0] = 0;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = i - (float)(tile_resolution);
			++n;
			vertices[n*3+0] = 1;
			vertices[n*3+1] = 0;
			vertices[n*3+2] = i - (float)(tile_resolution);
			++n;
		}

		n = 0;
		// Horizontal indices
		for(int i = 0; i < tile_resolution * 2 + 1; ++i)
		{
			uint32 bl = i * 2 + 0;
			uint32 br = i * 2 + 1;
			uint32 tl = i * 2 + 2;
			uint32 tr = i * 2 + 3;

			indices[n++] = br;
			indices[n++] = bl;
			indices[n++] = tr;
			indices[n++] = bl;
			indices[n++] = tl;
			indices[n++] = tr;
		}

		// Vertical indices
		for(int i = 0; i < tile_resolution * 2 + 1; ++i)
		{
			if(i == tile_resolution)
				continue;

			uint32 bl = i * 2 + 0;
			uint32 br = i * 2 + 1;
			uint32 tl = i * 2 + 2;
			uint32 tr = i * 2 + 3;
			
			indices[n++] = start_of_vertical + br;
			indices[n++] = start_of_vertical + tr;
			indices[n++] = start_of_vertical + bl;
			indices[n++] = start_of_vertical + bl;
			indices[n++] = start_of_vertical + tr;
			indices[n++] = start_of_vertical + tl;
		}

		cross_mesh = new Model3D(vertices, indices);
	}

	// Generate seam mesh
	{
		std::vector<float> vertices;
		vertices.resize(clipmap_vert_resolution * 4 * 3);
		std::vector<int> indices;
		indices.resize(clipmap_vert_resolution * 6);

		for(int i = 0; i < clipmap_vert_resolution; ++i)
		{
			uint32 index;
			index = clipmap_vert_resolution * 0 + i;
			vertices[index*3+0] = i;
			vertices[index*3+1] = 0;
			vertices[index*3+2] = 0;

			index = clipmap_vert_resolution * 1 + i;
			vertices[index*3+0] = clipmap_vert_resolution;
			vertices[index*3+1] = 0;
			vertices[index*3+2] = i;

			index = clipmap_vert_resolution * 2 + i;
			vertices[index*3+0] = clipmap_vert_resolution - i;
			vertices[index*3+1] = 0;
			vertices[index*3+2] = clipmap_vert_resolution;

			index = clipmap_vert_resolution * 3 + i;
			vertices[index*3+0] = 0;
			vertices[index*3+1] = 0;
			vertices[index*3+2] = clipmap_vert_resolution - i;
		}

		uint32 n = 0;
		// Indices
		for(int i = 0; i < clipmap_vert_resolution * 4; i += 2)
		{
			indices[n++] = i + 1;
			indices[n++] = i;
			indices[n++] = i + 2;
		}

		// Make the last triangle wrap-around
		indices[indices.size() - 1] = 0;

		seam_mesh = new Model3D(vertices, indices);
	}
}

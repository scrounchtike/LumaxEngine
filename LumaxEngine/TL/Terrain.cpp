
#include "Terrain.hpp"

#include "../RL/Model3D.hpp"

#include "Map.hpp"
#include "TerrainQueue.hpp"

#include <vector>

Terrain::Terrain(TerrainGenDesc* gen_description, float x, float z, float size)
	: gen_description(gen_description), x(x), z(z), size(size)
{
	length_triangle = (float) size / (float) (LOD_SIZES[lod] - 1);
}

void Terrain::generate(int lod)
{
	this->lod = lod;
	length_triangle = (float) size / (float) (LOD_SIZES[lod] - 1);
		
	// Determine the size of terrain from LOD argument
	uint32 lod_terrain_size = LOD_SIZES[lod];
	float scale_factor = LOD_SIZES[LOD0] / lod_terrain_size;

	uint32 vertex_count = lod_terrain_size - 1;
	int count = vertex_count * vertex_count;

	// Variables for border stitching upto LOD0
	int border_vertices = (LOD_SIZES[LOD0] - 2) * 4 + 4;

	// Resizing the vectors to hold the new mesh data
	vertices.resize(count * 3);
	indices.resize(6 * (vertex_count - 1) * (vertex_count - 1));
	texCoords.resize(count * 2);
	normals.resize(count * 3);
	slopes.resize(count);
	
	int vertex_pointer = 0;
	for(int i = 0; i < vertex_count; ++i){
		for(int j = 0; j < vertex_count; ++j){
			// Getting the height of the terrain
			float height = gen_description->getHeight(j*scale_factor+x,
																								i*scale_factor+z);

			// Vertices
			vertices[vertex_pointer*3+0] = (float)(j + 1)/((float)vertex_count+1) * size;
			vertices[vertex_pointer*3+1] = height;
			vertices[vertex_pointer*3+2] = (float)(i + 1)/((float)vertex_count+1) * size;

			// Texcoords
			texCoords[vertex_pointer*2+0] = (float)j/((float)vertex_count+1) * 16.0;
			texCoords[vertex_pointer*2+1] = (float)i/((float)vertex_count+1) * 16.0;

			// Normals
			Vec3 normal = gen_description->getNormal(j*scale_factor+x,
																							 i*scale_factor+z,
																							 length_triangle);
			normals[vertex_pointer*3+0] = normal.x;
			normals[vertex_pointer*3+1] = normal.y;
			normals[vertex_pointer*3+2] = normal.z;
			
			// Slope
			float slope = gen_description->getSlope(j*scale_factor+x,
																							i*scale_factor+z);
			//std::cout << slope << std::endl;
			slopes[vertex_pointer] = slope;
			
			++vertex_pointer;
		}
	}
	
	// Indices
	int pointer = 0;
  for(int gz = 0; gz < vertex_count - 1; gz++){
		for(int gx = 0; gx < vertex_count - 1; gx++){
			int topLeft = (gz * vertex_count) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vertex_count) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
		}
	}
	
	// Create terrain model
	model = new Model3D(vertices, indices, texCoords, normals);
}

void Terrain::generate_async(int lod)
{
}

void Terrain::upgrade_to_LOD0()
{
	// LOD1 -> LOD0
	// Verify we are on LOD1
	if(lod != LOD1)
		assert(!"Trying to upgrade to LOD0, but terrain is not LOD1!");
	this->lod = LOD0;

	upgrade_LOD(LOD1);
}

void Terrain::upgrade_to_LOD1()
{
	// LOD2 -> LOD1
	// Verify we are on LOD2
	if(lod != LOD2)
		assert(!"Trying to upgrade to LOD1, but terrain is not LOD2!");
	this->lod = LOD1;
	
	upgrade_LOD(LOD2);
}

void Terrain::upgrade_to_LOD2()
{
	// LOD3 -> LOD2
	// Verify we are on LOD3
	if(lod != LOD3)
		assert(!"Trying to upgrade to LOD2, but terrain is not LOD3!");
	this->lod = LOD2;

	upgrade_LOD(LOD3);
}

void Terrain::downgrade_to_LOD1()
{
	// LOD0 -> LOD1
	// Verify we are on LOD0
	if(lod != LOD0)
		assert(!"Trying to downgrade to LOD1, but terrain is not LOD0!");
	this->lod = LOD1;
	
	downgrade_LOD(LOD0);
}

void Terrain::downgrade_to_LOD2()
{
	// LOD2 -> LOD3
	// Verify we are on LOD2
	if(lod != LOD1)
		assert(!"Trying to downgrade to LOD2, but terrain is not LOD1!");
	this->lod = LOD2;

	downgrade_LOD(LOD1);
}

void Terrain::downgrade_to_LOD3()
{
	// LOD2 -> LOD3
	// Verify we are on LOD2
	if(lod != LOD2)
		assert(!"Trying to downgrade to LOD3, but terrain is not LOD2!");
	this->lod = LOD3;

	downgrade_LOD(LOD2);
}

void Terrain::upgrade_LOD(int old_lod)
{
	// Can't upgrade LOD0
	assert(old_lod != LOD0);

	length_triangle = (float) size / (float) (LOD_SIZES[lod] - 1);
	
	int scale_lod_factor = (int)(LOD_SIZES[lod] / LOD_SIZES[old_lod]);
	uint32 vertex_count = LOD_SIZES[lod];
	uint32 old_vertex_count = LOD_SIZES[old_lod];
	float scale_factor = (int)(LOD_SIZES[LOD0] / vertex_count);
	int count = vertex_count * vertex_count;

	// Make a local copy of the mesh data
	std::vector<float> prev_vertices = vertices;
	std::vector<int> prev_indices = indices;
	std::vector<float> prev_texCoords = texCoords;
	std::vector<float> prev_normals = normals;
	std::vector<float> prev_slopes = slopes;

	// Resize mesh data
	vertices.resize(count * 3);
	indices.resize(6 * (vertex_count - 1) * (vertex_count - 1));
	texCoords.resize(count * 2);
	normals.resize(count * 3);
	slopes.resize(count);

	int old_lod_pointer = 0;
	int vertex_pointer = 0;
	for(int i = 0; i < vertex_count; ++i){
		for(int j = 0; j < vertex_count; ++j){
			// Is this a height we already have ?
			float height = 0.0f;
			if((i % scale_lod_factor == 0) && (j % scale_lod_factor == 0))
			{
				// Retrieve the height from previous mesh data
				height = prev_vertices[old_lod_pointer*3+1];
				++old_lod_pointer;
			}
			else
			{
				// Compute the height of the terrain
				height = gen_description->getHeight(j*scale_factor+x,
																						i*scale_factor+z);
			}
			
			// Vertices
			vertices[vertex_pointer*3+0] = (float)j/((float)vertex_count - 1) * size;
			vertices[vertex_pointer*3+1] = height;
			vertices[vertex_pointer*3+2] = (float)i/((float)vertex_count - 1) * size;

			// Texcoords
			texCoords[vertex_pointer*2+0] = (float)j/((float)vertex_count - 1) * 16.0;
			texCoords[vertex_pointer*2+1] = (float)i/((float)vertex_count - 1) * 16.0;

			// Normals
			Vec3 normal = gen_description->getNormal(j*scale_factor+x,
																							 i*scale_factor+z,
																							 length_triangle);
			normals[vertex_pointer*3+0] = normal.x;
			normals[vertex_pointer*3+1] = normal.y;
			normals[vertex_pointer*3+2] = normal.z;
			
			// Slope
			float slope = gen_description->getSlope(j*scale_factor+x,
																							i*scale_factor+z);
			slopes[vertex_pointer] = slope;
			
			++vertex_pointer;
		}
	}
	
	// Indices
	int pointer = 0;
  for(int gz = 0; gz < vertex_count - 1; gz++){
		for(int gx = 0; gx < vertex_count - 1; gx++){
			int topLeft = (gz * vertex_count) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vertex_count) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
		}
	}

	// Delete old model
	delete model;
	// Create a new model
	model = new Model3D(vertices, indices, texCoords, normals);
}

void Terrain::downgrade_LOD(int old_lod)
{
	// Can't downgrade LOD3 !
	assert(old_lod != LOD3);

	length_triangle = (float) size / (float) (LOD_SIZES[lod] - 1);

	float  scale_lod_factor = (float)LOD_SIZES[old_lod] / (float)LOD_SIZES[lod];
	uint32 vertex_count = LOD_SIZES[lod];
	uint32 old_vertex_count = LOD_SIZES[old_lod];
	float scale_factor = (int)(LOD_SIZES[LOD0] / vertex_count);
	int count = vertex_count * vertex_count;

	// Make a local copy of the mesh data
	std::vector<float> prev_vertices = vertices;
	std::vector<int> prev_indices = indices;
	std::vector<float> prev_texCoords = texCoords;
	std::vector<float> prev_normals = normals;
	std::vector<float> prev_slopes = slopes;

	// Resize mesh data
	vertices.resize(count * 3);
	indices.resize(6 * (vertex_count - 1) * (vertex_count - 1));
	texCoords.resize(count * 2);
	normals.resize(count * 3);
	slopes.resize(count);

	int vertex_pointer = 0;
	for(int i = 0; i < vertex_count; ++i){
		for(int j = 0; j < vertex_count; ++j){
			// Get height from previous higher resolution mesh data
			int old_pointer = (int)((i*scale_lod_factor*old_vertex_count) + j*scale_lod_factor);
			float previous_height = prev_vertices[old_pointer*3+1];
			
			// Vertices
			vertices[vertex_pointer*3+0] = (float)j/((float)vertex_count - 1) * size;
			vertices[vertex_pointer*3+1] = previous_height;
			vertices[vertex_pointer*3+2] = (float)i/((float)vertex_count - 1) * size;

			// Texcoords
			texCoords[vertex_pointer*2+0] = (float)j/((float)vertex_count - 1) * 16.0;
			texCoords[vertex_pointer*2+1] = (float)i/((float)vertex_count - 1) * 16.0;

			// Normals
			Vec3 normal = gen_description->getNormal(j*scale_factor+x,
																							 i*scale_factor+z,
																							 length_triangle);
			normals[vertex_pointer*3+0] = normal.x;
			normals[vertex_pointer*3+1] = normal.y;
			normals[vertex_pointer*3+2] = normal.z;
			
			// Slope
			float slope = gen_description->getSlope(j*scale_factor+x,
																							i*scale_factor+z);
			slopes[vertex_pointer] = slope;
			
			++vertex_pointer;
		}
	}

	// Indices
	int pointer = 0;
  for(int gz = 0; gz < vertex_count - 1; gz++){
		for(int gx = 0; gx < vertex_count - 1; gx++){
			int topLeft = (gz * vertex_count) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vertex_count) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
		}
	}

	// Delete old model
	delete model;
	// Create a new model
	model = new Model3D(vertices, indices, texCoords, normals);
}

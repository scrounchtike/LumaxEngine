
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>

#include "../math.hpp"

#include "TerrainGenDesc.hpp"

// Levels of Detail
// LOD0 = best appearance level of detail
// LOD3 = Shape and color recognizable, long distance
enum LOD
{
	LOD0 = 0,
	LOD1 = 1,
	LOD2 = 2,
	LOD3 = 3
};
static const unsigned LOD_SIZES[4] = {256, 128, 64, 32};

class Model3D;
class Map;
class TerrainQueue;

class Terrain{
	friend class Map;
public:
	Terrain(TerrainGenDesc* gen_description, float x, float z, float size);

	// Generate functions reallocate memory (if size if insufficient)
	// and redo the algorithm full
	// Previous terrain is lost and destroyed
	void generate(int lod);
	void generate_async(int lod);

	// LOD generation functions are all safe for multithreading

	// LOD upgrade functions
	void upgrade_to_LOD0();
	void upgrade_to_LOD1();
	void upgrade_to_LOD2();

	// LOD downgrade functions
	void downgrade_to_LOD1();
	void downgrade_to_LOD2();
	void downgrade_to_LOD3();
	
public:
	// Terrain description
	TerrainGenDesc* gen_description;
	float x, z;
	float size = 256.0f;
	float length_triangle;
	
	int lod = LOD0;

	Model3D* model;

	// Store mesh data for LOD regenerations
	std::vector<float> vertices;
	std::vector<int> indices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<float> slopes;

	// Helper functions for switching between LODs
	void upgrade_LOD(int old_lod);
	void downgrade_LOD(int old_lod);
};

#endif

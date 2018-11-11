
#ifndef HEIGHT_MAP_TERRAIN_HPP
#define HEIGHT_MAP_TERRAIN_HPP

#include <string>

#include "../core/ECS.hpp"
#include "../GL/LevelWolf.hpp"

struct TileHolder;

#define MAX_PIXEL_COLOR 256*256*256

class HeightMapTerrain{
public:
	HeightMapTerrain(const std::string& filename, float size, float maxHeight, int vertexCount) : filename(filename), size(size), maxHeight(maxHeight), vertexCount(vertexCount) {}
	
	float size;
	float maxHeight;
	int vertexCount;
	
	std::string filename;
	
	static Model3D* createHeightMapTerrain(const std::string& filename);
	static Model3D* createMPDTerrain();
	static Model3D* createFaultTerrain();
	static Model3D* createPerlinTerrain(unsigned int x, unsigned int z);

	static std::vector<float> staticVertices;
	static std::vector<int> staticIndices;
	static std::vector<float> staticTexCoords;
	static std::vector<float> staticNormals;
	static std::vector<float> staticSlopes;
	static bool building;
};


#endif

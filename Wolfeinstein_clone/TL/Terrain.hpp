
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>

#include "../math.hpp"

class Model3D;
class Map;
class TerrainQueue;

class Terrain{
	friend class Map;
public:
	static Model3D* generateTerrainAsync(unsigned vertexCount, float size,
																			 std::function<float(int i, int j)> h,
																			 std::function<Vec3(int i, int j)> n,
																			 std::function<float(int i, int j)> s);

	static Model3D* generateTerrain(unsigned vertexCount, float size,
																	std::function<float(int i, int j)> h,
																	std::function<Vec3(int i, int j)> n,
																	std::function<float(int i, int j)> s);
public:
	static std::vector<float> staticVertices;
	static std::vector<int> staticIndices;
	static std::vector<float> staticTexCoords;
	static std::vector<float> staticNormals;
	static std::vector<float> staticSlopes;
	
public:
	static bool building;
};

#endif

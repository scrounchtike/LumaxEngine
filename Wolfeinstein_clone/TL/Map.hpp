
#ifndef MAP_HPP
#define MAP_HPP

#include "../RL/Camera.hpp"
#include "Terrain.hpp"

#include "../math.hpp"
#include "../math/types.hpp"

#include <math.h>

#include <vector>
#include <unordered_map>

#include "TerrainQueue.hpp"

#include "../RL/WaterTile.hpp"

#include "FBmTerrain.hpp"

class Model3D;

// LOD values range from 0 - 3
// 3 - Worst
// 2 - Bad
// 1 - Good
// 0 - Best

enum LODs {
	NONE, LOD0, LOD1, LOD2, LOD3
};

#define UNLOAD 1
#define SEEN 2
#define SET 4
#define SWAP 8
#define LOADED 16
#define LOADING 32

struct TileHolder {
	TileHolder() {}

	WaterTile* water;

	FBmTerrain* terrainDesc;
	
	Model3D* terrain;
	Model3D* swapTerrain;
	Transform3D* transform;
	Transform3D* swapTransform;

	const int vertexCount = 128;
	float heights[128][128];
	Vec3 normals[128][128];
	float slopes[128][128];
	
	float LOD = NONE;
	float previousLOD = NONE;
	
	Vec2 terrainPos;
Vec2 swapTerrainPos;

	uint64 flags = 0;
};

class Map{
public:
	Map(float visionRadius, float terrainSize);
	
	void update();
	void render(Shader* shader) const;
	void renderWater() const;
private:
	float visionRadius;
	float visionRadiusSquared;
	float terrainSize;
	
	void generate(TileHolder* tile, unsigned i, unsigned j);
	
std::vector<TileHolder*> loadedTerrains;

	TileHolder*** tiles;

	TerrainQueue queue;

	//FBmTerrain* terrainDesc;

public:
	static std::mutex mutex_terrain;
};

#endif

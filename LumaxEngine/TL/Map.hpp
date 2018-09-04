
#ifndef MAP_HPP
#define MAP_HPP

#include "../RL/Camera.hpp"
#include "Terrain.hpp"

#include "../math.hpp"
#include "../math/types.hpp"

#include <math.h>

#include <vector>
#include <unordered_map>

// LOD values range from 0 - 3
// 3 - Worst
// 2 - Bad
// 1 - Good
// 0 - Best

bool Square_intersects_Circle(Vec2 pos, Vec2 size, Vec2 circle, float radius){
	float sqDistance = 0;
	for (int i = 0; i < 2; ++i) {
		float min = pos[i] - size[i];
		float max = pos[i] + size[i];
		float v = circle[i];
		if (v < min) sqDistance += (min - v)*(min - v);
		if (v > max) sqDistance += (v - max)*(v - max);
	}
	return sqDistance <= radius*radius;
}

enum LODs {
	LOD0, LOD1, LOD2, LOD3
};

struct TileHolder {
	TileHolder() {}
	
	Terrain* terrain;
	bool seen = false;
	float LOD = LOD3;
};

class Map{
public:
	Map();

	void update(Camera* camera){
		float visionRadius = 400;
		float radius = visionRadius;
		float visionRadiusSquared = visionRadius * visionRadius;
		float terrainSize = 100;
		
		//1. Obtain position
		Vec3 playerPosition = camera->getPosition();
		Vec2 playerPos = Vec2(playerPosition.x, playerPosition.z);
		float posX = playerPosition.x;
		float posY = playerPosition.z;

		//2. Get bounding box from radius
		Vec2 lower_bound = Vec2(posX - radius, posY - radius);
		Vec2 upper_bound = Vec2(posX + radius, posY + radius);

		//3. Get terrain bounding box (includes all possible seen terrains)
		lower_bound = Vec2(floor(lower_bound.x/(float)terrainSize)*terrainSize,
											 floor(lower_bound.y/(float)terrainSize)*terrainSize);
		upper_bound = Vec2(floor(upper_bound.x/(float)terrainSize)*terrainSize,
											 floor(upper_bound.y/(float)terrainSize)*terrainSize);

		//4. Reset seen flag for all terrains
		for(auto it = terrains.begin(); it != terrains.end(); ++it){
			it->second->seen = false;
		}

		//5. Intersect all possible terrains with vision circle
		for(int i = lower_bound.x; i <= upper_bound.x; i += terrainSize){
			for(int j = lower_bound.y; j <= upper_bound.y; j += terrainSize){
				// Check if terrain square intersects vision circle
				if(!Square_intersects_Circle(Vec2(i,j), Vec2(terrainSize, terrainSize), playerPos, visionRadius))
					continue;

				// Add terrain to terrain map if not already present
				uint64 ti64 = ((uint64)i << 32) | (uint64)j;
				auto it = terrains.find(ti64);
				if(it != terrains.end()){
					// Terrain is already present, set it to seen
					it->second->seen = true;
				}else{
					// Terrain is new
					// TODO: Allocate terrain from a pool of terrains
					terrains.insert(std::pair<uint64, TileHolder*>(ti64, new TileHolder()));
					// Add terrain to queue for asynchronous loading
					// TODO
				}
			}
		}

		//6. Remove all terrains that can no longer be seen
		for(auto it = terrains.begin(); it != terrains.end(); ++it){
			if(!it->second->seen){
				// Remove terrain
				//TODO
			}
		}
	}
	void render();
private:
	std::unordered_map<uint64, TileHolder*> terrains;
};

#endif

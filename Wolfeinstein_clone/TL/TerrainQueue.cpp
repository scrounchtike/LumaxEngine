
#include "TerrainQueue.hpp"

#include <iostream>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <future>

#include "HeightMapTerrain.hpp"
#include "Map.hpp"
#include "../core/Time.hpp"

TerrainQueue::TerrainQueue(){
	
}

void TerrainQueue::start(){
	t1 = std::thread(&TerrainQueue::asyncLoad, this);
	t1.detach();
}

void TerrainQueue::terminate(){
	//t1.join();
}

void TerrainQueue::addTerrain(TileHolder* tile){
	// Lock mutex
	queue.push_back(tile);
	// Unlock mutex
}

void TerrainQueue::addTerrainLOD0(TileHolder* tile){
	queueLOD0.push_back(tile);
}

void TerrainQueue::addTerrainLOD1(TileHolder* tile){
	queueLOD1.push_back(tile);
}

void TerrainQueue::addTerrainLOD2(TileHolder* tile){
	queueLOD2.push_back(tile);
}

void TerrainQueue::addTerrainLOD3(TileHolder* tile){
	queueLOD3.push_back(tile);
}

//std::mutex mutex;

void TerrainQueue::asyncLoad(){
	while(true){
		//std::cout << "Hello from thread" << std::endl;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(50ms);
		
		// Lock mutex
		
		//mutex.lock();
		//auto lock = std::unique_lock<std::mutex>(mutex);
		std::mutex& mutex = Map::mutex_terrain;
		//if(!mutex.try_lock())
		//	continue;
		mutex.lock();
		
		//std::cout << "test" << std::endl;
		
		if(Terrain::building){
			std::cout << "cant build new terrain" << std::endl;
			mutex.unlock();
			continue;
		}
		
		//std::cout << "building new terrain" << std::endl;
		
		if(!queue.size()){
			mutex.unlock();
			continue;
		}
		TileHolder* toLoad = queue.front();
		queue.pop_front();
		
		const int terrainSize = 100;
		Vec2 terrainPos = toLoad->terrainPos;
		//HeightMapTerrain::createFBmTerrainASYNC(128, std::abs(terrainPos.x)/terrainSize*127, std::abs(terrainPos.y)/terrainSize*127);
		int vertexCount = 128;
		
		int previousLOD = toLoad->previousLOD;
		int LOD = toLoad->LOD;
		
		/*
		std::string LOD = "LOD3";
		if(toLoad->LOD = LOD0)
			LOD = "LOD0";
		else if(toLoad->LOD = LOD1)
			LOD = "LOD1";
		else if(toLoad->LOD = LOD2)
			LOD = "LOD2";
		*/
		
		FBmTerrain* terrainDesc = toLoad->terrainDesc;
		terrainDesc->x = std::abs(terrainPos.x)/terrainSize*(vertexCount-1);
		terrainDesc->z = std::abs(terrainPos.y)/terrainSize*(vertexCount-1);
		
		//std::cout << "Loading terrain LOD" << (previousLOD-1) << " to LOD" << (LOD-1) << std::endl;
		
		//Time::startTimer();
		
	  switch((int)toLoad->LOD){
		case LOD0:
			//std::cout << "Loading terrain LOD0" << std::endl;
			if(toLoad->previousLOD == NONE)
				toLoad->terrainDesc->generate_async_LOD0(toLoad);
			else
				toLoad->terrainDesc->upgrade_to_LOD0(toLoad);
			break;
		case LOD1:
			//std::cout << "Loading terrain LOD1" << std::endl;
			if(toLoad->previousLOD == NONE)
				toLoad->terrainDesc->generate_async_LOD1(toLoad);
			else if(toLoad->previousLOD == LOD0)
				toLoad->terrainDesc->downgrade_to_LOD1(toLoad);
			else
				toLoad->terrainDesc->upgrade_to_LOD1(toLoad);
			break;
		case LOD2:
			//std::cout << "Loading terrain LOD2" << std::endl;
			if(toLoad->previousLOD == NONE)
				toLoad->terrainDesc->generate_async_LOD2(toLoad);
			else if(toLoad->previousLOD == LOD1)
				toLoad->terrainDesc->downgrade_to_LOD2(toLoad);
			else
				toLoad->terrainDesc->upgrade_to_LOD2(toLoad);//, std::cout << "LOAD LOD2 from LOD3" << std::endl;
			break;
		case LOD3:
			//std::cout << "Loading terrain LOD3" << std::endl;
			if(toLoad->previousLOD == LOD2)
				toLoad->terrainDesc->downgrade_to_LOD3(toLoad);
			else
				toLoad->terrainDesc->generate_async_LOD3(toLoad);
			break;
		}
		
		//Time::endTimer();
		//std::cout << "time = " << Time::getElapsedTime() << std::endl;
		
		
		
		//HeightMapTerrain::createFBmTerrainASYNC_LOD3_new(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//HeightMapTerrain::createFBmTerrainASYNC_LOD2_new(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//HeightMapTerrain::createFBmTerrainASYNC_LOD1_new(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//Time::startTimer();

		//HeightMapTerrain::createFBmTerrainASYNC_LOD0_new(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//Time::endTimer();
		//std::cout << "libnoise time = " << Time::getElapsedTime() << std::endl;
		
		//HeightMapTerrain::createFBmTerrainASYNC_LOD3(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));
		
		//HeightMapTerrain::createFBmTerrainASYNC_LOD2(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));
		
		//HeightMapTerrain::createFBmTerrainASYNC_LOD1(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//HeightMapTerrain::createFBmTerrainASYNC_LOD0(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//HeightMapTerrain::terrain_LOD0_LOD1(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//HeightMapTerrain::terrain_LOD1_LOD2(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));

		//HeightMapTerrain::terrain_LOD2_LOD3(toLoad, vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));
		
		// Terrain loaded
		toLoad->flags |= (LOADED);
		// Terrain no longer loading
		toLoad->flags &= (~LOADING);
		
		//std::cout << "Loaded a terrain" << std::endl;
		
		// Unlock mutex
		//mutex.unlock();
		mutex.unlock();
	}
}

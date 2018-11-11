
#ifndef TERRAIN_QUEUE_HPP
#define TERRAIN_QUEUE_HPP

#include <thread>
#include <deque>

struct TileHolder;

class TerrainQueue {
public:
TerrainQueue();

void start();
void terminate();

void asyncLoad();

void addTerrain(TileHolder* tile);

void addTerrainLOD0(TileHolder* tile);
void addTerrainLOD1(TileHolder* tile);
void addTerrainLOD2(TileHolder* tile);
void addTerrainLOD3(TileHolder* tile);

std::deque<TileHolder*> queue;

std::deque<TileHolder*> queueLOD0;
std::deque<TileHolder*> queueLOD1;
std::deque<TileHolder*> queueLOD2;
std::deque<TileHolder*> queueLOD3;

std::thread t1;
};

#endif

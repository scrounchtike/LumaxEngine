
#include "Map.hpp"

#include "../lumax.hpp"
#include "HeightMapTerrain.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/Renderer.hpp"

#include "../core/Time.hpp"

std::mutex Map::mutex_terrain;

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

Map::Map(float visionRadius, float terrainSize)
	: visionRadius(visionRadius), visionRadiusSquared(visionRadius*visionRadius), terrainSize(terrainSize)
{	
	// Generate terrain pool storage
	unsigned int maxTerrainSide = (unsigned int)(std::ceil(2.0*visionRadius / terrainSize)) + 1;
	tiles = new TileHolder**[maxTerrainSide];
	for(int i = 0; i < maxTerrainSide; ++i){
		tiles[i] = new TileHolder*[maxTerrainSide];
		for(int j = 0; j < maxTerrainSide; ++j){
			tiles[i][j] = new TileHolder();
			tiles[i][j]->terrainDesc = new FBmTerrain();
			tiles[i][j]->flags = 0;
		}
	}
	
	// Terrain queue creation
	//queue.start();
	//queue.terminate();
}

void Map::generate(TileHolder* toLoad, unsigned i, unsigned j){
	Vec2 terrainPos = toLoad->terrainPos;
	toLoad->terrainDesc->x = std::abs(terrainPos.x)/terrainSize*(toLoad->terrainDesc->vertexCount-1);
	toLoad->terrainDesc->z = std::abs(terrainPos.y)/terrainSize*(toLoad->terrainDesc->vertexCount-1);
		
	switch((int)toLoad->LOD){
	case LOD0:
		if(toLoad->previousLOD == NONE)
			toLoad->terrainDesc->generate_async_LOD0(toLoad);
		else
			toLoad->terrainDesc->upgrade_to_LOD0(toLoad);
		break;
	case LOD1:
		if(toLoad->previousLOD == NONE)
			toLoad->terrainDesc->generate_async_LOD1(toLoad);
		else if(toLoad->previousLOD == LOD0)
			toLoad->terrainDesc->downgrade_to_LOD1(toLoad);
		else
			toLoad->terrainDesc->upgrade_to_LOD1(toLoad);
		break;
	case LOD2:
		if(toLoad->previousLOD == NONE)
			toLoad->terrainDesc->generate_async_LOD2(toLoad);
		else if(toLoad->previousLOD == LOD1)
			toLoad->terrainDesc->downgrade_to_LOD2(toLoad);
		else
			toLoad->terrainDesc->upgrade_to_LOD2(toLoad);
		break;
	case LOD3:
		if(toLoad->previousLOD == LOD2)
			toLoad->terrainDesc->downgrade_to_LOD3(toLoad);
		else
			toLoad->terrainDesc->generate_async_LOD3(toLoad);
		break;
	}

	tiles[i][j]->terrain = new Model3D(Terrain::staticVertices,
																		 Terrain::staticIndices,
																		 Terrain::staticTexCoords,
																		 Terrain::staticNormals,
																		 Terrain::staticSlopes);
	Terrain::building = false;
	
	// Add loaded terrain to map
	loadedTerrains.push_back(tiles[i][j]);
	
	// Terrain loaded
	//toLoad->flags |= (LOADED);
	// Terrain no longer loading
	toLoad->flags &= (~(LOADING));
}

void Map::update(){
	//1. Obtain position
	Camera* camera = lmx::getStaticCamera();
	Vec3 playerPosition = camera->getPosition();
	Vec2 playerPos = Vec2(playerPosition.x, playerPosition.z);
	float posX = playerPosition.x;
	float posY = playerPosition.z;
	//std::cout << "camera = " << playerPos.x << "   " << playerPos.y << std::endl;

	static Vec2 last_lower_bounds(0,0);
	static Vec2 last_upper_bounds(0,0);
	static bool first = true;
	
	bool bounds_changed = false;
	//2. Get bounding box from radius
	Vec2 lower_bounds = Vec2(posX - visionRadius, posY - visionRadius);
	Vec2 upper_bounds = Vec2(posX + visionRadius, posY + visionRadius);
	
	//3. Get terrain bounding box (includes all possible seen terrains)
	lower_bounds = Vec2(floor(lower_bounds.x/(float)terrainSize)*terrainSize,
										 floor(lower_bounds.y/(float)terrainSize)*terrainSize);
	upper_bounds = Vec2(floor(upper_bounds.x/(float)terrainSize)*terrainSize,
										 floor(upper_bounds.y/(float)terrainSize)*terrainSize);
	//std::cout << "bounds" << std::endl;
	//std::cout << lower_bounds.x << "   " << lower_bounds.y << std::endl;
	//std::cout << upper_bounds.x << "   " << upper_bounds.y << std::endl;
	if(lower_bounds.x != last_lower_bounds.x || lower_bounds.y != last_lower_bounds.y || upper_bounds.x != last_upper_bounds.x || upper_bounds.y != last_upper_bounds.y){
		last_lower_bounds = lower_bounds;
		last_upper_bounds = upper_bounds;
		bounds_changed = true;
	}
	
	unsigned int x_range = upper_bounds.x - lower_bounds.x;
	unsigned int z_range = upper_bounds.y - lower_bounds.y;
	
	unsigned int maxTerrainSide = (unsigned int)(std::ceil(2.0*visionRadius / terrainSize)) + 1;
	//std::cout << "max terrain side = " << maxTerrainSide << std::endl;
	
	//std::cout << "unload" << std::endl;
	// Unload terrains no longer in range
	for(TileHolder* tile : loadedTerrains){
		Vec2 terrainPos = tile->terrainPos;
		//std::cout << "terrain pos = " << terrainPos.x << "   " << terrainPos.y << std::endl;
		bool unload = false;
		if(terrainPos.x < lower_bounds.x || terrainPos.x > upper_bounds.x)
			unload = true;
		if(terrainPos.y < lower_bounds.y || terrainPos.y > upper_bounds.y)
			unload = true;
		
		// Mark as unset if terrain no longer in range
		if(unload){
			// Remove tile from loadedTerrains:
			//std::cout << "deleting : " << terrainPos.x << "  " << terrainPos.y << std::endl;
			loadedTerrains.erase(std::remove(loadedTerrains.begin(), loadedTerrains.end(), tile), loadedTerrains.end());
			
			tile->flags &= (~SET);
			// Should we delete the tile ?
			delete tile->terrain;
		}
	}

	// Check for loaded terrains
	for(int i = 0; i < maxTerrainSide; ++i){
		for(int j = 0; j < maxTerrainSide; ++j){
			if(!mutex_terrain.try_lock())
				continue;
			//mutex_terrain.lock();
			//std::cout << "checking if terrain is loaded" << std::endl;
			if(tiles[i][j]->flags & LOADING){
				mutex_terrain.unlock();
				continue;
			}
			
			if(tiles[i][j]->flags & LOADED){
				tiles[i][j]->flags &= (~LOADED);

				// Does terrain need to be loaded?
				if(Terrain::building){
					std::cout << "Creating terrain model" << std::endl;
					
					// Create model
					tiles[i][j]->terrain = new Model3D(Terrain::staticVertices,
																						 Terrain::staticIndices,
																						 Terrain::staticTexCoords,
																						 Terrain::staticNormals,
																						 Terrain::staticSlopes);
					Terrain::building = false;
					
					std::cout << "Terrain model created" << std::endl;
				}
				
				// Add loaded terrain to map
				loadedTerrains.push_back(tiles[i][j]);
			}
			mutex_terrain.unlock();
		}
	}

	// Remap and create new terrains
	if(bounds_changed || first){
		// Create temp tiles storage
		/*
		TileHolder*** temp_tiles = new TileHolder**[maxTerrainSide];
		for(int i = 0; i < maxTerrainSide; ++i){
			temp_tiles[i] = new TileHolder*[maxTerrainSide];
			for(int j = 0; j < maxTerrainSide; ++j){
				temp_tiles[i][j] = new TileHolder();
				temp_tiles[i][j]->flags = 0;
			}
		}
		*/
		
		for(int i = 0; i < maxTerrainSide; ++i){
			for(int j = 0; j < maxTerrainSide; ++j){
				if(!mutex_terrain.try_lock())
					continue;
				if(tiles[i][j]->flags & LOADING){
					mutex_terrain.unlock();
					continue;
				}
				//mutex_terrain.unlock();
				
				// Terrain pos we wish to display
				Vec2 terrainPos = Vec2(lower_bounds.x + i*terrainSize, lower_bounds.y + j*terrainSize);

				// Check if terrain was already loaded
				bool alreadyLoaded = false;
				for(TileHolder* tile : loadedTerrains){
					Vec2 targetPos = tile->terrainPos;
					if(terrainPos.x == targetPos.x && terrainPos.y == targetPos.y){
						// Terrain was loaded and found
						tiles[i][j] = tile;
						alreadyLoaded = true;
						continue;
					}
				}
				mutex_terrain.unlock();
				
				//std::cout << "terrain pos = " << terrainPos.x << "   " << terrainPos.y << std::endl;
				
				// Check if terrain is in vision radius
				//if(!Square_intersects_Circle(Vec2(terrainPos.x,terrainPos.y), Vec2(terrainSize, terrainSize), playerPos, visionRadius))
				//	continue;
				
				//std::cout << "visible" << std::endl;
				
				if(alreadyLoaded)
					continue;
				
				// Load terrain procedurally
				std::cout << "Schedule terrain for loading" << std::endl;
				TileHolder* tileHolder = new TileHolder();
				tileHolder->terrainDesc = new FBmTerrain();
				
				//mutex_terrain.lock();
				if(mutex_terrain.try_lock()){
					Vec3 translation = Vec3(terrainPos.x, 0, terrainPos.y);
					Vec3 rotation(0,0,0);
					if(terrainPos.y < 0.0)
						rotation.y = M_PI;//, translation.x += terrainSize, translation.y += 0;
					if(terrainPos.x < 0.0)
						rotation.y = M_PI;
					if(terrainPos.x < 0.0 && terrainPos.y > 0.0){
						rotation.y = 2.0*M_PI;
						//translation.y += 100;
					}
					if(terrainPos.x > 0.0 && terrainPos.y < 0.0){
						rotation.y = M_PI/2.0;
					}
					//rotation.y = M_PI/2.0;
					tileHolder->transform = new Transform3D(translation, rotation);
					tileHolder->terrainPos = terrainPos;
					tileHolder->flags |= (SET | LOADING);
					//tileHolder->flags |= SEEN;

					tileHolder->water = new WaterTile();
					
				  // Calculate LOD level from distance
					Vec3 terrainPos1 = Vec3(tileHolder->terrainPos.x, 0, tileHolder->terrainPos.y);
					Vec3 terrainPos2 = Vec3(terrainPos1.x+terrainSize, 0, terrainPos1.z);
					Vec3 terrainPos3 = Vec3(terrainPos1.x+terrainSize, 0, terrainPos1.z+terrainSize);
					Vec3 terrainPos4 = Vec3(terrainPos1.x, 0, terrainPos1.z+terrainSize);
					
					Vec3 cam = lmx::getStaticCamera()->getPosition();
					
					Vec3 L1 = terrainPos1 - cam;
					float distanceSq1 = dot(L1, L1);
					Vec3 L2 = terrainPos2 - cam;
					float distanceSq2 = dot(L2, L2);
					Vec3 L3 = terrainPos3 - cam;
					float distanceSq3 = dot(L3, L3);
					Vec3 L4 = terrainPos4 - cam;
					float distanceSq4 = dot(L4, L4);
					
					float distanceSq = std::min(distanceSq1, std::min(distanceSq2, std::min(distanceSq3, distanceSq4)));
					
					
					tileHolder->LOD = LOD3;
					if(distanceSq < (120*120))
						tileHolder->LOD = LOD0;
					else if(distanceSq < (200*200))
						tileHolder->LOD = LOD1;
					else if(distanceSq < (350*350))
						tileHolder->LOD = LOD2;
					
					//tileHolder->LOD = LOD3;
					
					std::cout << "LOD = " << (tileHolder->LOD-1) << std::endl;
					
					tileHolder->previousLOD = NONE;
					
					//int vertexCount = 64;
					//tileHolder->terrain = HeightMapTerrain::createFBmTerrain(vertexCount, std::abs(terrainPos.x)/terrainSize*(vertexCount-1), std::abs(terrainPos.y)/terrainSize*(vertexCount-1));
					tiles[i][j] = tileHolder;
					//queue.addTerrain(tileHolder);
					generate(tileHolder, i, j);
					//loadedTerrains.push_back(tileHolder);
					
					mutex_terrain.unlock();
				}
				std::cout << "Scheduling done" << std::endl;
				
				
				//tileHolder->terrain = HeightMapTerrain::createFBmTerrain(128, std::abs(terrainPos.x)/terrainSize*127, std::abs(terrainPos.y)/terrainSize*127);
				
				
				
				//tileHolder->terrain = HeightMapTerrain::createPerlinTerrain(i*255, j*255);
				//tileHolder->terrain = HeightMapTerrain::createMPDTerrain();
				
				//Time::endTimer();
				//std::cout << "time total = " << Time::getElapsedTime() << " ms" << std::endl;
				
				//std::cout << "Model created successfully" << std::endl;
				std::cout << i << "    " << j << std::endl;
				
				// Add it to storage
				std::cout << maxTerrainSide << std::endl;
				
				//loadedTerrains.push_back(tileHolder);
			}
		}
		
		// Swap tiles
		/*
		for(int i = 0; i < maxTerrainSide; ++i){
			for(int j = 0; j < maxTerrainSide; ++j){
				tiles[i][j] = temp_tiles[i][j];
			}
		}
		*/
	}

	// Calculate LOD values each frame
	for(int i = 0; i < maxTerrainSide; ++i){
		for(int j = 0; j < maxTerrainSide; ++j){
			TileHolder* tile = tiles[i][j];
			if(!mutex_terrain.try_lock())
				continue;
			if(tile->flags & LOADING){
				mutex_terrain.unlock();
				continue;
			}
			Vec3 terrainPos1 = Vec3(tile->terrainPos.x, 0, tile->terrainPos.y);
			Vec3 terrainPos2 = Vec3(terrainPos1.x+terrainSize, 0, terrainPos1.z);
			Vec3 terrainPos3 = Vec3(terrainPos1.x+terrainSize, 0, terrainPos1.z+terrainSize);
			Vec3 terrainPos4 = Vec3(terrainPos1.x, 0, terrainPos1.z+terrainSize);
			
			Vec3 cam = lmx::getStaticCamera()->getPosition();
			
			Vec3 L1 = terrainPos1 - cam;
			float distanceSq1 = dot(L1, L1);
			Vec3 L2 = terrainPos2 - cam;
			float distanceSq2 = dot(L2, L2);
			Vec3 L3 = terrainPos3 - cam;
			float distanceSq3 = dot(L3, L3);
			Vec3 L4 = terrainPos4 - cam;
			float distanceSq4 = dot(L4, L4);
			
			float distanceSq = std::min(distanceSq1, std::min(distanceSq2, std::min(distanceSq3, distanceSq4)));
			
			float newLOD = LOD3;
			if(distanceSq < (120*120))
				newLOD = LOD0;
			else if(distanceSq < (200*200))
				newLOD = LOD1;
			else if(distanceSq < (350*350))
				newLOD = LOD2;
			
			if(newLOD != tile->LOD){
				tile->previousLOD = tile->LOD;
				tile->LOD = newLOD;
				
				std::cout << "update terrain LOD: " << (tile->LOD-1) << std::endl;
				//queue.addTerrain(tile);
				generate(tile, i, j);
			}
			mutex_terrain.unlock();
		}
	}

	// Reset visibility flag
	for(TileHolder* tile : loadedTerrains){
		tile->flags &= (~SEEN);
	}
	
	// Visibility test
	for(int i = 0; i < maxTerrainSide; ++i){
		for(int j = 0; j < maxTerrainSide; ++j){
			if(!mutex_terrain.try_lock())
				continue;
			if(tiles[i][j]->flags & LOADING){
				mutex_terrain.unlock();
				continue;
			}
			mutex_terrain.unlock();
			
			// Terrain pos we wish to display
			Vec2 terrainPos = Vec2(lower_bounds.x + i*terrainSize, lower_bounds.y + j*terrainSize);

			// Check intersection with vision circle
			if(!Square_intersects_Circle(Vec2(terrainPos.x,terrainPos.y), Vec2(terrainSize, terrainSize), playerPos, visionRadius))
				continue;

			// Is visible
			tiles[i][j]->flags |= SEEN;
		}
	}

	if(first)
		first = false;
}

void Map::render(Shader* shader) const{
	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// Render all terrains
	for(TileHolder* tile : loadedTerrains){
		if(tile->flags & SEEN){
			//shader->bind();
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
			// Bind projection and view matrices
			static Camera* camera = lmx::getStaticCamera();
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, tile->transform->getTransformation()->getHeadPointer());
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
			float color[4] = {0,0,0, 1.0};
			if(tile->LOD == 0)
				color[1] = 1;
			else if(tile->LOD == 1)
				color[2] = color[1] = 1;
			else if(tile->LOD == 2)
				color[0] = 1;
			else if(tile->LOD >= 3)
				color[0] = color[1] = color[2] = 0.5f;
			float colorGreen[4] = {0,1,0,1};
			glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &colorGreen[0], GL_DYNAMIC_DRAW);

			//shader->setUniform1f("lod", tile->LOD);
			
			tile->terrain->render();
		}
	}
}

void Map::renderWater() const {
	for(TileHolder* tile : loadedTerrains){
		if(tile->flags & SEEN){
			//shader->bind();
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
			// Bind projection and view matrices
			static Camera* camera = lmx::getStaticCamera();
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, tile->transform->getTransformation()->getHeadPointer());

			//shader->setUniform1f("lod", tile->LOD);
			
			//tile->terrain->render();
			tile->water->render();
		}
	}
}

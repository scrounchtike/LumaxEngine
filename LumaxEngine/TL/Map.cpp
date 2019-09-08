
#include "Map.hpp"

#include "../lumax.hpp"
#include "HeightMapTerrain.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/Renderer.hpp"

/*

//#include "../core/Time.hpp"

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
	// Indicates the max possible number of terrains the map can have on one side
	// For example, maxTerrainSide could be 5 and thus the map would hold up to 25 terrains maximum
	// The number of terrains can be lower than the max amount!
	maxTerrainSide = (unsigned int)(std::ceil(2.0*visionRadius / terrainSize)) + 1;
	
	// Generate terrain pool storage
	tiles = new TileHolder**[maxTerrainSide];
	for(int i = 0; i < maxTerrainSide; ++i){
		tiles[i] = new TileHolder*[maxTerrainSide];
		for(int j = 0; j < maxTerrainSide; ++j){
			//tiles[i][j] = new TileHolder();
			//tiles[i][j]->terrainDesc = new FBmTerrain();
			tiles[i][j] = 0;
			//tiles[i][j]->flags = 0;
			
			// TODO: Load the terrains at init time
			
		}
	}

	// Generate terrain swap pool storage
	swap_tiles = new TileHolder**[maxTerrainSide];
	for(int i = 0; i < maxTerrainSide; ++i){
		swap_tiles[i] = new TileHolder*[maxTerrainSide];
		for(int j = 0; j < maxTerrainSide; ++j){
			//swap_tiles[i][j] = new TileHolder();
			//swap_tiles[i][j]->terrainDesc = new FBmTerrain();
			swap_tiles[i][j] = 0;
			//swap_tiles[i][j]->flags = 0;
		}
	}
	
	// Terrain queue creation
	queue.start();
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

	toLoad->terrain = new Model3D(Terrain::staticVertices,
																Terrain::staticIndices,
																Terrain::staticTexCoords,
																Terrain::staticNormals,
																Terrain::staticSlopes);
	Terrain::building = false;
	
	// Add loaded terrain to map
	//loadedTerrains.push_back(toLoad);
	
	// Terrain loaded
	//toLoad->flags |= (LOADED);
	// Terrain no longer loading
	toLoad->flags &= (~LOADING);

	std::cout << "current LOD = " << toLoad->LOD << std::endl;
}

void Map::update(){
	//1. Obtain position from camera
	// Map is centered around camera
	Camera* camera = lmx::getStaticCamera();
	Vec3 playerPosition = camera->getPosition();
	Vec2 playerPos = Vec2(playerPosition.x, playerPosition.z);
	// Get X and Z components of player position (2D map on X-Z plane)
	float posX = playerPosition.x;
	float posY = playerPosition.z;

	static Vec2 last_lower_bounds(0,0);
	static Vec2 last_upper_bounds(0,0);
	static bool first = true;
	
	//2. Get bounding box from radius
	// Terrains inbetween these bounds are loaded and rendered
	// Minimum bound
	Vec2 lower_bounds = Vec2(posX - visionRadius, posY - visionRadius);
	// Maximum bound
	Vec2 upper_bounds = Vec2(posX + visionRadius, posY + visionRadius);
	
	//3. Get terrain bounding box (includes all possible seen terrains)
	// Gets base positions (bottom-left corner position) of all terrains that are either fully or partially inside the last bounds
	lower_bounds = Vec2(floor(lower_bounds.x/(float)terrainSize)*terrainSize,
										 floor(lower_bounds.y/(float)terrainSize)*terrainSize);
	upper_bounds = Vec2(floor(upper_bounds.x/(float)terrainSize)*terrainSize,
										 floor(upper_bounds.y/(float)terrainSize)*terrainSize);

	// Variable indicates if bounds have changed from last frame
	// If no bounds changed, then no terrain have to be moved and/or loaded
	bool bounds_changed = false;
	if(lower_bounds.x != last_lower_bounds.x || lower_bounds.y != last_lower_bounds.y || upper_bounds.x != last_upper_bounds.x || upper_bounds.y != last_upper_bounds.y){
		// Bounds have changed
		// Update the last bounds
		last_lower_bounds = lower_bounds;
		last_upper_bounds = upper_bounds;
		bounds_changed = true;
	}
	
	// Calculate the range in X and Z that terrains can occupy
	unsigned int x_range = upper_bounds.x - lower_bounds.x;
	unsigned int z_range = upper_bounds.y - lower_bounds.y;
	
	// Unload terrains no longer in range
	for(TileHolder* tile : loadedTerrains){
		Vec2 terrainPos = tile->terrainPos;
		
		// Figure out if the terrain is no longer in range
		// If so, mark to unload
		// NOTE: the bounds are inclusive!
		bool unload = false;
		if(terrainPos.x < lower_bounds.x || terrainPos.x > upper_bounds.x)
			unload = true;
		if(terrainPos.y < lower_bounds.y || terrainPos.y > upper_bounds.y)
			unload = true;
		
		// Terrain is to be unloaded
		if(unload){
			// Remove tile from loadedTerrains:
			loadedTerrains.erase(std::remove(loadedTerrains.begin(), loadedTerrains.end(), tile), loadedTerrains.end());

			// Set the tile as NOT SET (therefore not rendered nor updated)
			tile->flags &= (~SET);
			// Delete the terrain of the tile (since we will have to rebuild it)
			delete tile->terrain;
			tile->terrain = 0;
		}
	}

	
	// Check for loaded terrains
	// Loaded terrains are marked as LOADED by the asynchronous loader (TerrainQueue)
	for(int i = 0; i < maxTerrainSide; ++i){
		for(int j = 0; j < maxTerrainSide; ++j){
			if(!swap_tiles[i][j])
				continue;
			
			//if(!mutex_terrain.try_lock())
			//	continue;

			// Wait for mutex to be available
			while(!mutex_terrain.try_lock()){ }
			
			// If tile is already loading asynchronously, continue to next tile
			if(swap_tiles[i][j]->flags & LOADING){
				mutex_terrain.unlock();
				continue;
			}
			
			// Check LOADED flag
			if(swap_tiles[i][j] && swap_tiles[i][j]->flags & LOADED){
				// Unset LOADED flag
				swap_tiles[i][j]->flags &= (~LOADED);
				
				// Does terrain need to be loaded?
				// LOD0->LOD1 requires no building since we do not change vertex data
				if(Terrain::building){
					std::cout << "Creating terrain model" << std::endl;
					
					// Create model
					swap_tiles[i][j]->terrain = new Model3D(Terrain::staticVertices,
																									Terrain::staticIndices,
																									Terrain::staticTexCoords,
																									Terrain::staticNormals,
																									Terrain::staticSlopes);
					Terrain::building = false;
					
					std::cout << "Terrain model created" << std::endl;
				}
				
				// Add loaded terrain to map
				loadedTerrains.push_back(swap_tiles[i][j]);

				tiles[i][j] = swap_tiles[i][j];
			}
			mutex_terrain.unlock();
		}
	}
	
	
	// Remap and create new terrains
	if(bounds_changed || first){
		for(int i = 0; i < maxTerrainSide; ++i){
			for(int j = 0; j < maxTerrainSide; ++j){
				//std::cout << std::endl << "Terrain " << i << "  " << j << std::endl;
				
				// Wait for the mutex to be available
				while(!mutex_terrain.try_lock()){ }
				
				// If terrain is already LOADING, continue to next tile
				if(tiles[i][j] && tiles[i][j]->flags & LOADING){
					mutex_terrain.unlock();
					continue;
				}
				
				// Terrain position we wish to display
				Vec2 terrainPos = Vec2(lower_bounds.x + i*terrainSize, lower_bounds.y + j*terrainSize);
				
				// POSSIBLE BUG:
				// Have to included LOADING tiles to the algorithm
				// When we move (bounds change) then loading terrains might have to
				// be remaped to other tile
				
				// Check if terrain was already loaded
				bool alreadyLoaded = false;
				// For every tile in loadedTerrains (rendered and updated tile), check
				// to see if target terrain pos is matched by another terrain
				for(TileHolder* tile : loadedTerrains){
					Vec2 targetPos = tile->terrainPos;
					if(terrainPos.x == targetPos.x && terrainPos.y == targetPos.y){
						// Terrain was loaded and found
						// The terrain will be swaped at the end of the algorithm so that
						// we do not mistankenly leak or delete a valid terrain at the location
						// tiles[i][j]
						swap_tiles[i][j] = tile;

						//loadedTerrains.push_back(tile);
						//tiles[i][j] = tile;
						// We can safely leave since the terrain was already loaded (exists)
						alreadyLoaded = true;
						continue;
					}
				}
				mutex_terrain.unlock();

				//std::cout << "already loaded = " << alreadyLoaded << std::endl;

				// No need to load new terrain mesh since we found existing mesh
				if(alreadyLoaded)
					continue;
				
				// If the terrain is inside the required range AND was not
				// previously found in the loadedTerrains, then it has to be
				// loaded
				
				// Load NEW terrain procedurally
				std::cout << "Schedule terrain for loading" << std::endl;
				
				// POSSIBLE ISSUE:
				// Leaking of TileHolder and FBmTerrain data!
				// It definitely leaks, please fix..
				TileHolder* tileHolder = new TileHolder();
				tileHolder->terrainDesc = new FBmTerrain();
				
				{
					// Wait for mutex to be available
					while(!mutex_terrain.try_lock()){ }
					
					Vec3 translation = Vec3(terrainPos.x, 0, terrainPos.y);
					// Handle rotation of terrain
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
					
					// Set tile as SET and LOADING
					// Meaning the algorith won't try to assign a terrain to the tile (SET)
					// And the tile won't be updated nor rendering until loading completion (OADONG)
					tileHolder->flags |= (SET | LOADING);
					
					// Water Tile for rendering water
					tileHolder->water = new WaterTile();
					
				  // Calculate LOD level from distance
					Vec3 terrainPos1 = Vec3(tileHolder->terrainPos.x, 0, tileHolder->terrainPos.y);
					Vec3 terrainPos2 = Vec3(terrainPos1.x+terrainSize, 0, terrainPos1.z);
					Vec3 terrainPos3 = Vec3(terrainPos1.x+terrainSize, 0, terrainPos1.z+terrainSize);
					Vec3 terrainPos4 = Vec3(terrainPos1.x, 0, terrainPos1.z+terrainSize);
					
					Vec3 cam = lmx::getStaticCamera()->getPosition();

					// Get squared distances from all four corners of terrain
					Vec3 L1 = terrainPos1 - cam;
					float distanceSq1 = dot(L1, L1);
					Vec3 L2 = terrainPos2 - cam;
					float distanceSq2 = dot(L2, L2);
					Vec3 L3 = terrainPos3 - cam;
					float distanceSq3 = dot(L3, L3);
					Vec3 L4 = terrainPos4 - cam;
					float distanceSq4 = dot(L4, L4);

					// Find the smallest distance from camera to terrain
					// NOTE: The terrain might actually be closer
					// For example, with a terrain with a side closer to the camera
					// than the four corners
					// The actual render vision (radius) might not be respected fully,
					// but this is very minor here
					float distanceSq = std::min(distanceSq1, std::min(distanceSq2, std::min(distanceSq3, distanceSq4)));
					
					// Figure out the LOD from arbitrary LOD range values
					// Closer terrain means lower LOD
					tileHolder->LOD = LOD3;
					if(distanceSq < (120*120))
						tileHolder->LOD = LOD0;
					else if(distanceSq < (200*200))
						tileHolder->LOD = LOD1;
					else if(distanceSq < (350*350))
						tileHolder->LOD = LOD2;

					// Since this is a NEW terrain, the previous LOD is NONE
					// This indicates to the loader that the terrain has to be loaded from
					// scratch
					tileHolder->previousLOD = NONE;
					
					// Set the tiles appropriate cell to the terrain pointer
					swap_tiles[i][j] = tileHolder;
					//tiles[i][j] = tileHolder;

					// Generate the terrain asynchronously or on the same thread (debugging)
					// NOTE: This is a query, the actual loading and adding to render vector
					// will happen later, once the terrain is set as LOADED
					queue.addTerrain(tileHolder);
					//generate(tileHolder, i, j);

					//loadedTerrains.push_back(tileHolder);
					
					mutex_terrain.unlock();
				}
				std::cout << "Scheduling done" << std::endl;
			}
		}
	}

	std::cout << loadedTerrains.size() << std::endl;

	// Assert that all swap terrains are not nullptr
	for(int i = 0; i < maxTerrainSide; ++i){
		for(int j = 0; j < maxTerrainSide; ++j){
			assert(swap_tiles[i][j]);
		}
	}
	
	// Swap all terrain that we moved / loaded
	/*
	if(bounds_changed || first){
		for(int i = 0; i < maxTerrainSide; ++i){
			for(int j = 0; j < maxTerrainSide; ++j){
				// Wait for mutex to be available
				while(!mutex_terrain.try_lock()){ }
				
				TileHolder* swap = swap_tiles[i][j];
				if(swap_tiles[i][j]->flags & LOADING){
					mutex_terrain.unlock();
					continue;
				}
				
				// Terrain is not loading and needs swaping
				tiles[i][j] = swap;
				
				// Add terrain to the rendered list
				//loadedTerrains.push_back(tiles[i][j]);
				
				mutex_terrain.unlock();
			}
		}
		
		//for(int i = 0; i < maxTerrainSide; ++i){
		//	for(int j = 0; j < maxTerrainSide; ++j){
		//		assert(tiles[i][j]);
		//	}
		//}
	}
*//*
	
	// Calculate LOD values each frame
	for(int i = 0; i < maxTerrainSide; ++i){
		for(int j = 0; j < maxTerrainSide; ++j){
			if(!tiles[i][j])
				continue;
			
			if(!mutex_terrain.try_lock())
				continue;
			
			TileHolder* tile = tiles[i][j];
			if(tiles[i][j] && tile->flags & LOADING){
				mutex_terrain.unlock();
				continue;
			}
			// Same LOD algorithm calculation as last section
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
			
			// If the new calculated LOD is not the same as the loaded one,
			// then we need to upgrade or downgrade the terrain mesh
			//std::cout << "LODs: " << tile->LOD << " to " << newLOD << std::endl;
			if(newLOD != tile->LOD){
				tile->previousLOD = tile->LOD;
				tile->LOD = newLOD;
				
				std::cout << "update terrain LOD: " << (tile->LOD) << std::endl;
				
				// POSSIBLE BUG:
				// Tile is not set as LOADING, therefore it is still updated and rendered
				// This is fine, but at the same time, feels risky..
				
				tile->flags |= (SET | LOADING);
				loadedTerrains.erase(std::remove(loadedTerrains.begin(), loadedTerrains.end(), tile), loadedTerrains.end());

				// POSSIBLE BUG:
				// What happens when a LOADING terrain goes out of range?
				// Does it get deleted without the loader noticing (hence the SEGFAULTs?)
				
				// Tell the loader (TerrainQueue) to load the terrain
				// Since the last LOD isn't NONE, the loader will call the appropriate
				// subroutine for LOD changes from existing terrain data
				// NOTE: This is a query, the terrain will later be added to the render
				// vector once the TerrainQueue loader has finished
				queue.addTerrain(tile);
				//generate(tile, i, j);

				// Add terrain back to potentially visible set
				//loadedTerrains.push_back(tile);
				
				tile->flags &= (~LOADING);
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
			if(!tiles[i][j])
				continue;
			
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// Render all terrains
	for(TileHolder* tile : loadedTerrains){
		assert(tile);
		
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

			assert(tile->terrain);
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
*/

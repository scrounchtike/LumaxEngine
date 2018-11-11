
#ifndef LEVEL_WOLF_TPP
#define LEVEL_WOLF_TPP

#include "Enemy.hpp"
#include "MedKit.hpp"
#include "Door.hpp"
#include "../RML/ResourceManager.hpp"

template <typename ECS>
struct LevelWolfUpdateSystem : public System<ECS, LevelWolfComponent, Material, Model3D>{
	static void update(LevelWolfComponent* level, Material* material, Model3D* model){
		if(!model){
			const float tileDepth = 1;
			const float tileWidth = 1;
			const float tileHeight = 1;
			const float scale = 0.0825f;
			
			const float SIZEY = scale;
			const float SIZEX = (float)((double) SIZEY / (1.037974683544 * 1.5f));
			
			// Load map model from bitmap
			Bitmap* bitmap = LevelWolf::loadBitmap(level->filename);

			std::vector<Vec3> vertices;
			std::vector<int> indices;
			std::vector<Vec2> texCoords;
			std::vector<Vec3> normals;
			
			// TODO: Rename numTexX to be more precise
			// numTexX -> log2(numTextures)
			int numTexX = 4;
			int numTex = (int) std::pow(2, numTexX);
			
			for(int i = 0; i < bitmap->width; ++i){
				for(int j = 0; j < bitmap->height; ++j){
					//std::cout << bitmap->pixels[i + j * bitmap->width] << std::endl;
					// Black pixel
					if((bitmap->pixels[i + j * bitmap->width] & 0xFFFFFF) == 0)
						continue;
					//std::cout << "Colored pixel" << std::endl;
					
					int texX = (int)((bitmap->pixels[i + j * bitmap->width] & 0x00FF00) >> 8) / (int)numTex;
					int texY = texX % numTexX;
					texX /= numTexX;
					
					float xHigher = 1.0f - (float)texX / (float)numTexX;
					float xLower = xHigher - 1.0f / (float)numTexX;
					float yLower = 1.0f - (float)texY / (float)numTexX;
					float yHigher = yLower - 1.0f / (float)numTexX;
					
					// Check if special to add
					uint8 blueValue = (uint8)((bitmap->pixels[i + j * bitmap->width] & 0xFF0000) >> 16);
					int x = i, y = j;
					
					// Door (Hold the door Hodor)
					if(blueValue == 16){
						bool xDoor = false;
						if((bitmap->pixels[x + (y - 1) * bitmap->width] & 0xFFFFFF) == 0)
							if((bitmap->pixels[x + (y + 1) * bitmap->width] & 0xFFFFFF) == 0)
								xDoor = true;
						
						bool yDoor = false;
						if((bitmap->pixels[(x - 1) + y * bitmap->width] & 0xFFFFFF) == 0)
							if((bitmap->pixels[(x + 1) + y * bitmap->width] & 0xFFFFFF) == 0)
								yDoor = true;
						
						if(!(xDoor ^ yDoor)){
							std::cerr << "Error: Door in invalid location" << std::endl;
							assert(false);	
						}
						
						Vec3 openPosition(0,0,0);
						Door door(Vec3(0, 0, 0), openPosition, material->getTexture());
						
						if(yDoor){
							door.transform->translation = Vec3(x, 0, y + tileDepth / 2.0f - Door::DOOR_DEPTH/2.0f);
							door.transform->updateMatrix();
							door.closePosition = Vec3(x, 0, y + tileDepth / 2.0f - Door::DOOR_DEPTH/2.0f);
							door.offset = 0;
							openPosition = door.transform->translation - Vec3(0.9f, 0, 0);
							door.rotated = false;
						}
						
						if(xDoor){
							door.transform->translation = Vec3(x + tileWidth / 2.0f + Door::DOOR_DEPTH/2.0f, 0, y);
							door.closePosition = Vec3(x + tileWidth / 2.0f + Door::DOOR_DEPTH/2.0f, 0, y+1);
							door.offset = 1;
							door.transform->rotation = Vec3(0, (90*M_PI/180.0f), 0);
							door.transform->updateMatrix();
							openPosition = door.transform->translation - Vec3(0, 0, 0.9f-1);
							door.rotated = true;
							
						}
						door.openPosition = openPosition;
						//doors.push_back(door);
						
					}else if(blueValue == 1){
						lmx::getStaticCamera()->position = Vec3((x + 0.5f) * tileWidth, 0.4375f, (y + 0.5f) * tileDepth);
						
					}else if(blueValue == 128){
						/*
						Enemy enemy = Enemy(Enemy::BLUE,
																[=](Vec2 a, Vec2 b, bool c)->Vec2{ return this->checkIntersections(a, b, c); },
																[=](Vec3 a, Vec3 b, float c, float d)->Vec3{ return this->checkCollisions(a, b, c, d); },
																this);
						enemy.transform->translation = Vec3((x + 0.5f) * tileWidth, 0.0f, (y + 0.5f) * tileDepth);
						enemy.transform->updateMatrix();
						//enemies.push_back(enemy);
						
						*/
					}else if(blueValue == 96){
						/*
						Enemy enemy = Enemy(Enemy::WHITE,
																[=](Vec2 a, Vec2 b, bool c)->Vec2{ return this->checkIntersections(a, b, c); },
																[=](Vec3 a, Vec3 b, float c, float d)->Vec3{ return this->checkCollisions(a, b, c, d); },
																this);
						enemy.transform->translation = Vec3((x + 0.5f) * tileWidth, 0.f, (y + 0.5f) * tileDepth);
						enemy.transform->updateMatrix();
						//enemies.push_back(enemy);
						
						*/
					}else if(blueValue == 192){
						MedKit kit = MedKit(Vec3((x + 0.5f) * tileWidth, 0.01, (y + 0.5f) * tileDepth - 1), lmx::getStaticResourceManager()->getTexture("medkit.png"));
						//kits.push_back(kit);
						
					}else if(blueValue == 97){
						//exitPoints.add(new Vector3f((x + 0.5f) * tileWidth, 0, (y + 0.5f) * tileDepth));
						
					}
					
					// Add floor
					//floor
					indices.push_back(vertices.size() + 0);
					indices.push_back(vertices.size() + 1);
					indices.push_back(vertices.size() + 2);
					indices.push_back(vertices.size() + 0);
					indices.push_back(vertices.size() + 2);
					indices.push_back(vertices.size() + 3);
					
					vertices.push_back(Vec3(i * tileWidth, 0, j * tileDepth));
					vertices.push_back(Vec3((i + 1) * tileWidth, 0, j * tileDepth));
					vertices.push_back(Vec3((i + 1) * tileWidth, 0, (j + 1) * tileDepth));
					vertices.push_back(Vec3(i * tileWidth, 0, (j + 1) * tileDepth));
					
					texCoords.push_back(Vec2(xLower, yLower));
					texCoords.push_back(Vec2(xHigher, yLower));
					texCoords.push_back(Vec2(xHigher, yHigher));
					texCoords.push_back(Vec2(xLower, yHigher));
					
					normals.push_back(Vec3(0, 1, 0));
					normals.push_back(Vec3(0, 1, 0));
					normals.push_back(Vec3(0, 1, 0));
					normals.push_back(Vec3(0, 1, 0));
					
					//ceiling
					indices.push_back(vertices.size() + 2);
					indices.push_back(vertices.size() + 1);
					indices.push_back(vertices.size() + 0);
					indices.push_back(vertices.size() + 3);
					indices.push_back(vertices.size() + 2);
					indices.push_back(vertices.size() + 0);
					
					vertices.push_back(Vec3(i * tileWidth, tileHeight, j * tileDepth));
					vertices.push_back(Vec3((i + 1) * tileWidth, tileHeight, j * tileDepth));
					vertices.push_back(Vec3((i + 1) * tileWidth, tileHeight, (j + 1) * tileDepth));
					vertices.push_back(Vec3(i * tileWidth, tileHeight, (j + 1) * tileDepth));
					
					texCoords.push_back(Vec2(xLower, yLower));
					texCoords.push_back(Vec2(xHigher, yLower));
					texCoords.push_back(Vec2(xHigher, yHigher));
					texCoords.push_back(Vec2(xLower, yHigher));
					
					normals.push_back(Vec3(0, -1, 0));
					normals.push_back(Vec3(0, -1, 0));
					normals.push_back(Vec3(0, -1, 0));
					normals.push_back(Vec3(0, -1, 0));
					
					// Walls
					texX = (int)((bitmap->pixels[i + j * bitmap->width] & 0xFF0000) >> 16) / (int)numTex;
					texY = texX % numTexX;
					texX /= numTexX;
					
					xHigher = 1.0f - (float)texX / (float)numTexX;
					xLower = xHigher - 1.0f / (float)numTexX;
					yLower = 1.0f - (float)texY / (float)numTexX;
					yHigher = yLower - 1.0f / (float)numTexX;
					
					//walls
					if((bitmap->pixels[i + (j - 1) * bitmap->width] & 0xFFFFFF) == 0){
						//back wall
						level->collisionPosStart.push_back(Vec2(i * tileWidth, j * tileDepth));
						level->collisionPosEnd.push_back(Vec2((i + 1) * tileWidth, j * tileDepth));
						
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 1);
						indices.push_back(vertices.size() + 0);
						indices.push_back(vertices.size() + 3);
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 0);
						
						vertices.push_back(Vec3(i * tileWidth, 0, j * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, 0, j * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, tileHeight, j * tileDepth));
						vertices.push_back(Vec3(i * tileWidth, tileHeight, j * tileDepth));
						
						texCoords.push_back(Vec2(xLower, yLower));
						texCoords.push_back(Vec2(xHigher, yLower));
						texCoords.push_back(Vec2(xHigher, yHigher));
						texCoords.push_back(Vec2(xLower, yHigher));
						
						normals.push_back(Vec3(0, 0, -1));
						normals.push_back(Vec3(0, 0, -1));
						normals.push_back(Vec3(0, 0, -1));
						normals.push_back(Vec3(0, 0, -1));
						
					}
					
					if((bitmap->pixels[i + (j + 1) * bitmap->width] & 0xffffff) == 0){
						//front wall
						level->collisionPosStart.push_back(Vec2(i * tileWidth, (j + 1) * tileDepth));
						level->collisionPosEnd.push_back(Vec2((i + 1) * tileWidth, (j + 1) * tileDepth));
						
						indices.push_back(vertices.size() + 0);
						indices.push_back(vertices.size() + 1);
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 0);
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 3);
				
						vertices.push_back(Vec3(i * tileWidth, 0, (j + 1) * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, 0, (j + 1) * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, tileHeight, (j + 1) * tileDepth));
						vertices.push_back(Vec3(i * tileWidth, tileHeight, (j + 1) * tileDepth));
						
						texCoords.push_back(Vec2(xLower, yLower));
						texCoords.push_back(Vec2(xHigher, yLower));
						texCoords.push_back(Vec2(xHigher, yHigher));
						texCoords.push_back(Vec2(xLower, yHigher));
						
						normals.push_back(Vec3(0, 0, 1));
						normals.push_back(Vec3(0, 0, 1));
						normals.push_back(Vec3(0, 0, 1));
						normals.push_back(Vec3(0, 0, 1));
						
					}
					
					if((bitmap->pixels[(i - 1) + j * bitmap->width] & 0xffffff) == 0){
						//back wall
						level->collisionPosStart.push_back(Vec2(i * tileWidth, j * tileDepth));
						level->collisionPosEnd.push_back(Vec2(i * tileWidth, (j + 1) * tileDepth));
						
						indices.push_back(vertices.size() + 0);
						indices.push_back(vertices.size() + 1);
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 0);
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 3);
						
						vertices.push_back(Vec3(i * tileWidth, 0, j * tileDepth));
						vertices.push_back(Vec3(i * tileWidth, 0, (j + 1) * tileDepth));
						vertices.push_back(Vec3(i * tileWidth, tileHeight, (j + 1) * tileDepth));
						vertices.push_back(Vec3(i * tileWidth, tileHeight, j * tileDepth));
						
						texCoords.push_back(Vec2(xLower, yLower));
						texCoords.push_back(Vec2(xHigher, yLower));
						texCoords.push_back(Vec2(xHigher, yHigher));
						texCoords.push_back(Vec2(xLower, yHigher));
						
						normals.push_back(Vec3(1, 0, 0));
						normals.push_back(Vec3(1, 0, 0));
						normals.push_back(Vec3(1, 0, 0));
						normals.push_back(Vec3(1, 0, 0));
						
					}
					
					if((bitmap->pixels[(i + 1) + j * bitmap->width] & 0xffffff) == 0){
						//back wall
						level->collisionPosStart.push_back(Vec2((i + 1) * tileWidth, j * tileDepth));
						level->collisionPosEnd.push_back(Vec2((i + 1) * tileWidth, (j + 1) * tileDepth));
						
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 1);
						indices.push_back(vertices.size() + 0);
						indices.push_back(vertices.size() + 3);
						indices.push_back(vertices.size() + 2);
						indices.push_back(vertices.size() + 0);
						
						vertices.push_back(Vec3((i + 1) * tileWidth, 0, j * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, 0, (j + 1) * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, tileHeight, (j + 1) * tileDepth));
						vertices.push_back(Vec3((i + 1) * tileWidth, tileHeight, j * tileDepth));
						
						texCoords.push_back(Vec2(xLower, yLower));
						texCoords.push_back(Vec2(xHigher, yLower));
						texCoords.push_back(Vec2(xHigher, yHigher));
						texCoords.push_back(Vec2(xLower, yHigher));
						
						normals.push_back(Vec3(-1, 0, 0));
						normals.push_back(Vec3(-1, 0, 0));
						normals.push_back(Vec3(-1, 0, 0));
						normals.push_back(Vec3(-1, 0, 0));
					}
				}
			}

			// Create map model
			std::vector<float> vertices_;
			vertices_.resize(vertices.size() * 3);
			std::vector<float> texCoords_;
			texCoords_.resize(texCoords.size() * 2);
			std::vector<float> normals_;
			normals_.resize(normals.size() * 3);
			
			for(int i = 0; i < vertices.size(); ++i){
				vertices_[i*3+0] = vertices[i].x;
				vertices_[i*3+1] = vertices[i].y;
				vertices_[i*3+2] = vertices[i].z;
			}
			for(int i = 0; i < texCoords.size(); ++i){
				texCoords_[i*2+0] = texCoords[i].x;
				texCoords_[i*2+1] = texCoords[i].y;
			}
			for(int i = 0; i < normals.size(); ++i){
				normals_[i*3+0] = normals[i].x;
				normals_[i*3+1] = normals[i].y;
				normals_[i*3+2] = normals[i].z;
			}
			
			// Mesh Creation
			Model3D* map = new Model3D(vertices_, indices, texCoords_, normals_);
			//lmx::getCurrentLevel()->addComponent<Model3D>(LevelType::current_entity, map);
		}
		
		// Update
	}
};

#endif

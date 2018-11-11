
#include "LevelWolf.hpp"

#include "../math.hpp"
#include <vector>
#include <cmath>
#include <functional>

#include "../RL/Model3D.hpp"
#include "../RL/Model2D.hpp"
#include "../RL/ShaderPipeline.hpp"

//#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

#include "../RL/Renderer.hpp"

#include "../RML/ResourceManager.hpp"

#include "Door.hpp"
#include "MedKit.hpp"
#include "Enemy.hpp"

#include <float.h>

using namespace std::placeholders;

unsigned int VBO, VAO;

const float tileDepth = 1;
const float tileWidth = 1;
const float tileHeight = 1;

const float scale = 0.0825f;
	
const float SIZEY = scale;
const float SIZEX = (float)((double) SIZEY / (1.037974683544 * 1.5f));

const float OFFSET_X = 0.0f;
const float OFFSET_Y = 0.0f;

const float texMaxX = -1.0f - OFFSET_X;
const float texMinX = -OFFSET_X;

const float texMaxY = 1.0f - OFFSET_Y;
const float texMinY = -OFFSET_Y;

const float gunOffset = -0.0975f;

const float PLAYER_SIZE = 0.2f;

const float damageMin = 30;
const float damageMax = 40;

float LevelWolf::getDamage(){
	return (rand() % (int)(damageMax - damageMin)) + damageMin;
}

bool intersectRayRectangle(Enemy* enemy, Vec3 o, Vec3 d, Vec3& intersect){
	float width = Enemy::SIZEX - 0.22f;
	float height = Enemy::SIZEY - 0.15f;
	Vec3 P1(-width, 0, 0);
	Vec3 P2(-width, height, 0);
	Vec3 P3(width-0.04, height, 0);
	Vec3 P4(width-0.04, 0, 0);
	P1 = Mat4::mulPoint(*enemy->transform->getTransformation(), P1);
	P2 = Mat4::mulPoint(*enemy->transform->getTransformation(), P2);
	P3 = Mat4::mulPoint(*enemy->transform->getTransformation(), P3);
	P4 = Mat4::mulPoint(*enemy->transform->getTransformation(), P4);
	
	intersect = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);

	Vec3 normal = cross((P2-P1), (P3-P1)).normalize();
	//std::cout << "normal = ";
	//normal.print();
	//std::cout << std::endl;
	
	float denom = dot(normal, d);
	//std::cout << "denom = " << denom << std::endl;
	if(std::abs(denom) > 0.0001f){
		//std::cout << "test" << std::endl;
		float t =  dot((P1 - o), normal) / denom;
		//std::cout << "t = " << t << std::endl;
		if(t > 0){
			//std::cout << "hit plane" << std::endl;
			Vec3 hitPoint = o + d * t;
			intersect = hitPoint;
			Vec3 V1 = (P2-P1).normalize();
			Vec3 V2 = (P3-P2).normalize();
			Vec3 V3 = (P4-P3).normalize();
			Vec3 V4 = (P1-P4).normalize();
			Vec3 V5 = (hitPoint - P1).normalize();
			Vec3 V6 = (hitPoint - P2).normalize();
			Vec3 V7 = (hitPoint - P3).normalize();
			Vec3 V8 = (hitPoint - P4).normalize();
			if(dot(V1, V5) < 0.0) return false;
			if(dot(V2, V6) < 0.0) return false;
			if(dot(V3, V7) < 0.0) return false;
			if(dot(V4, V8) < 0.0) return false;
			return true;
		}
	}
	return false;
}

Vec2 findNearestVec2(Vec2 a, Vec2 b, Vec2 start){
	if((a - start).getLength() > (b - start).getLength())
		return b;
	return a;
}

LevelWolf::LevelWolf(const std::string& filename){
	// Load bitmap
	bitmap = loadBitmap(filename);
	
	// Textures
	textureAtlas = lmx::getStaticResourceManager()->getTexture("WolfCollection2.png");
	gunTexture = lmx::getStaticResourceManager()->getTexture("gun.png");
	gunFiringTexture = lmx::getStaticResourceManager()->getTexture("gunFiring.png");
	
	// Aim aide
	float verticesAim[] = {
		-1,-1,-1,1,1,1,1,-1
	};
	int indicesAim[] = {
		0,1,2,2,3,0
	};
	float texCoordsAim[] = {
		0,1,2,2,3,0
	};
	aim = new Model2D(verticesAim, 6, indicesAim, 6, texCoordsAim);
	
	// Gun model
	float verticesGun[] = {
		-1,-1,-1,1,1,1,1,-1
	};
	int indicesGun[] = {
		0,1,2,2,3,0
	};
	float texCoordsGun[] = {
		0,1,0,0,1,0,1,1
	};
	gun = new Model2D(verticesGun, 8, indicesGun, 6, texCoordsGun);
	gunTransform = new Transform3D(Vec3(-0.175, -0.51f, 0), Vec3(), Vec3(0.76f,0.76f,0.76f));
	
	// Camera
	camera = new Camera();
	
	// Init shader pipeline
	shader = new ShaderPipeline(new Shader("shaders/GLSL/wolfeinstein/shader3D"));
	shader2D = new ShaderPipeline(new Shader("shaders/GLSL/wolfeinstein/shader2D"));
	
	// TODO: Better way to store mesh data?
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
				Door door(Vec3(0, 0, 0), openPosition, textureAtlas);
				
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
				doors.push_back(door);
				
			}else if(blueValue == 1){
				camera->position = Vec3((x + 0.5f) * tileWidth, 0.4375f, (y + 0.5f) * tileDepth);
				
			}else if(blueValue == 128){
				Enemy enemy = Enemy(Enemy::BLUE,
														[=](Vec2 a, Vec2 b, bool c)->Vec2{ return this->checkIntersections(a, b, c); },
														[=](Vec3 a, Vec3 b, float c, float d)->Vec3{ return this->checkCollisions(a, b, c, d); },
														this);
				enemy.transform->translation = Vec3((x + 0.5f) * tileWidth, 0.0f, (y + 0.5f) * tileDepth);
				enemy.transform->updateMatrix();
				enemies.push_back(enemy);
				
			}else if(blueValue == 96){
				Enemy enemy = Enemy(Enemy::WHITE,
														[=](Vec2 a, Vec2 b, bool c)->Vec2{ return this->checkIntersections(a, b, c); },
														[=](Vec3 a, Vec3 b, float c, float d)->Vec3{ return this->checkCollisions(a, b, c, d); },
														this);
				enemy.transform->translation = Vec3((x + 0.5f) * tileWidth, 0.f, (y + 0.5f) * tileDepth);
				enemy.transform->updateMatrix();
				enemies.push_back(enemy);
				
			}else if(blueValue == 192){
				MedKit kit = MedKit(Vec3((x + 0.5f) * tileWidth, 0.01, (y + 0.5f) * tileDepth - 1), lmx::getStaticResourceManager()->getTexture("medkit.png"));
				kits.push_back(kit);
				
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
				collisionPosStart.push_back(Vec2(i * tileWidth, j * tileDepth));
				collisionPosEnd.push_back(Vec2((i + 1) * tileWidth, j * tileDepth));
				
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
				collisionPosStart.push_back(Vec2(i * tileWidth, (j + 1) * tileDepth));
				collisionPosEnd.push_back(Vec2((i + 1) * tileWidth, (j + 1) * tileDepth));
				
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
				collisionPosStart.push_back(Vec2(i * tileWidth, j * tileDepth));
				collisionPosEnd.push_back(Vec2(i * tileWidth, (j + 1) * tileDepth));
				
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
				collisionPosStart.push_back(Vec2((i + 1) * tileWidth, j * tileDepth));
				collisionPosEnd.push_back(Vec2((i + 1) * tileWidth, (j + 1) * tileDepth));
				
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
	
	// Map Generation
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
	map = new Model3D(vertices_, indices, texCoords_, normals_);
}

void LevelWolf::damage(float amt){
	health -= amt;

	if(health < 0)
		std::cout << "You died" << std::endl, assert(false);
}

void LevelWolf::initLevel(const std::string &filename, Camera* camera){
}

void LevelWolf::openDoors(Vec3 position){
	for(Door& door : doors){
		Vec3 distVec = (door.transform->translation - position);
		float distanceSquared = dot(distVec, distVec);
		if(distanceSquared < Door::OPEN_DISTANCE_SQUARED)
			door.open();
	}
}

// 2D Collision Detection
bool LevelWolf::lineIntersectRect(Vec2 lineStart, Vec2 lineEnd, Vec2 rectPos, Vec2 rectSize, Vec2 &result){
	result = Vec2(FLT_MAX, FLT_MAX);
	
	Vec2 collisionVector;
	bool col = lineIntersect(lineStart, lineEnd, rectPos, Vec2(rectPos.x + rectSize.x, rectPos.y), collisionVector);
	if(col)
		result = findNearestVec2(result, collisionVector, lineStart);

	col = lineIntersect(lineStart, lineEnd, rectPos, Vec2(rectPos.x, rectPos.y + rectSize.y), collisionVector);
	if(col)
		result = findNearestVec2(result, collisionVector, lineStart);

	col = lineIntersect(lineStart, lineEnd, Vec2(rectPos.x, rectPos.y + rectSize.y), rectPos + rectSize, collisionVector);
	if(col)
		result = findNearestVec2(result, collisionVector, lineStart);

	col = lineIntersect(lineStart, lineEnd, Vec2(rectPos.x + rectSize.x, rectPos.y), rectPos + rectSize, collisionVector);
	if(col)
		result = findNearestVec2(result, collisionVector, lineStart);
	
	if(result.x == result.y == FLT_MAX)
		return false;

	//std::cout << "collision" << std::endl;
	return true;
}

bool LevelWolf::lineIntersect(Vec2 lineStart1, Vec2 lineEnd1, Vec2 lineStart2, Vec2 lineEnd2, Vec2& result){
	Vec2 line1 = lineEnd1 - lineStart1;
	Vec2 line2 = lineEnd2 - lineStart2;
	
	float cross = cross2D(line1, line2);
	
	if(cross == 0)
		return false;
	
	Vec2 distanceBetweenLineStarts = lineStart2 - lineStart1;
	
	float a = cross2D(distanceBetweenLineStarts, line2) / cross;
	float b = cross2D(distanceBetweenLineStarts, line1) / cross;
	
	if(0.0f < a && a < 1.0f && 0.0f < b && b < 1.0f){
		result = lineStart1 + line1 * a;
		return true;
	}
	
	return false;
}

Vec2 LevelWolf::checkIntersections(Vec2 lineStart, Vec2 lineEnd, bool hurtMonsters){
	Vec2 nearestIntersection = Vec2(FLT_MAX, FLT_MAX);

	// Walls
	for(int i = 0; i < collisionPosStart.size(); ++i){
		Vec2 collisionVector;
		bool col = lineIntersect(lineStart, lineEnd, collisionPosStart[i], collisionPosEnd[i], collisionVector);
		if(col && ((nearestIntersection - lineStart).getLength() > (collisionVector - lineStart).getLength()))
			{
				//std::cout << "test" << std::endl;
				//std::cout << collisionVector.x << "   " << collisionVector.y << std::endl;
				nearestIntersection = collisionVector;
			}
	}
	
	// Doors
	for(Door& door : doors){
		Vec2 doorPos(door.transform->translation.x, door.transform->translation.z);
		Vec2 collisionVector;
		bool col = lineIntersect(lineStart, lineEnd, doorPos, doorPos + door.getDoorSize(), collisionVector);
		if(!col)
			continue;
		nearestIntersection = findNearestVec2(nearestIntersection, collisionVector, lineStart);
	}

	if(hurtMonsters){
		Vec2 nearestMonsterIntersect(FLT_MAX, FLT_MAX);
		Enemy* nearestEnemy = nullptr;

		for(Enemy& enemy : enemies){
			//Enemy& enemy = enemies[0];
			
			//Vec2 enemySize(Enemy::WIDTH, Enemy::DEPTH);
			//Vec2 enemyPos(enemy.transform->translation.x, enemy.transform->translation.z);
			//Vec2 collisionVector;
			//bool col = lineIntersectRect(lineStart, lineEnd, enemyPos, enemySize, collisionVector);
			
			//std::cout << col << std::endl;
		  Vec3 intersect;
			bool col = intersectRayRectangle(&enemy, camera->getPosition(), camera->forward, intersect);

			Vec2 collisionVector(FLT_MAX, FLT_MAX);
			if(col){
				collisionVector = Vec2(intersect.x, intersect.z);
				nearestMonsterIntersect = findNearestVec2(nearestMonsterIntersect, collisionVector, lineStart);
			}

			if(col && nearestMonsterIntersect.x == collisionVector.x &&
				 nearestMonsterIntersect.y == collisionVector.y)
				nearestEnemy = &enemy;
		}

		if(nearestEnemy && nearestEnemy->state != Enemy::STATE_DEAD &&
			 (nearestIntersection.x == FLT_MAX ||
				((nearestMonsterIntersect - lineStart).getLength() < (nearestIntersection - lineStart).getLength()))){
			std::cout << "Shot the enemy!" << std::endl;
			nearestEnemy->damage(getDamage());
		}

		// Ray line
		Vec2 intersection = findNearestVec2(nearestMonsterIntersect, nearestIntersection, lineStart);
		//rayStart = lineStart;
		//rayEnd = lineEnd;
	}

	return nearestIntersection;
}

Vec3 LevelWolf::checkCollisions(Vec3 oldPos, Vec3 newPos, float objectWidth, float objectLength){
	Vec2 collisionVector(1,1);
	Vec3 movementVector = newPos - oldPos;

	if(movementVector.isNull())
		return Vec3(movementVector.x, 0, movementVector.y);

	Vec2 blockSize = Vec2(1,1);
	Vec2 objectSize = Vec2(objectWidth, objectLength);
	Vec2 oldPos2 = Vec2(oldPos.x, oldPos.z);
	Vec2 newPos2 = Vec2(newPos.x, newPos.z);

	// BAD CODE
	for(int i = 0; i < bitmap->width; ++i){
		for(int j = 0; j < bitmap->height; ++j){
			if((bitmap->pixels[i + j * bitmap->width] & 0xFFFFFF) == 0){
				Vec2 col = rectCollide(oldPos2,
															 newPos2, 
															 objectSize,
															 blockSize.mul(Vec2(i, j)),
															 blockSize);
				collisionVector = collisionVector.mul(col);
			}
		}
	}
	// Door collisions
	for(Door& door : doors){
		float offset = Door::DOOR_DEPTH/2.0f;
		float xOffset = door.rotated ? offset : 0;
		float zOffset = door.rotated ? 0 : offset;
		collisionVector =
			collisionVector.mul(
				rectCollide(
										oldPos2,
										newPos2,
										objectSize,
										Vec2(door.transform->translation.x-xOffset, door.transform->translation.z+zOffset),
										door.getDoorSize()));
		
	}
	
	return Vec3(collisionVector.x, 0, collisionVector.y);
}

Vec2 LevelWolf::rectCollide(Vec2 oldPos, Vec2 newPos, Vec2 size1, Vec2 pos2, Vec2 size2){
	Vec2 result(0,0);

	if(newPos.x + size1.x < pos2.x ||
		 newPos.x - size1.x > pos2.x + size2.x * size2.x ||
		 oldPos.y + size1.y < pos2.y ||
		 oldPos.y - size1.y > pos2.y + size2.y * size2.y)
		result.x = 1;
	
	if(oldPos.x + size1.x < pos2.x ||
		 oldPos.x - size1.x > pos2.x + size2.x * size2.x ||
		 newPos.y + size1.y < pos2.y ||
		 newPos.y - size1.y > pos2.y + size2.y * size2.y)
		result.y = 1;
	
	return result;
}

void LevelWolf::input(){
	float speed = 0.05f;
	Vec3 delta = Vec3(0,0,0);
	if(Input::keyDown(LMX_KEY_W))
		delta += camera->walk * speed;
	if(Input::keyDown(LMX_KEY_S))
		delta -= camera->walk * speed;
	if(Input::keyDown(LMX_KEY_A))
		delta -= cross(camera->walk, camera->yAxis).normalize() * speed;
	if(Input::keyDown(LMX_KEY_D))
		delta += cross(camera->walk, camera->yAxis).normalize() * speed;
	if(Input::keyDown(LMX_KEY_SPACE))
		delta += camera->yAxis * speed;
	if(Input::keyDown(LMX_KEY_LSHIFT))
		delta -= camera->yAxis * speed;
	
	Vec3 oldPos = camera->getPosition();
	Vec3 newPos = camera->getPosition() + delta;
	Vec3 collisionVector = checkCollisions(oldPos, newPos, PLAYER_SIZE, PLAYER_SIZE);

	delta = delta.mul(collisionVector);
	
	camera->update(delta);
}

float fireTime = 0;
bool fireUpdate = false;

void LevelWolf::update(){
	// Open doors
	if(Input::keyDown(LMX_KEY_E))
		openDoors(camera->getPosition());
	
	// Shooting
	if(Input::mouseDownOnce(0)){
		Vec2 lineStart(camera->getPosition().x, camera->getPosition().z);
		Vec2 castDirection = Vec2(camera->walk.x, camera->walk.z);
		Vec2 lineEnd = lineStart + castDirection * 1000.0f;

		checkIntersections(lineStart, lineEnd, true);
		fireUpdate = true;
	}

	if(fireUpdate){
		fireTime++;
		if(fireTime >= 5)
			fireUpdate = false, fireTime = 0;
	}

	// Update entities
	for(Door& door : doors)
		door.update();

	for(Enemy& enemy : enemies)
		enemy.update(camera);
	//enemies[0].update(camera);
		
	for(MedKit& kit : kits)
		kit.update(camera);
}

void LevelWolf::render() const {
	lmx::setFaceCulling(true);
	
	shader->bind();
	
	// Bind Texture
	textureAtlas->bind();
	
	// Bind MVP UBO
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
	static Mat4* identity = new Mat4();
	identity->initIdentity();
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, identity->getHeadPointer());
	
	Model3D* cube = lmx::getStaticResourceManager()->getModel3D("cube.obj");
	//cube->render();
	
	//map->render();

	//glBindVertexArray(map->model->vaoID);
	//glDrawArrays(GL_TRIANGLES, 0, 9);
	map->render();
	
	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	
	// Render doors
	for(const Door& door : doors)
		door.render();
	
	// Render enemies
	for(const Enemy& enemy : enemies)
		enemy.render();
	//enemies[0].render();
	
	// Render kits
	for(const MedKit& kit : kits)
		kit.render();
	
	// Render player
	lmx::setDepthClip(false);
	shader2D->bind();

	if(fireUpdate)
		gunFiringTexture->bind();
	else
		gunTexture->bind();
	
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboTransform2D);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, gunTransform->getTransformation()->getHeadPointer(), GL_DYNAMIC_DRAW);

	gun->render();

	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboTransform2D);
	static Transform3D aimTransform(Vec3(), Vec3(), Vec3(0.01, 0.01, 0.01));
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, aimTransform.getTransformation()->getHeadPointer(), GL_DYNAMIC_DRAW);

	textureAtlas->bind();
	
	aim->render();
	lmx::setDepthClip(true);
}

Bitmap* LevelWolf::loadBitmap(const std::string &filename){
	
	int32 width, height, numComponents;
	uint8* data = stbi_load(filename.c_str(), &width, &height, &numComponents, 3);
	std::cout << "comps = " << numComponents << std::endl;

	Bitmap* bitmap = new Bitmap();
	bitmap->width = width;
	bitmap->height = height;
	bitmap->pixels = new uint32[width * height];
	for(int i = 0; i < height; ++i){
		for(int j = 0; j < width; ++j){
			//bitmap->pixels[j + i * width] = ((uint32)data[(j + i * width)+0] << 16) + ((uint32)data[(j + i * width)+1] << 8) + ((uint32)data[(j + i * width)+2]);
			bitmap->pixels[j + i * width] = ((uint32)data[3*(j + i * width)+0]) + ((uint32)data[3*(j + i * width)+1] << 8) + ((uint32)data[3*(j + i * width)+2] << 16);
		}
	}

	stbi_image_free(data);

	return bitmap;
}


#ifndef LEVEL_WOLF_HPP
#define LEVEL_WOLF_HPP

#include <string>

#include "../math/types.hpp"
#include "../RL/Camera.hpp"
#include "../RL/Texture.hpp"
#include "../lumax.hpp"

class Door;
class MedKit;
class Enemy;

class Model3D;
class Model2D;
class ShaderPipeline;

struct Bitmap {
	uint32 width, height;
	uint32* pixels;
};

class LevelWolf{
public:
	LevelWolf(const std::string& filename);
	void initLevel(const std::string& filename, Camera* camera);
	
	void input();
	void update();
	void render() const;
	
public:
	void openDoors(Vec3 position);
	
	static bool lineIntersectRect(Vec2 lineStart, Vec2 lineEnd, Vec2 rectPos, Vec2 rectSize, Vec2& result);
	static bool lineIntersect(Vec2 lineStart1, Vec2 lineEnd1, Vec2 lineStart2, Vec2 lineEnd2, Vec2& result);

	static float getDamage();
	void damage(float amt);
	
	Vec2 checkIntersections(Vec2 lineStart, Vec2 lineEnd, bool hurtMonsters);
	
	Vec3 checkCollisions(Vec3 oldPos, Vec3 newPos, float objectWidth, float objectLength);
	Vec2 rectCollide(Vec2 oldPos, Vec2 newPos, Vec2 size1, Vec2 pos2, Vec2 size2);
	
	static Bitmap* loadBitmap(const std::string& filename);
	
	Texture* textureAtlas;
	Camera* camera;
	ShaderPipeline* shader;
	ShaderPipeline* shader2D;
	
	// Collision detection
	std::vector<Vec2> collisionPosStart;
	std::vector<Vec2> collisionPosEnd;
	
	// Entities
	std::vector<Door> doors;
	std::vector<MedKit> kits;
	std::vector<Enemy> enemies;
	
	Bitmap* bitmap;
	Model3D* map;

	// Player
	float health = 100;
	Model2D* gun;
	Texture* gunTexture;
	Texture* gunFiringTexture;
	Transform3D* gunTransform;
	
	Model2D* aim;

	// TEST
	Vec3 rayStart;
	Vec3 rayEnd;
	Vec3 collisionPos;
};

struct LevelWolfComponent {
	std::vector<Vec2> collisionPosStart;
	std::vector<Vec2> collisionPosEnd;

	std::string filename;

	LevelWolfComponent(const std::string& filename) : filename(filename){ }
};

#include "LevelWolf.tpp"

#endif

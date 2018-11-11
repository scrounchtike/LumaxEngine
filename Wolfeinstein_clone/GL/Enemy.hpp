
#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <vector>

#include "../math.hpp"

//#include "LevelWolf.hpp"
#include "../lumax.hpp"

#include "../RL/Camera.hpp"

#include <functional>
#include <chrono>
using namespace std::chrono;

class Texture;
class Model3D;
class Camera;
class LevelWolf;

class Enemy {
public:
	static float SCALE;

	static float SIZEY;
	static float SIZEX;
	
	static float OFFSET_X;
	static float OFFSET_Y;
	
	static float TEX_MAX_X;
	static float TEX_MIN_X;
	static float TEX_MAX_Y;
	static float TEX_MIN_Y;

	static float WIDTH;
	static float DEPTH;
	
	static float SHOOT_DISTANCE;
	static float SHOT_FACTOR;
	static float ATTACK_CHANCE;

	// Animations
	static std::vector<Texture*> animationsBlue;
	static std::vector<Texture*> animationsWhite;
	static std::vector<Texture*> animationsMutant;
	static std::vector<Texture*> animationsColonel;
	static std::vector<Texture*> animationsOfficer;
	static std::vector<Texture*> animationsBoss;
	std::vector<Texture*>* animations;
	Texture* texture;

	LevelWolf* level;

	enum STATE {
		STATE_IDLE, STATE_CHASING, STATE_ATTACKING, STATE_DYING, STATE_DEAD
	};
	STATE state;
	bool canLook = false;
	bool canAttack = false;
	
	float deathTime = 0;
	
	float moveSpeed = 0.02f;
	float stopMoveDistance = 1.5f;
	
	int damageMax = 30;
	int damageMin = 5;
	
	float health = 100.0f;
	
	float deadY;

	std::function<Vec3(Vec3, Vec3, float, float)> checkCollisions;
	std::function<Vec2(Vec2, Vec2, bool)> checkIntersections;
	
	enum TYPE {
		UNDEFINED, BLUE, WHITE, MUTANT, COLONEL, OFFICER, BOSS
	};
	TYPE type;
	
	Enemy(TYPE type, std::function<Vec2(Vec2, Vec2, bool)> checkIntersections, std::function<Vec3(Vec3, Vec3, float, float)> checkCollisions, LevelWolf* level);

	void damage(float amt);
	
	void update_idle(Vec3 orientation, float distance, Camera* camera);
	void update_chasing(Vec3 orientation, float distance, Camera* camera);
	void update_attacking(Vec3 orientation, float distance, Camera* camera);
	void update_dying(Vec3 orientation, float distance, Camera* camera);
	void update_dead(Vec3 orientation, float distance, Camera* camera);

	void faceCamera(Vec3 orientation);

	void update(Camera* camera);
	void render() const;

	static void init();
	static Model3D* mesh;
public:
	
	DynamicTransform3D* transform;
};

struct ENEMY {};

struct EnemyComponent {
	enum TYPE {
		UNDEFINED, BLUE, WHITE, MUTANT, COLONEL, OFFICER, BOSS
	};
	TYPE type = UNDEFINED;
	enum STATE {
		STATE_IDLE, STATE_CHASING, STATE_ATTACKING, STATE_DYING, STATE_DEAD
	};
	STATE state = STATE_IDLE;

	//static std::vector<std::vector<Texture*>> animations;
	static Texture*** animations;
	
	bool canLook = false;
	bool canAttack = false;
	
	float deathTime = 0;
	
	float moveSpeed = 0.02f;
	float stopMoveDistance = 1.5f;

	float health = 100.0f;
	float deadY = 0;
};

template <typename ECS>
struct EnemyUpdateSystem : public System<ECS, DynamicTransform3D, Material, EnemyComponent>{
	static DynamicTransform3D* transform;
	static Material* material;
	static EnemyComponent* enemy;
	static Camera* camera;
	
	static void update(DynamicTransform3D* transform, Material* material, EnemyComponent* enemy){
		EnemyUpdateSystem::transform = transform;
		EnemyUpdateSystem::material = material;
		EnemyUpdateSystem::enemy = enemy;
		EnemyUpdateSystem::camera = lmx::getStaticCamera();
		
		Camera* camera = lmx::getStaticCamera();
		Vec3 direction = camera->getPosition() - transform->translation;
		float distance = direction.getLength();
		Vec3 orientation = direction / distance;

		// Face the camera
		float angle = (float)(-std::atan2(direction.z, direction.x));
		if(direction.x > 0)
			angle = -(M_PI + angle);
		if(direction.x < 0)
			angle = -angle + M_PI;
		angle += M_PI/2.0f;
		
		transform->rotation = Vec3(0, angle, 0);
		transform->updateMatrix();

		switch(enemy->state){
		case EnemyComponent::STATE_IDLE:
			update_idle(orientation, distance);
			break;
		case EnemyComponent::STATE_CHASING:
			update_chasing(orientation, distance);
			break;
		case EnemyComponent::STATE_ATTACKING:
			update_attacking(orientation, distance);
			break;
		case EnemyComponent::STATE_DYING:
			update_dying(orientation, distance);
			break;
		case EnemyComponent::STATE_DEAD:
			update_dead(orientation, distance);
			break;
		};
	}
	
	static void update_idle(Vec3 orientation, float distance){
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
		float timeDecimals = time - (double)((int)time);
		
		if(timeDecimals < 0.5){
			enemy->canLook = true;
			//Texture** animations = EnemyComponent::animations[(enemy->type)-1];
			//assert(animations);
			//Texture* texture = animations[0];
			//material->setTexture(texture);
			//}else{
			//material->setTexture(EnemyComponent::animations[enemy->type-1][1]);
			/*
			if(enemy->canLook){
				Vec2 lineStart(transform->translation.x, transform->translation.z);
				Vec2 castDirection(orientation.x, orientation.z);
				Vec2 lineEnd = lineStart + castDirection * Enemy::SHOOT_DISTANCE;
				
				//Vec2 collisionVector = checkIntersections(lineStart, lineEnd, false);
				Vec2 playerIntersect(camera->getPosition().x, camera->getPosition().z);
				
				//if((playerIntersect - lineStart).getLength() < (collisionVector - lineStart).getLength())
				//	state = STATE_CHASING;
				
				enemy->canLook = false;
			}
			*/
		}
	}
	
	static void update_chasing(Vec3 orientation, float distance){
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
		float timeDecimals = time - (double)((int)time);
	}
	
	static void update_attacking(Vec3 orientation, float distance){
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
		float timeDecimals = time - (double)((int)time);
	}
	
	static void update_dying(Vec3 orientation, float distance){
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
		float timeDecimals = time - (double)((int)time);
	}
	
	static void update_dead(Vec3 orientation, float distance){
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
		float timeDecimals = time - (double)((int)time);
		
		material->setTexture(EnemyComponent::animations[enemy->type][12]);
	}
};
template <typename E>
DynamicTransform3D* EnemyUpdateSystem<E>::transform = nullptr;
template <typename E>
Material* EnemyUpdateSystem<E>::material = nullptr;
template <typename E>
EnemyComponent* EnemyUpdateSystem<E>::enemy = nullptr;
template <typename E>
Camera* EnemyUpdateSystem<E>::camera = nullptr;

#endif

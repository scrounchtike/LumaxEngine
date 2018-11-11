
#include "Enemy.hpp"

#include "../RL/Texture.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/Camera.hpp"

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"
#include "../RL/Renderer.hpp"

#include "LevelWolf.hpp"

#include <chrono>
using namespace std::chrono;

Texture*** EnemyComponent::animations;

float Enemy::SCALE = 0.75f;

float Enemy::SIZEY = SCALE;
float Enemy::SIZEX = (float)(SIZEY / (1.9310344827586f * 1.2f));

float Enemy::OFFSET_X = 0.0f;
float Enemy::OFFSET_Y = 0.0f;

float Enemy::TEX_MAX_X = -1.0f - OFFSET_X;
float Enemy::TEX_MIN_X = -OFFSET_X;
float Enemy::TEX_MAX_Y = -1.0f - OFFSET_Y;
float Enemy::TEX_MIN_Y = -OFFSET_Y;

float Enemy::WIDTH = 0.2f;
float Enemy::DEPTH = 0.2f;

float Enemy::SHOOT_DISTANCE = 100;
float Enemy::SHOT_FACTOR = 10;
float Enemy::ATTACK_CHANCE = 0.2f;

Model3D* Enemy::mesh;

std::vector<Texture*> Enemy::animationsBlue;
std::vector<Texture*> Enemy::animationsWhite;
std::vector<Texture*> Enemy::animationsMutant;
std::vector<Texture*> Enemy::animationsColonel;
std::vector<Texture*> Enemy::animationsOfficer;
std::vector<Texture*> Enemy::animationsBoss;

Enemy::Enemy(TYPE type, std::function<Vec2(Vec2, Vec2, bool)> checkIntersections, std::function<Vec3(Vec3, Vec3, float, float)> checkCollisions, LevelWolf* level) : type(type), checkIntersections(checkIntersections), checkCollisions(checkCollisions), level(level) {
	deadY = 0;
	
	switch(type){
	case BLUE:
		deadY = -0.07f;
		animations = &animationsBlue;
		break;
	case WHITE:
		animations = &animationsWhite;
		break;
	case MUTANT:
		animations = &animationsMutant;
		break;
	case COLONEL:
		animations = &animationsColonel;
		break;
	case OFFICER:
		animations = &animationsOfficer;
		break;
	case BOSS:
		animations = &animationsBoss;
		break;
	};

	transform = new DynamicTransform3D();
	state = STATE_IDLE;

	// Starting texture
	texture = animations->operator[](0);
}

void Enemy::damage(float amt){
	if(state == STATE_IDLE)
		state = STATE_CHASING;

	health -= amt;

	if(health <= 0)
		state = STATE_DYING;
}

void Enemy::update_idle(Vec3 orientation, float distance, Camera* camera){
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
	float timeDecimals = time - (double)((int)time);
	
	if(timeDecimals < 0.5){
		canLook = true;
		texture = animations->operator[](0);
	}else{
		texture = animations->operator[](1);
		if(canLook){
			Vec2 lineStart(transform->translation.x, transform->translation.z);
			Vec2 castDirection(orientation.x, orientation.z);
			Vec2 lineEnd = lineStart + castDirection * SHOOT_DISTANCE;

			Vec2 collisionVector = checkIntersections(lineStart, lineEnd, false);
			Vec2 playerIntersect(camera->getPosition().x, camera->getPosition().z);

			//std::cout << collisionVector.x << "    " << collisionVector.y << std::endl;
			if((playerIntersect - lineStart).getLength() < (collisionVector - lineStart).getLength()){
				//std::cout << "CHASE" << std::endl;
				state = STATE_CHASING;
			}else{
				//std::cout << "IDLE" << std::endl;
			}
			
			canLook = false;
		}
	}
}

void Enemy::update_chasing(Vec3 orientation, float distance, Camera* camera){
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
	float timeDecimals = time - (double)((int)time);

	if(timeDecimals < 0.25)
		texture = animations->operator[](0);
	else if(timeDecimals < 0.5)
		texture = animations->operator[](1);
	else if(timeDecimals < 0.75)
		texture = animations->operator[](2);
	else
		texture = animations->operator[](3);
	
	if(rand() % 100 < (100 * ATTACK_CHANCE)){
		//canAttack = true;
		//state = STATE_ATTACKING;
	}

	if(distance > stopMoveDistance){
		Vec3 oldPos = transform->translation;
		Vec3 newPos = oldPos + Vec3(orientation.x, 0, orientation.z).normalize() * moveSpeed;

		Vec3 collisionVector = checkCollisions(oldPos, newPos, WIDTH, DEPTH);
		Vec3 movementVector = collisionVector.mul(orientation);

		transform->translation += movementVector * moveSpeed;
		transform->updateMatrix();

		
		
	}else{
		canAttack = true;
		state = STATE_ATTACKING;
	}
	
}

void Enemy::update_attacking(Vec3 orientation, float distance, Camera* camera){
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
	float timeDecimals = time - (double)((int)time);

	if(timeDecimals < 0.25)
		texture = animations->operator[](4);
	else if(timeDecimals < 0.5)
		texture = animations->operator[](5);
	else if(timeDecimals < 0.75){
		texture = animations->operator[](6);
		if(canAttack){
			Vec2 lineStart(transform->translation.x, transform->translation.z);
			Vec2 castDirection = Vec2(orientation.x, orientation.z);
			Vec2 lineEnd = lineStart + castDirection * SHOOT_DISTANCE;

			Vec2 collisionVector = checkIntersections(lineStart, lineEnd, false);

			Vec2 playerIntersect(camera->getPosition().x, camera->getPosition().z);
			
			if((playerIntersect - lineStart).getLength() < (collisionVector - lineStart).getLength() && rand()%100 < 85){ // 85% chance hit
				std::cout << "HIT" << std::endl;
				//level->damage(LevelWolf::getDamage());
			}
			
			canAttack = false;
			if((distance-0.1f) > stopMoveDistance)
				state = STATE_CHASING;
		}
	}
	else{
		if(!canAttack)
			texture = animations->operator[](5);
		canAttack = true;
	}
}

void Enemy::update_dying(Vec3 orientation, float distance, Camera* camera){
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	float time = (float)((long)(ms.count()%1000000)) / 1000.0f;
	float timeDecimals = time - (double)((int)time);
	
	if(deathTime == 0)
		deathTime = time;

	static float time1 = 0.1f;
	static float time2 = 0.3f;
	static float time3 = 0.45f;
	static float time4 = 0.6f;

	if(time < deathTime + time1){
		texture = animations->operator[](8);
		transform->scale = Vec3(1.0f, 0.964285714f, 1.0f);
	}else if(time < deathTime + time2){
		texture = animations->operator[](9);
		transform->scale = Vec3(1.2f, 0.9f, 1.0f);
	}else if(time < deathTime + time3){
		texture = animations->operator[](10);
		transform->scale = Vec3(1.2f, 0.9f, 1.0f);
	}else if(time < deathTime + time4){
		texture = animations->operator[](11);
		transform->scale = Vec3(1.2f, 0.5f, 1.0f);
	}else
		state = STATE_DEAD;

	transform->updateMatrix();
}

void Enemy::update_dead(Vec3 orientation, float distance, Camera* camera){
	texture = animations->operator[](12);
	transform->scale = Vec3(1.258620689655f, 0.785714285714f, 1.0f);
	transform->translation = Vec3(transform->translation.x, deadY, transform->translation.z);
		
}

void Enemy::faceCamera(Vec3 direction){
	float angle = (float)(-std::atan2(direction.z, direction.x));
	if(direction.x > 0)
		angle = -(M_PI + angle);
	if(direction.x < 0)
		angle = -angle + M_PI;
	angle += M_PI/2.0f;

	transform->rotation = Vec3(0, angle, 0);
	transform->updateMatrix();
}

void Enemy::update(Camera* camera){
	Vec3 direction = camera->getPosition() - transform->translation;

	float distance = direction.getLength();
	Vec3 orientation = direction / distance;

	faceCamera(orientation);

	switch(state){
	case STATE_IDLE:
		update_idle(orientation, distance, camera);
		break;
	case STATE_CHASING:
		update_chasing(orientation, distance, camera);
		break;
	case STATE_ATTACKING:
		update_attacking(orientation, distance, camera);
		break;
	case STATE_DYING:
		update_dying(orientation, distance, camera);
		break;
	case STATE_DEAD:
		update_dead(orientation, distance, camera);
		break;
	};
}

void Enemy::render() const {
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());

	texture->bind();

	//lmx::setBlending(false);
	mesh->render();
	//lmx::setBlending(true);
}

void Enemy::init(){
	// Mesh
	float vertices[] = {
		-SIZEX, 0, 0,
		-SIZEX, SIZEY, 0,
		SIZEX, SIZEY, 0,
		SIZEX, 0, 0,
	};
	float texCoords[] = {
		TEX_MAX_X, TEX_MIN_Y,
		TEX_MAX_X, TEX_MAX_Y,
		TEX_MIN_X, TEX_MAX_Y,
		TEX_MIN_X, TEX_MIN_Y,
	};
	int indices[] = {
		0, 1, 2, 0, 2, 3,
	};	
	float normals[] = {
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
	};
			
	mesh = new Model3D(vertices, 12, indices, 6, texCoords, normals);
	
	// Animations
	ResourceManager* resManager = lmx::getStaticResourceManager();
	
	// Animations Blue
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS1.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS2.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS3.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS4.png"));
	
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS5.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS6.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/SS7.png"));
	
	animationsBlue.push_back(resManager->getTexture("blueSoldier/death1.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/wolfLeft.png"));
	
	animationsBlue.push_back(resManager->getTexture("blueSoldier/Death1.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/Death2.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/Death3.png"));
	animationsBlue.push_back(resManager->getTexture("blueSoldier/Death4.png"));
	
	// Animations White
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS1w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS2w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS3w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS4w.png"));
	
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS5w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS6w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/SS7w.png"));
	
	animationsWhite.push_back(resManager->getTexture("whiteGuard/death1w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/Death1w.png"));
	
	animationsWhite.push_back(resManager->getTexture("whiteGuard/Death2w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/Death3w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/Death4w.png"));
	animationsWhite.push_back(resManager->getTexture("whiteGuard/Death5w.png"));

	// Animations Mutant
	animationsMutant.push_back(resManager->getTexture("mutant/SS1m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/SS2m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/SS3m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/SS4m.png"));
	
	animationsMutant.push_back(resManager->getTexture("mutant/SS5m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/SS6m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/SS7m.png"));
	
	animationsMutant.push_back(resManager->getTexture("mutant/death1m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/Death1m.png"));
	
	animationsMutant.push_back(resManager->getTexture("mutant/Death2m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/Death3m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/Death4m.png"));
	animationsMutant.push_back(resManager->getTexture("mutant/Death5m.png"));

	// Animations Colonel
	animationsColonel.push_back(resManager->getTexture("colonel/SS1c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/SS2c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/SS3c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/SS4c.png"));
	
	animationsColonel.push_back(resManager->getTexture("colonel/SS5c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/SS6c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/SS7c.png"));
	
	animationsColonel.push_back(resManager->getTexture("colonel/death1c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/Death1c.png"));
	
	animationsColonel.push_back(resManager->getTexture("colonel/Death2c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/Death3c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/Death4c.png"));
	animationsColonel.push_back(resManager->getTexture("colonel/Death5c.png"));

	// Animations Officer
	animationsOfficer.push_back(resManager->getTexture("officer/SS1o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/SS2o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/SS3o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/SS4o.png"));
	
	animationsOfficer.push_back(resManager->getTexture("officer/SS5o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/SS6o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/SS7o.png"));
	
	animationsOfficer.push_back(resManager->getTexture("officer/death1o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/Death1o.png"));
	
	animationsOfficer.push_back(resManager->getTexture("officer/Death2o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/Death3o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/Death4o.png"));
	animationsOfficer.push_back(resManager->getTexture("officer/Death5o.png"));

	// Animations Boss
	animationsBoss.push_back(resManager->getTexture("boss/SS1b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/SS2b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/SS3b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/SS4b.png"));
	
	animationsBoss.push_back(resManager->getTexture("boss/SS5b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/SS6b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/SS7b.png"));
	
	animationsBoss.push_back(resManager->getTexture("boss/death1b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/Death1b.png"));
	
	animationsBoss.push_back(resManager->getTexture("boss/Death2b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/Death3b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/Death4b.png"));
	animationsBoss.push_back(resManager->getTexture("boss/Death5b.png"));

	std::cout << "Wolf enemy texture initialization" << std::endl;
	EnemyComponent::animations = new Texture**[6];
	EnemyComponent::animations[BLUE-1] = new Texture*[13];
	memcpy(&EnemyComponent::animations[BLUE-1], &animationsBlue[0], sizeof(Texture*)*13);
	EnemyComponent::animations[WHITE-1] = new Texture*[13];
  memcpy(&EnemyComponent::animations[WHITE-1], &animationsWhite[0], sizeof(Texture*)*13);
	EnemyComponent::animations[MUTANT-1] = new Texture*[13];
  memcpy(&EnemyComponent::animations[MUTANT-1], &animationsMutant[0], sizeof(Texture*)*13);
	EnemyComponent::animations[COLONEL-1] = new Texture*[13];
  memcpy(&EnemyComponent::animations[COLONEL-1], &animationsColonel[0], sizeof(Texture*)*13);
	EnemyComponent::animations[OFFICER-1] = new Texture*[13];
  memcpy(&EnemyComponent::animations[OFFICER-1], &animationsOfficer[0], sizeof(Texture*)*13);
	EnemyComponent::animations[BOSS-1] = new Texture*[13];
  memcpy(&EnemyComponent::animations[BOSS-1], &animationsBoss[0], sizeof(Texture*)*13);
	
}

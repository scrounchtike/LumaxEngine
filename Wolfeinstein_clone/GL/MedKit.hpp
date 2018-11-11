
#ifndef MEDKIT_HPP
#define MEDKIT_HPP

#include "../math.hpp"

#include "../core/ECS.hpp"
#include "../lumax.hpp"

#include "../RL/Camera.hpp"

class Texture;
class Model3D;
class DynamicTransform3D;
class Camera;

class MedKit {
public:
	static float PICKUP_DISTANCE;
	static float SCALE;

	static float SIZEY;
	static float SIZEX;

	static float OFFSET_X;
	static float OFFSET_Y;

	static float TEX_MAX_X;
	static float TEX_MIN_X;
	static float TEX_MAX_Y;
	static float TEX_MIN_Y;

	MedKit(Vec3 position, Texture* texture);

	void update(Camera* camera);
	void render() const;
	
private:
	Texture* texture;
	static Model3D* mesh;
public:
	DynamicTransform3D* transform;
};

struct MEDKIT {};

// Update Medkit System
template <typename ECS>
struct MedkitUpdateSystem : public System<ECS, DynamicTransform3D, MEDKIT> {
	static void update(DynamicTransform3D* transform){
		Camera* camera = lmx::camera;

		Vec3 direction = camera->getPosition() - transform->translation;

		float angle = (float)(-std::atan2(direction.z, direction.x));
		if(direction.x > 0)
			angle = -(M_PI + angle);
		if(direction.x < 0)
			angle = -angle + M_PI;
		angle += M_PI/2.0f;

		transform->rotation = Vec3(0, angle, 0);
		transform->updateMatrix();

		// Pickup
	}
};

#endif

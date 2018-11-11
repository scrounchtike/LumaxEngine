
#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <functional>

#include "../math.hpp"
#include "../core/ECS.hpp"
//#include "../lumax.hpp"

struct Func2 {
	Func2(std::function<float(float)> x, std::function<float(float)> y) : x(x), y(y) {}
	std::function<float(float)> x;
	std::function<float(float)> y;
};
struct Func3 {
	Func3(std::function<float(float)> x, std::function<float(float)> y, std::function<float(float)> z) : x(x), y(y), z(z) {}
	std::function<float(float)> x;
	std::function<float(float)> y;
	std::function<float(float)> z;
};

static float function0(float delta) { return 0; }
static float function1(float delta) { return 1; }

struct Movement2D {
	Movement2D(Vec2 translation){
		tx = [translation](float delta){ return translation.x; };
		ty = [translation](float delta){ return translation.y; };
	}
	Movement2D(Vec2 translation, float rotation){
		tx = [translation](float delta){ return translation.x; };
		ty = [translation](float delta){ return translation.y; };
		r = [rotation](float delta){ return rotation; };
	}
	Movement2D(Vec2 translation, float rotation, Vec2 scale){
		tx = [translation](float delta){ return translation.x; };
		ty = [translation](float delta){ return translation.y; };
		r = [rotation](float delta){ return rotation; };
		sx = [scale](float delta){ return scale.x; };
		sy = [scale](float delta){ return scale.y; };
	}
	Movement2D(Func2 translation){
		tx = translation.x;
		ty = translation.y;
	}
	Movement2D(Func2 translation, std::function<float(float)> rotation){
		tx = translation.x;
		ty = translation.y;
		r = rotation;
	}
	Movement2D(Func2 translation, std::function<float(float)> rotation, Func2 scale){
		tx = translation.x;
		ty = translation.y;
		r = rotation;
		sx = scale.x;
		sy = scale.y;
	}
	void setTranslation(Func2 translation){
		tx = translation.x;
		ty = translation.y;
	}
	void setRotation(std::function<float(float)> rotation){
		r = rotation;
	}
	void setScale(Func2 scale){
		sx = scale.x;
		sy = scale.y;
	}
	std::function<float(float)> tx = function0;
	std::function<float(float)> ty = function0;
	std::function<float(float)> r = function0;
	std::function<float(float)> sx = function1;
	std::function<float(float)> sy = function1;
};

struct Movement3D {
	Movement3D() {}
	Movement3D(Vec3 translation){
		tx = [translation](float delta) { return translation.x; };
		ty = [translation](float delta) { return translation.y; };
		tz = [translation](float delta) { return translation.z; };
	}
	Movement3D(Vec3 translation, Vec3 rotation){
		tx = [translation](float delta) { return translation.x; };
		ty = [translation](float delta) { return translation.y; };
		tz = [translation](float delta) { return translation.z; };
		rx = [rotation](float delta) { return rotation.x; };
		ry = [rotation](float delta) { return rotation.y; };
		rz = [rotation](float delta) { return rotation.z; };
	}
	Movement3D(Vec3 translation, Vec3 rotation, Vec3 scale){
		tx = [translation](float delta) { return translation.x; };
		ty = [translation](float delta) { return translation.y; };
		tz = [translation](float delta) { return translation.z; };
		rx = [rotation](float delta) { return rotation.x; };
		ry = [rotation](float delta) { return rotation.y; };
		rz = [rotation](float delta) { return rotation.z; };
		sx = [scale](float delta) { return scale.x; };
		sy = [scale](float delta) { return scale.y; };
		sz = [scale](float delta) { return scale.z; };
	}
	Movement3D(Func3 translation){
		tx = translation.x;
		ty = translation.y;
		tz = translation.z;
	}
	Movement3D(Func3 translation, Func3 rotation){
		tx = translation.x;
		ty = translation.y;
		tz = translation.z;
		rx = rotation.x;
		ry = rotation.y;
		rz = rotation.z;
	}
	Movement3D(Func3 translation, Func3 rotation, Func3 scale){
		tx = translation.x;
		ty = translation.y;
		tz = translation.z;
		rx = rotation.x;
		ry = rotation.y;
		rz = rotation.z;
		sx = scale.x;
		sy = scale.y;
		sz = scale.z;
	}
	void setTranslation(Func3 translation){
		tx = translation.x;
		ty = translation.y;
		tz = translation.z;
	}
	void setRotation(Func3 rotation){
		rx = rotation.x;
		ry = rotation.y;
		rz = rotation.z;
	}
	void setScale(Func3 scale){
		sx = scale.x;
		sy = scale.y;
		sz = scale.z;
	}
	std::function<float(float)> tx = function0;
	std::function<float(float)> ty = function0;
	std::function<float(float)> tz = function0;
	std::function<float(float)> rx = function0;
	std::function<float(float)> ry = function0;
	std::function<float(float)> rz = function0;
	std::function<float(float)> sx = function1;
	std::function<float(float)> sy = function1;
	std::function<float(float)> sz = function1;
};

// Movement2D System
template <typename ECS>
struct MovementSystem2D : public System<ECS, DynamicTransform2D, Movement2D>{
	static void update(DynamicTransform2D* transform, Movement2D* movement){
		static float delta = 0.0f;
		delta += 1.0f / 60.0f;
		transform->translation += Vec2(movement->tx(delta), movement->ty(delta));
		transform->rotation += movement->r(delta);
		transform->scale = transform->scale.mul(Vec2(movement->sx(delta), movement->sy(delta)));
		transform->updateMatrix();
	}
};

// Movement3D System
template <typename ECS>
struct MovementSystem3D : public System<ECS, DynamicTransform3D, Movement3D>{
	static void update(DynamicTransform3D* transform, Movement3D* movement){
		static float delta = 0.0f;
		delta += 1.0f / 60.0f;
		transform->translation += Vec3(movement->tx(delta), movement->ty(delta), movement->tz(delta));
		transform->rotation += Vec3(movement->rx(delta), movement->ry(delta), movement->rz(delta));
		transform->scale = transform->scale.mul(Vec3(movement->sx(delta), movement->sy(delta), movement->sz(delta)));
		transform->updateMatrix();
	}
};

// Movement3D physics system
template <typename ECS>
struct MovementSystemPhysics3D : public System<ECS, PhysicsPrimitive, Movement3D>{
	static void update(PhysicsPrimitive* physics, Movement3D* movement){
		static float delta = 0.0f;
		delta += 1.0f / 60.0f;
		physics->addTranslation(Vec3(movement->tx(delta), movement->ty(delta), movement->tz(delta)));
		physics->addRotation(Vec3(movement->rx(delta), movement->ry(delta), movement->rz(delta)));
		physics->addScale(Vec3(movement->sx(delta), movement->sy(delta), movement->sz(delta)));
	}
};

#endif

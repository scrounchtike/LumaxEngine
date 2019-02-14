
#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <functional>

#include "../math.hpp"
#include "../core/ECS.hpp"
//#include "../lumax.hpp"

#include "../PL/PhysicsPrimitives.hpp"

namespace Movement{

	static float function0(float delta) { return 0; }
	static float function1(float delta) { return 1; }
	static float sin(float delta) { return std::sin(delta); }
	static float cos(float delta) { return std::cos(delta); }
	static float tan(float delta) { return std::tan(delta); }
	
	struct MovFunc{
		MovFunc()
			: function(function0), coeff(1.0), speed(1.0)
		{
		}
		MovFunc(std::function<float(float)> function, float coeff = 1.0, float speed = 1.0)
			: function(function), coeff(coeff), speed(speed)
		{
			
		}
		std::function<float(float)> function;
		float coeff = 1.0;
		float speed = 1.0;
		float operator()(float delta){
			return coeff * function(speed * delta);
		}
	};
	
	struct MovFunc2{
		MovFunc2(MovFunc x, MovFunc y) : x(x), y(y) {}
		MovFunc x, y;
		MovFunc& operator[](int index){
			return index ? y : x;
		}
	};
	struct MovFunc3{
		MovFunc3(MovFunc x, MovFunc y, MovFunc z) : x(x), y(y), z(z) {}
		MovFunc x, y, z;
		MovFunc& operator[](int index){
			if(index == 2)
				return z;
			return index ? y : x;
		}
	};
}

using namespace Movement;

// Add MovFuncs to Movement2D
struct Movement2D {
	Movement2D(Vec2 translation){
		tx = MovFunc([translation](float delta){ return translation.x; });
		ty = MovFunc([translation](float delta){ return translation.y; });
	}
	Movement2D(Vec2 translation, float rotation){
		tx = MovFunc([translation](float delta){ return translation.x; });
		ty = MovFunc([translation](float delta){ return translation.y; });
		r = MovFunc([rotation](float delta){ return rotation; });
	}
	Movement2D(Vec2 translation, float rotation, Vec2 scale){
		tx = MovFunc([translation](float delta){ return translation.x; });
		ty = MovFunc([translation](float delta){ return translation.y; });
		r = MovFunc([rotation](float delta){ return rotation; });
		sx = MovFunc([scale](float delta){ return scale.x; });
		sy = MovFunc([scale](float delta){ return scale.y; });
	}
	Movement2D(MovFunc2 translation){
		tx = translation.x;
		ty = translation.y;
	}
	Movement2D(MovFunc2 translation, MovFunc rotation){
		tx = translation.x;
		ty = translation.y;
		r = rotation;
	}
	Movement2D(MovFunc2 translation, MovFunc rotation, MovFunc2 scale){
		tx = translation.x;
		ty = translation.y;
		r = rotation;
		sx = scale.x;
		sy = scale.y;
	}
	void setTranslation(MovFunc2 translation){
		tx = translation.x;
		ty = translation.y;
	}
	void setRotation(MovFunc rotation){
		r = rotation;
	}
	void setScale(MovFunc2 scale){
		sx = scale.x;
		sy = scale.y;
	}
  MovFunc tx = MovFunc(function0);
	MovFunc ty = MovFunc(function0);
	MovFunc r  = MovFunc(function0);
	MovFunc sx = MovFunc(function0);
	MovFunc sy = MovFunc(function0);
};

struct Movement3D {
	Movement3D() {}
	Movement3D(Vec3 translation){
		tx = MovFunc([translation](float delta) { return translation.x; });
		ty = MovFunc([translation](float delta) { return translation.y; });
		tz = MovFunc([translation](float delta) { return translation.z; });
    hasTranslation = true;
	}
	Movement3D(MovFunc3 translation)
		: tx(translation.x),
			ty(translation.y),
			tz(translation.z),
			hasTranslation(true)
	{
	}
	Movement3D(Vec3 translation, Vec3 rotation){
		tx = MovFunc([translation](float delta) { return translation.x; });
		ty = MovFunc([translation](float delta) { return translation.y; });
		tz = MovFunc([translation](float delta) { return translation.z; });
		rx = MovFunc([rotation](float delta) { return rotation.x; });
		ry = MovFunc([rotation](float delta) { return rotation.y; });
		rz = MovFunc([rotation](float delta) { return rotation.z; });
		hasTranslation = hasRotation = true;
	}
	Movement3D(MovFunc3 translation, MovFunc3 rotation)
	  : tx(translation.x),
			ty(translation.y),
			tz(translation.z),
			rx(rotation.x),
			ry(rotation.y),
			rz(rotation.z),
			hasTranslation(true),
			hasRotation(true)
	{
	}
	Movement3D(Vec3 translation, Vec3 rotation, Vec3 scale){
		tx = MovFunc([translation](float delta) { return translation.x; });
		ty = MovFunc([translation](float delta) { return translation.y; });
		tz = MovFunc([translation](float delta) { return translation.z; });
		rx = MovFunc([rotation](float delta) { return rotation.x; });
		ry = MovFunc([rotation](float delta) { return rotation.y; });
		rz = MovFunc([rotation](float delta) { return rotation.z; });
		sx = MovFunc([scale](float delta) { return scale.x; });
		sy = MovFunc([scale](float delta) { return scale.y; });
		sz = MovFunc([scale](float delta) { return scale.z; });
		hasTranslation = hasRotation = hasScale = true;
	}
	Movement3D(MovFunc3 translation, MovFunc3 rotation, MovFunc3 scale)
		: tx(translation.x),
			ty(translation.y),
			tz(translation.z),
			rx(rotation.x),
			ry(rotation.y),
			rz(rotation.z),
			sx(scale.x),
			sy(scale.y),
			sz(scale.z),
			hasTranslation(true),
			hasRotation(true),
			hasScale(true)
	{
	}
	void setTranslation(MovFunc3 translation){
		hasTranslation = true;
		tx = translation.x;
		ty = translation.y;
		tz = translation.z;
	}
	void setRotation(MovFunc3 rotation){
		hasRotation = true;
		rx = rotation.x;
		ry = rotation.y;
		rz = rotation.z;
	}
	void setScale(MovFunc3 scale){
		hasScale = true;
		sx = scale.x;
		sy = scale.y;
		sz = scale.z;
	}
	MovFunc tx = MovFunc(function0);
	MovFunc ty = MovFunc(function0);
  MovFunc tz = MovFunc(function0);
	MovFunc rx = MovFunc(function0);
	MovFunc ry = MovFunc(function0);
	MovFunc rz = MovFunc(function0);
	MovFunc sx = MovFunc(function1);
	MovFunc sy = MovFunc(function1);
	MovFunc sz = MovFunc(function1);
	bool hasTranslation = false;
	bool hasRotation = false;
	bool hasScale = false;
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
		transform->translation += Vec3(0.01f*movement->tx(delta), movement->ty(delta), movement->tz(delta));
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

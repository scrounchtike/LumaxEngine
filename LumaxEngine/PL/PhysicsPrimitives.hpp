
#ifndef PHYSICS_PRIMITIVES_HPP
#define PHYSICS_PRIMITIVES_HPP

#include "../math.hpp"
#include "../RAL/Log.hpp"

#include <unordered_map>
#include <functional>
#include <cassert>

class RendererGL;

class PhysicsPrimitive {
public:
	virtual void addTranslation(Vec3 translation) { };
	virtual void addRotation(Vec3 rotation) { };
	virtual void addScale(Vec3 scale) { };
	virtual void renderAxes(const RendererGL* renderer);

	bool collidesWith(PhysicsPrimitive* other) {
		auto handler = collisionHandlers.find(getKey(tid, other->tid));
		bool result;

		if (handler != collisionHandlers.end()) {
			result = handler->second(this, other);
		}
		else {
			handler = collisionHandlers.find(getKey(other->tid, tid));
			if (handler == collisionHandlers.end()) {
				// Something went horribly wrong; abort.
				Log::println("Error: Collision detection with physics primitives could not find a valid handler function.");
				assert(false);
				return false;
			}
			result = handler->second(other, this);
		}
		// Use the handler
		return result;
	}

	static void initialize();
protected:
	PhysicsPrimitive(const unsigned int tid) : tid(tid) { }
	const unsigned tid;

	typedef std::function<bool(const PhysicsPrimitive* a, const PhysicsPrimitive* b)> CollisionHandler;
	typedef unsigned long long keyID;
	static std::unordered_map<keyID, CollisionHandler> collisionHandlers;

	static keyID getKey(unsigned tid1, unsigned tid2) {
		return (keyID)(tid1) << 32 | tid2;
	}

	static void addCollisionHandler(unsigned tid1, unsigned tid2, CollisionHandler handler) {
		collisionHandlers.insert(std::pair<unsigned long long, CollisionHandler>(getKey(tid1, tid2), handler));
	}
};

struct AABB : public PhysicsPrimitive {
	Vec3 position;
	Vec3 extents;
	AABB(const Vec3& position, const Vec3& extents) 
		: position(position), extents(extents), PhysicsPrimitive(cid) {
	}
	static void initCases();
	static const unsigned int cid;

	virtual void addTranslation(Vec3 translation) { position += translation; }
	//virtual void addRotation(Vec3 rotation) { assert(false); }
	virtual void addScale(Vec3 scale) { extents = extents.mul(scale); }

	virtual void renderAxes(const RendererGL* renderer);
};

struct Sphere : public PhysicsPrimitive {
	Vec3 position;
	float radius;
	Sphere(const Vec3& position, float radius)
		: position(position), radius(radius), PhysicsPrimitive(cid) {
	}
	static void initCases();
	static const unsigned int cid;

	virtual void addTranslation(Vec3 translation) { position += translation; }
	virtual void addRotation(Vec3 rotation) {  } // Rotation not supported for now
	virtual void addScale(Vec3 scale) { radius = scale.x * radius; }

	virtual void renderAxes(const RendererGL* renderer);
};

struct Plane : public PhysicsPrimitive {
	//for rendering
	Vec3 tangent;
	float renderingScale = 1.0;

	Vec3 normal;
	Vec3 position;
	float D;
	Mat4 rotationMatrix;
	Plane(Vec3 normal, float d) 
		: normal(normal), D(d), PhysicsPrimitive(cid) 
	{
		// TODO: Calculate position from D and N
	}
	Plane(Vec3 position, Vec3 normal)
		: normal(normal), position(position), PhysicsPrimitive(cid) {
		D = -dot(position, normal);
	}
	static void initCases();
	static const unsigned int cid;

	virtual void addTranslation(Vec3 translation) { position += translation; }
	virtual void addRotation(Vec3 rotation) { assert(false); }
	virtual void addScale(Vec3 scale) { assert(false); }

	virtual void renderAxes(const RendererGL* renderer);
};

struct OBB : public PhysicsPrimitive {
	Vec3 position;
	Vec3 u[3];
	Vec3 initial_u[3];
	Vec3 e;
	OBB(const Vec3& position, const Vec3 u[3], const Vec3& e)
		: position(position), e(e), PhysicsPrimitive(cid) 
	{
		this->initial_u[0] = this->u[0] = u[0];
		this->initial_u[1] = this->u[1] = u[1];
		this->initial_u[2] = this->u[2] = u[2];
	}
	static void initCases();
	static const unsigned int cid;

	virtual void addTranslation(Vec3 translation) { position += translation; }
	virtual void addRotation(Vec3 rotation) { 
		Matrix4f rotationMatrix = Mat4().initRotation(rotation);
		u[0] = Matrix4f::mulDirection(rotationMatrix, u[0]);
		u[1] = Matrix4f::mulDirection(rotationMatrix, u[1]);
		u[2] = Matrix4f::mulDirection(rotationMatrix, u[2]);
	}
	virtual void addScale(Vec3 scale) { e = e.mul(scale); }

	virtual void renderAxes(const RendererGL* renderer);
};

struct Ray : public PhysicsPrimitive {
	Vec3 position;
	Vec3 direction;
	bool acceptNegativeHits;

	Ray(const Vec3& position, const Vec3& direction, bool acceptNegativeHits = false)
		: position(position), direction(direction), acceptNegativeHits(acceptNegativeHits), PhysicsPrimitive(cid) { }

	static void initCases();
	static const unsigned int cid;

	virtual void addTranslation(const Vec3& translation) { position += translation; }
	virtual void addRotation(const Vec3& rotation) { }
	virtual void addScale(const Vec3& scale) { assert(false); }

	virtual void renderAxes(const RendererGL* renderer);
};

struct Line : public PhysicsPrimitive {
	Vec3 start;
	Vec3 end;

	Line(const Vec3& start, const Vec3& end)
		: start(start), end(end), PhysicsPrimitive(cid) { }

	static void initCases();
	static const unsigned int cid;

	virtual void addTranslation(const Vec3& translation) { start += translation; end += translation; }
	virtual void addRotation(const Vec3& rotation) { }
	virtual void addScale(const Vec3& scale) { }

	virtual void renderAxes(const RendererGL* renderer);
};

#endif
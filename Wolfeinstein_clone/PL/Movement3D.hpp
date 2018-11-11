
#ifndef MOVEMENT_3D_HPP
#define MOVEMENT_3D_HPP

#include "../math.hpp"

#include <functional>

class Movement3D;

namespace MovFunctions {
	// Default functions
	static float none(float A, float f, float theta) {
		return 0;
	}
	static float fsin(float A, float f, float theta) {
		return A * sin(2.0 * PI * f * theta);
	}
	static float fcos(float A, float f, float theta) {
		return A * cos(2 * PI * f * theta);
	}
	static float ftan(float A, float f, float theta) {
		return A * tan(2 * PI * f * theta);
	}
}

struct MovComponent {
	Vec3 value;
	Vec3 amplitudes;
	Vec3 frequencies;
	std::function<float(float,float,float)> functions[3];

	MovComponent() : value(Vec3()) {
		functions[0] = functions[1] = functions[2] = MovFunctions::none;
	}

	void update(float temp) {
		for (int i = 0; i < 3; ++i)
			value[i] = functions[i](amplitudes[i], frequencies[i], temp);
	}
};

class Movement3D {
public:
	Mesh3D* mesh;

	MovComponent* t;
	MovComponent* r;
	MovComponent* s;

	Movement3D(Mesh3D* mesh) : mesh(mesh), t(nullptr), r(nullptr), s(nullptr) { }

	float temp = 0;
	float frame_time = 1.0f / 60.0f;
	void update() {
		temp += frame_time;
		if (t)
			t->update(temp);
		if (r)
			r->update(temp);
		if (s)
			s->update(temp);

		// Update mesh transform
		mesh->addTranslation(getTranslation());
		mesh->addRotation(getRotation());
		mesh->addScale(getScale());
	}

	Vec3 getTranslation() {
		if (!t)
			return Vec3();
		return t->value;
	}
	Vec3 getRotation() {
		if (!r)
			return Vec3();
		return r->value;
	}
	Vec3 getScale() {
		if (!s)
			return Vec3(1, 1, 1);
		return s->value;
	}
};

#endif

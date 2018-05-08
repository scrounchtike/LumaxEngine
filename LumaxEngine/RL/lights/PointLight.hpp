
#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "../../math.hpp"

struct Attenuation {
	float constant;
	float linear;
	float exponent;
	Attenuation(float constant, float linear, float exponent)
		: constant(constant), linear(linear), exponent(exponent) { }
};

class PointLight {
public:
	PointLight(const Vec3& position, const Vec3& color, const Attenuation& attenuation);
	~PointLight();

	Vec3 position;
	Vec3 color;
	Attenuation attenuation;
private:
};

#endif
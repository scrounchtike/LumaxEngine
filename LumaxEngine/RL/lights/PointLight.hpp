
#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "../../math.hpp"

struct PointLight {
	PointLight() { }
	PointLight(const Vec3& position, const Vec3& color, const Vec3& attenuation) : position(position), color(color), exponent(attenuation.x), linear(attenuation.y), constant(attenuation.z)
	{
		const float minLight = 0.01;
		radius = std::sqrt(1.0 / (exponent * minLight));
	}
	
	Vec3 position;
	float padding1;
	
	Vec3 color;
	float padding2;
	
	float exponent;
	float linear;
	float constant;
	float radius;
};

#endif

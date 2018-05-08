
#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

#include "../../math.hpp"

class DirectionalLight {
public:
	DirectionalLight(const Vec3& direction, const Vec3& color, float intensity = 1.0);
	~DirectionalLight();

	Vec3 direction;
	Vec3 color;
	float intensity;
private:
};

#endif
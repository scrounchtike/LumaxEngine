
#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include "PointLight.hpp"
#include "../../math.hpp"

class SpotLight{
public:
	SpotLight(const Vec3& position, const Vec3& color, const Attenuation& attenuation, const Vec3& direction, float cutoff);
	SpotLight(const PointLight& pointlight, const Vec3& direction, float cutoff);
	~SpotLight();
	
	Vec3 direction;
	float cutoff;
	PointLight pointlight;
};

#endif


#include "SpotLight.hpp"

SpotLight::SpotLight(const Vec3& position, const Vec3& color, const Vec3& attenuation, const Vec3& direction, float cutoff)
	: pointlight(PointLight(position, color, attenuation)), direction(direction), cutoff(cutoff)
{	
}

SpotLight::SpotLight(const PointLight& pointlight, const Vec3& direction, float cutoff)
	: pointlight(pointlight), direction(direction), cutoff(cutoff)
{
}

SpotLight::~SpotLight(){

}

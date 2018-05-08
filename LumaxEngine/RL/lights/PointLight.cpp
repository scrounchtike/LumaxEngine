
#include "PointLight.hpp"

PointLight::PointLight(const Vec3& position, const Vec3& color, const Attenuation& attenuation)
	: position(position), color(color), attenuation(attenuation)
{

}

PointLight::~PointLight() {

}

#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(const Vec3& direction, const Vec3& color, float intensity) 
	: direction(direction), color(color), intensity(intensity) 
{

}

DirectionalLight::~DirectionalLight() {

}
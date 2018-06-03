
#include "PointLight.hpp"

#include <algorithm>

PointLight::PointLight(const Vec3& position, const Vec3& color, const Attenuation& attenuation)
	: position(position), color(color), attenuation(attenuation)
{
	float Imax = std::fmaxf(std::fmaxf(color.x, color.y), color.z);
	radius = (-attenuation.linear + std::sqrtf(attenuation.linear*attenuation.linear - 4.0f * attenuation.exponent * (attenuation.constant - (256.0f / 5.0f) * Imax))) / (2.0f * attenuation.exponent);
}

PointLight::~PointLight() {

}

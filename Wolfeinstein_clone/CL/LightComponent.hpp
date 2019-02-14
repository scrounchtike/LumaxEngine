
#ifndef LIGHT_COMPONENT_HPP
#define LIGHT_COMPONENT_HPP

#include "../RL/lights.hpp"

struct LightComponent {
	PointLight vertexPointLights[4];
	SpotLight  vertexSpotLights[4];
	float scoreVPL[4];
	float scoreVSL[4];

	PointLight pixelPointLights[2];
	SpotLight  pixelSpotLights[2];
	float scorePPL[2];
	float scorePSL[2];
};

#endif


#ifndef LIGHT_COMPONENT_HPP
#define LIGHT_COMPONENT_HPP

#include "../RL/lights.hpp"

struct LightComponent {
	PointLight* vertexPointLights[4];
	float scoreVPL[4];
	SpotLight*  vertexSpotLights[4];
	float scoreVSL[4];

	PointLight* pixelPointLights[4];
	float scorePPL[4];
	SpotLight*  pixelSpotLights[4];
	float scorePSL[4];
};

struct SimpleLightComponent {
	SimpleLightComponent()
		: dlight(nullptr)
	{
		memset(plights, 0, sizeof(PointLight*)*4);
		memset(slights, 0, sizeof(SpotLight*)*4);
	}
	DirectionalLight* dlight;
	PointLight* plights[4];
	SpotLight* slights[4];
};

#endif

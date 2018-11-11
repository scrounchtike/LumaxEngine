
#ifndef WATER_TILE_HPP
#define WATER_TILE_HPP

#include "../math.hpp"

#include "Model3D.hpp"

class WaterTile {
public:
	WaterTile();
	
	void update();
	void render() const;

	static void initModel();

private:
	static Model3D* waterModel;
};

#endif

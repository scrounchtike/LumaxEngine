
#include "WaterTile.hpp"

Model3D* WaterTile::waterModel;

WaterTile::WaterTile(){
	
}

void WaterTile::update(){
	
}

void WaterTile::render() const {
	waterModel->render();
}

void WaterTile::initModel(){
	const float vertices[] = {
		0,0,0, 1,0,0, 1,0,1, 0,0,1
	};
	const int indices[] = {
		0,1,2,2,3,0
	};
	const float texCoords[] = {
		0,0,0,1,1,1,1,0
	};
	const float normals[] = {
		0,1,0, 0,1,0, 0,1,0, 0,1,0
	};
	waterModel = new Model3D(vertices, 4, indices, 6, texCoords, normals);
}

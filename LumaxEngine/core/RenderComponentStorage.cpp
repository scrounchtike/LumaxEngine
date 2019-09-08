
#include "RenderComponentStorage.hpp"

#include "../RL/Model3D.hpp"
#include "../RL/Model2D.hpp"

unsigned int GeometryGroup3D::getGeometryID()
{
	return mesh->getGeometryID();
}

unsigned int GeometryGroup2D::getGeometryID()
{
	return mesh->getGeometryID();
}

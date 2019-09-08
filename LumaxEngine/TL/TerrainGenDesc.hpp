
#ifndef TERRAIN_GEN_DESC_HPP
#define TERRAIN_GEN_DESC_HPP

// 2D support only for now

// Abstract class !
class TerrainGenDesc
{
public:
	TerrainGenDesc(){}

	virtual float getHeight(float x, float y) = 0;
	virtual Vec3 getNormal(float x, float y, float length_triangle) = 0;
	virtual float getSlope(float x, float y) = 0;

	virtual std::vector<float>* getHeights(int vertex_count, float xstart, float ystart) = 0;
	virtual std::vector<Vec3>* getNormals(int vertex_count, float xstart, float ystart, float length_triangle) = 0;
	virtual std::vector<float>* getSlopes(int vertex_count, float xstart, float ystart) = 0;
	
private:
};

#endif

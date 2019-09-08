
#include "FBmTerrain.hpp"

#include "Map.hpp"

FBmTerrain::FBmTerrain(uint32 octaves,
											 float persistence,
											 float lacunarity,
											 float max_height)
	: octaves(octaves), persistence(persistence), lacunarity(lacunarity), frequency(1.0 / std::pow(2, octaves - 1)), max_height(max_height)
{
}

float FBmTerrain::getHeight(float x, float z)
{
	float amp = 1;
	float freq = frequency;
	
	float total = 0;
	for(int k = 0; k < octaves; ++k){
		float add = amp * getValueNoise(x*freq, z*freq);
		//float add = amp * getSmoothNoise((int)x, (int)z);
		//if(add > 0)
		//	add = -add + 1;
		//else
		//	add = add + 1;
		total += add;
		
		freq *= lacunarity;
		amp *= persistence;
	}
	
	return total * max_height;
}

Vec3 FBmTerrain::getNormal(float x, float z, float length_triangle)
{
	float heightL = getHeight(x-1,z);
	float heightR = getHeight(x+1,z);
	float heightD = getHeight(x,z-1);
	float heightU = getHeight(x,z+1);
	Vec3 normal(heightL - heightR, length_triangle / 2.0, heightD - heightU);
	normal.normalize();
	return normal;
}

float FBmTerrain::getSlope(float x, float z)
{
	return 0.0f;
}

std::vector<float>* FBmTerrain::getHeights(int vertex_count, float xstart, float zstart)
{
	return nullptr;
}

std::vector<Vec3>* FBmTerrain::getNormals(int vertex_count, float xstart, float zstart, float length_triangle)
{
	return nullptr;
}

std::vector<float>* FBmTerrain::getSlopes(int vertex_count, float xstart, float zstart)
{
	return nullptr;
}


//
// Helper functions for generating terrain
//

// Perlin Noise
float FBmTerrain::getPerlinNoise(float x, float z)
{
	int intX = fastFloor(x);
	int intZ = fastFloor(z);
	float fracX = x - intX;
	float fracZ = z - intZ;
	
	Vec2 v1(x - intX, z - intZ);
	Vec2 v2(x - (intX + 1), z - intZ);
	Vec2 v3(x - intX, z - (intZ + 1));
	Vec2 v4(x - (intX + 1), z - (intZ + 1));
  
	float d1 = v1.dot(getGradientNoise(intX, intZ));
	float d2 = v2.dot(getGradientNoise(intX + 1, intZ));
	float d3 = v3.dot(getGradientNoise(intX, intZ + 1));
	float d4 = v4.dot(getGradientNoise(intX + 1, intZ + 1));
  
	float Sx = (3 * fracX * fracX) - (2 * fracX * fracX * fracX);
	float a = d1 + Sx * (d2 - d1);
	float b = d3 + Sx * (d4 - d3);
	
	float Sy = (3 * fracZ * fracZ) - (2 * fracZ * fracZ * fracZ);
	float gradient = a + Sy * (b - a);
  
	return gradient;
}

Vec2 FBmTerrain::getGradientNoise(float x, float z)
{
	static int seed = 666;
	static int seedGradient = seed + 1000;
	int height = hash32shift(seed + hash32shift(x + hash32shift(z)));
	height = height >> 31;
	int gradient = hash32shift(seedGradient + hash32shift(x + hash32shift(z)));
	gradient = gradient >> 31;
  
	return Vec2(height, gradient);
}

// Value Noise
float FBmTerrain::getSmoothNoise(int x, int z)
{
	float corners = (getHashNoise(x - 1, z - 1) + getHashNoise(x + 1, z - 1) + getHashNoise(x - 1, z + 1) + getHashNoise(x + 1, z + 1)) / 16.0f;
	float sides = (getHashNoise(x + 1, z) + getHashNoise(x - 1, z) + getHashNoise(x, z + 1) + getHashNoise(x, z - 1)) / 8.0f;
	float center = getHashNoise(x, z) / 4.0f;
	return corners + sides + center;
}

float FBmTerrain::getValueNoise(float x, float z)
{
	int intX = fastFloor(x);
	int intZ = fastFloor(z);
	float fracX = x - intX;
	float fracZ = z - intZ;
	
	float v1 = getSmoothNoise(intX, intZ);
	float v2 = getSmoothNoise(intX + 1, intZ);
	float v3 = getSmoothNoise(intX, intZ + 1);
	float v4 = getSmoothNoise(intX + 1, intZ + 1);
	
	float i1 = interpolate(v1, v2, fracX);
	float i2 = interpolate(v3, v4, fracX);
		
	return interpolate(i1, i2, fracZ);
}

// Calculation functions
// TODO : Calculate normals, tangents and slopes

// Hashing functions
int FBmTerrain::hash32shift(int key)
{
	key = ~key + (key << 15); // key = (key << 15) - key - 1;
	key = key ^ ((unsigned)key >> 12);
	key = key + (key << 2);
	key = key ^ ((unsigned)key >> 4);
	key = key * 2057; // key = (key + (key << 3)) + (key << 11);
	key = key ^ ((unsigned)key >> 16);
	return key;
}

float FBmTerrain::getHashNoise(int x, int z)
{
	int n = x + z * 57;
	n = (n<<13) ^ n;
	float rand = (float) ( 1.0 - ( (n * (n * n * 15731 + 3263443) + 1376312589) & 0x7fffffff) / 1073741824.0);
	return (rand / 2.0f) + 0.5f;
}

/*

void FBmTerrain::calculateNormals(unsigned vertexCount, FUNC_GET_FLOAT heightLambda, FUNC_SET_VEC3 setNormal, float length_triangle){
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			float heightL = heightLambda(i - 1, j);
			float heightR = heightLambda(i + 1, j);
			float heightD = heightLambda(i, j - 1);
			float heightU = heightLambda(i, j + 1);
			if(i == 0)
				heightL = heightR;
			if(i == vertexCount - 1)
				heightR = heightL;
			if(j == 0)
				heightD = heightU;
			if(j == vertexCount - 1)
				heightU = heightD;
			Vec3 normal(heightL - heightR, length_triangle/2.0, heightD - heightU);
			normal.normalize();
			setNormal(i, j, normal);
		}
	}
}

void FBmTerrain::calculateSlopes(unsigned vertexCount, FUNC_GET_FLOAT heightLambda, FUNC_SET_FLOAT setSlope, float length_triangle){
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			float heightL = heightLambda(i - 1, j);
			float heightR = heightLambda(i + 1, j);
			float heightD = heightLambda(i, j - 1);
			float heightU = heightLambda(i, j + 1);
			if(i == 0)
				heightL = heightR;
			if(i == vertexCount - 1)
				heightR = heightL;
			if(j == 0)
				heightD = heightU;
			if(j == vertexCount - 1)
				heightU = heightD;
			
			float height = heightLambda(i, j);
			float heightLF = heightLambda(i - 2, j);
			float heightRF = heightLambda(i + 2, j);
			float heightDF = heightLambda(i, j - 2);
			float heightUF = heightLambda(i, j + 2);
			//		if(x == 1)
		//			heightLF = heightL;
			if(i == vertexCount - 2)
				heightRF = height;
			//		if(z == 1)
			//			heightDF = heightD;
			if(j == vertexCount - 2)
				heightUF = height;
			
			float slopeHoriz = (std::abs(height - heightL) + std::abs(height - heightR) + 
													std::abs(height - heightLF) + std::abs(height - heightRF)) / (length_triangle * 4.0f);
			float slopeVert  = (std::abs(height - heightU) + std::abs(height - heightD) + 
													std::abs(height - heightUF) + std::abs(height - heightDF)) / (length_triangle * 4.0f);
			float slope = slopeHoriz > slopeVert ? slopeHoriz : slopeVert;
			setSlope(i, j, slope);
		}
	}
}

void FBmTerrain::calculateTangents(unsigned vertexCount, FUNC_GET_FLOAT getHeight, FUNC_SET_VEC3 setTangent, float length_triangle){
	// TODO : tangents
}

void FBmTerrain::generateHeights(unsigned vertexCount, float (FBmTerrain::*getHeight)(float, float)){
	int scale = this->vertexCount / vertexCount;
	
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			heights[i*scale][j*scale] = (this->*getHeight)(i*scale+z, j*scale+x);
		}
	}
}

void FBmTerrain::interpolateHeights(unsigned vertexCount, float (FBmTerrain::*getHeight)(float, float)){
	// VertexCount should now be twice the vertexCount for generation
	int scale = this->vertexCount / vertexCount;

	
	auto checkHeight = [getHeight, scale, this](int i, int j)->float{
		if(i < 0 || i >= this->vertexCount || j < 0 || j >= this->vertexCount)
			return (this->*getHeight)(i+x, j+z);
		if(i%2 == 0 && j%2 == 0)
			return heights[i][j];
		return (this->*getHeight)(i+z, j+x);
	};
	
	
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			//if(i == 0 || j == 0 || i == (vertexCount-1) || j == (vertexCount-1)){
			//	heights[i*newMul][j*newMul] = (this->*getHeight)(i*newMul+x, j*newMul+z);
			//	continue;
			//}
			
			int lasti = i - (i%2);
			int lastj = j - (j%2);
			
			float newHeightX = 0;
			if(i%2==0)
				newHeightX = heights[i*scale][j*scale];
				//newHeightX = (this->*getHeight)(i, j);
			else
				newHeightX = (checkHeight(lasti*scale, lastj*scale) + checkHeight(lasti*scale+1, lastj*scale)) / 2.0;
				//newHeightX = ((this->*getHeight)(lasti, lastj) + (this->*getHeight)((lasti+1), lastj)) / 2.0;
			float newHeightY = 0;
			if(j%2==0)
				newHeightY = heights[i*scale][j*scale];
				//newHeightY = (this->*getHeight)(i, j);
			else
				newHeightY = (checkHeight(lasti*scale, lastj*scale) + checkHeight(lasti*scale, lastj*scale+1)) / 2.0;
				//newHeightY = ((this->*getHeight)(lasti, lastj) + (this->*getHeight)(lasti, (lastj+1))) / 2.0;
			float newHeight = 0;
			if(i%2 && j%2)
				newHeight = (newHeightX + newHeightY) / 2.0;
			else if(i%2)
				newHeight = newHeightX;
			else
				newHeight = newHeightY;

			heights[i*scale][j*scale] = newHeight;
		}
	}
}

auto FBmTerrain::getHeightLambda(int scale){
	return [this, scale](int i, int j)->float{
		if(i < 0 || i*scale >= vertexCount || j < 0 || j*scale >= vertexCount)
			return getFBmHeight(i*scale+z, j*scale+x);
		return heights[i*scale][j*scale];
	};
}

auto FBmTerrain::getNormalLambda(int scale){
	return [this, scale](int i, int j)->Vec3{
		return normals[i*scale][j*scale];
	};
}

auto FBmTerrain::getSlopeLambda(int scale){
	return [this, scale](int i, int j)->float{
		return slopes[i*scale][j*scale];
	};
}

auto FBmTerrain::getSetNormalLambda(int scale){
	return [this, scale](int i, int j, Vec3 normal){
		normals[i*scale][j*scale] = normal;
	};
}

auto FBmTerrain::getSetSlopeLambda(int scale){
	return [this, scale](int i, int j, float slope){
		slopes[i*scale][j*scale] = slope;
	};
}

void FBmTerrain::generate_async_LOD0(TileHolder* tile){
	int vertexCount = this->vertexCount;
	
	// Heights
	generateHeights(vertexCount, &FBmTerrain::getFBmHeight);
	auto heightLambda = getHeightLambda(1.0);

	// Normals and slopes
	float length_triangle = (float) size / (float) (this->vertexCount - 1);
	calculateNormals(vertexCount, heightLambda, getSetNormalLambda(1), length_triangle);
	calculateSlopes(vertexCount, heightLambda, getSetSlopeLambda(1), length_triangle);
	
	tile->LOD = LOD0;
	tile->terrain = Terrain::generateTerrainAsync(vertexCount, size, heightLambda, getNormalLambda(1.0), getSlopeLambda(1.0));
}

void FBmTerrain::generate_async_LOD1(TileHolder* tile){
	int scale = 2.0;
	int vertexCountGen = this->vertexCount / scale;
	int vertexCountInt = vertexCountGen * 2;

	int scaleInt = this->vertexCount / vertexCountInt;
	
	generateHeights(vertexCountGen, &FBmTerrain::getFBmHeight);
	interpolateHeights(vertexCountInt, &FBmTerrain::getFBmHeight);
	auto heightLambda = getHeightLambda(scaleInt);
	
	float length_triangle = (float) size / (float) (vertexCountInt - 1);
	calculateNormals(vertexCountInt, heightLambda, getSetNormalLambda(scaleInt), length_triangle);
	calculateSlopes(vertexCountInt, heightLambda, getSetSlopeLambda(scaleInt), length_triangle);
	
	tile->LOD = LOD1;
	tile->terrain = Terrain::generateTerrainAsync(vertexCount, size, heightLambda, getNormalLambda(scaleInt), getSlopeLambda(scaleInt));
}

void FBmTerrain::generate_async_LOD2(TileHolder* tile){
	int scale = 4;
	int vertexCountGen = this->vertexCount / scale;
	int vertexCountInt = vertexCountGen * 2;

	int scaleInt = this->vertexCount / vertexCountInt;

	generateHeights(vertexCountGen, &FBmTerrain::getFBmHeight);
	interpolateHeights(vertexCountInt, &FBmTerrain::getFBmHeight);
	auto heightLambda = getHeightLambda(scaleInt);

	float length_triangle = (float) size / (float) (vertexCountInt - 1);
	calculateNormals(vertexCountInt, heightLambda, getSetNormalLambda(scaleInt), length_triangle);
	calculateSlopes(vertexCountInt, heightLambda, getSetSlopeLambda(scaleInt), length_triangle);
	
	tile->LOD = LOD2;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountInt, size, heightLambda, getNormalLambda(scaleInt), getSlopeLambda(scaleInt));
}

void FBmTerrain::generate_async_LOD3(TileHolder* tile){
	int scale = 4;
	int vertexCountGen = this->vertexCount / scale;

	generateHeights(vertexCountGen, &FBmTerrain::getFBmHeight);

	auto heightLambda = getHeightLambda(scale);

	float length_triangle = (float) size / (float) (vertexCountGen- 1);
	calculateNormals(vertexCountGen, heightLambda, getSetNormalLambda(scale), length_triangle);
	calculateSlopes(vertexCountGen, heightLambda, getSetSlopeLambda(scale), length_triangle);
	
	tile->LOD = LOD3;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountGen, size, heightLambda, getNormalLambda(scale), getSlopeLambda(scale));
}

void FBmTerrain::upgradeHeightsResolution(unsigned vertexCount, float (FBmTerrain::*getHeight)(float, float)){
	int scale = this->vertexCount / vertexCount;
	
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			//if(i%2==0 && j%2==0)
			//continue;
			if(i%2 || j%2)
				heights[i*scale][j*scale] = (this->*getHeight)(i*scale+z, j*scale+x);
			//heights[i*scale][j*scale] = (this->*getHeight)(i*scale+z, j*scale+x);
		}
	}
}

void FBmTerrain::upgradeNormalsResolution(unsigned int vertexCount, FUNC_GET_FLOAT heightLambda, FUNC_SET_VEC3 setNormal, float length_triangle){
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			if(i%2 == 0 && j%2 == 0)
				continue;
			float heightL = heightLambda(i - 1, j);
			float heightR = heightLambda(i + 1, j);
			float heightD = heightLambda(i, j - 1);
			float heightU = heightLambda(i, j + 1);
			if(i == 0)
				heightL = heightR;
			if(i == vertexCount - 1)
				heightR = heightL;
			if(j == 0)
				heightD = heightU;
			if(j == vertexCount - 1)
				heightU = heightD;
			Vec3 normal(heightL - heightR, length_triangle/2.0, heightD - heightU);
			normal.normalize();
			setNormal(i, j, normal);
		}
	}
}

void FBmTerrain::upgradeSlopesResolution(unsigned int vertexCount, FUNC_GET_FLOAT heightLambda, FUNC_SET_FLOAT setSlope, float length_triangle){
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			if(i%2 == 0 && j%2==0)
				continue;
			float heightL = heightLambda(i - 1, j);
			float heightR = heightLambda(i + 1, j);
			float heightD = heightLambda(i, j - 1);
			float heightU = heightLambda(i, j + 1);
			if(i == 0)
				heightL = heightR;
			if(i == vertexCount - 1)
				heightR = heightL;
			if(j == 0)
				heightD = heightU;
			if(j == vertexCount - 1)
				heightU = heightD;
			
			float height = heightLambda(i, j);
			float heightLF = heightLambda(i - 2, j);
			float heightRF = heightLambda(i + 2, j);
			float heightDF = heightLambda(i, j - 2);
			float heightUF = heightLambda(i, j + 2);
			//		if(x == 1)
		//			heightLF = heightL;
			if(i == vertexCount - 2)
				heightRF = height;
			//		if(z == 1)
			//			heightDF = heightD;
			if(j == vertexCount - 2)
				heightUF = height;
			
			float slopeHoriz = (std::abs(height - heightL) + std::abs(height - heightR) + 
													std::abs(height - heightLF) + std::abs(height - heightRF)) / (length_triangle * 4.0f);
			float slopeVert  = (std::abs(height - heightU) + std::abs(height - heightD) + 
													std::abs(height - heightUF) + std::abs(height - heightDF)) / (length_triangle * 4.0f);
			float slope = slopeHoriz > slopeVert ? slopeHoriz : slopeVert;
			setSlope(i, j, slope);
		}
	}
}

void FBmTerrain::upgrade_to_LOD0(TileHolder* tile){
	int vertexCountGen = this->vertexCount;
	
	upgradeHeightsResolution(vertexCountGen, &FBmTerrain::getFBmHeight);
	
	auto heightLambda = getHeightLambda(1);
	
	//float length_triangle = (float) size / (float) (vertexCountGen - 1);
	// Should not be needed since LOD1 calculates 128x128 normals and slopes.
	// Albeit with the interpolated heights and therefore with some uncertainty
	//upgradeNormalsResolution(vertexCountGen, heightLambda, getSetNormalLambda(1.0), length_triangle);
	//upgradeSlopesResolution(vertexCountGen, heightLambda, getSetSlopeLambda(1.0), length_triangle);

	tile->LOD = LOD0;
	delete tile->terrain;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountGen, size, heightLambda, getNormalLambda(1.0), getSlopeLambda(1.0));
}

void FBmTerrain::upgrade_to_LOD1(TileHolder* tile){
	int scale = 2;
	int vertexCountGen = this->vertexCount / scale;
	int vertexCountInt = vertexCountGen * 2;
	int scaleInt = this->vertexCount / vertexCountInt;

	upgradeHeightsResolution(vertexCountGen, &FBmTerrain::getFBmHeight);
	interpolateHeights(vertexCountInt, &FBmTerrain::getFBmHeight);
	auto heightLambda = getHeightLambda(scaleInt);

	float length_triangle = (float) size / (float) (vertexCountGen - 1);
	//calculateNormals(vertexCountInt, heightLambda, getSetNormalLambda(scaleInt), length_triangle);
	//calculateSlopes(vertexCountInt, heightLambda, getSetSlopeLambda(scaleInt), length_triangle);
	upgradeNormalsResolution(vertexCountInt, heightLambda, getSetNormalLambda(scaleInt), length_triangle);
	upgradeSlopesResolution(vertexCountInt, heightLambda, getSetSlopeLambda(scaleInt), length_triangle);
	
	tile->LOD = LOD1;
	delete tile->terrain;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountInt, size, heightLambda, getNormalLambda(scaleInt), getSlopeLambda(scaleInt));
}

void FBmTerrain::upgrade_to_LOD2(TileHolder* tile){
	int scale = 4;
	int scaleInt = 2;
	int vertexCountInt = this->vertexCount / scale * 2;

	interpolateHeights(vertexCountInt, &FBmTerrain::getFBmHeight);
	auto heightLambda = getHeightLambda(scaleInt);

	float length_triangle = (float) size / (float) (vertexCountInt - 1);
	upgradeNormalsResolution(vertexCountInt, heightLambda, getSetNormalLambda(scaleInt), length_triangle);
	upgradeSlopesResolution(vertexCountInt, heightLambda, getSetSlopeLambda(scaleInt), length_triangle);

	tile->LOD = LOD2;
	delete tile->terrain;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountInt, size, heightLambda, getNormalLambda(scaleInt), getSlopeLambda(scaleInt));
}

void FBmTerrain::downgrade_to_LOD1(TileHolder* tile){
	// Nothing to do! 128x128 -> 128x128 interpolated
	//std::cout << "LOD0 -> LOD1" << std::endl;
	tile->LOD = LOD1;
}

void FBmTerrain::downgrade_to_LOD2(TileHolder* tile){
	int scale = 2;
	int vertexCountInt = this->vertexCount / scale;
	
	auto heightLambda = getHeightLambda(scale);
	
	tile->LOD = LOD2;
	delete tile->terrain;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountInt, size, heightLambda, getNormalLambda(scale), getSlopeLambda(scale));
}

void FBmTerrain::downgrade_to_LOD3(TileHolder* tile){
	int scale = 4;
	int vertexCountInt = this->vertexCount / scale;

	auto heightLambda = getHeightLambda(scale);
	
	tile->LOD = LOD3;
	delete tile->terrain;
	tile->terrain = Terrain::generateTerrainAsync(vertexCountInt, size, heightLambda, getNormalLambda(scale), getSlopeLambda(scale));
}
*/

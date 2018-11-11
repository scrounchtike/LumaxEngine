
#include "HeightMapTerrain.hpp"

#include <functional>

#include "../math.hpp"
#include <stdlib.h>
#include "Perlin.hpp"

#include "../core/Time.hpp"

#include "Map.hpp"

#include <noise/noise.h>
#include "noiseutils.h"

std::vector<float> HeightMapTerrain::staticVertices;
std::vector<int> HeightMapTerrain::staticIndices;
std::vector<float> HeightMapTerrain::staticTexCoords;
std::vector<float> HeightMapTerrain::staticNormals;
std::vector<float> HeightMapTerrain::staticSlopes;
bool HeightMapTerrain::building = false;

void createTerrainASYNC(float mul, unsigned int vertexCount,
												std::function<float(float, float)> getHeight,
												std::function<Vec3(float, float)> getNormal, std::function<float(float, float)> getSlope,
												std::function<void(int, int, float)> storeHeight,
												std::function<void(int, int, Vec3)> storeNormal,
												std::function<void(int, int, float)> storeSlope){
	//const int vertexCount = 128;
	const float size = 100;
	const float maxHeight = 20;

	const float height_offset = 20;
	
	int count = vertexCount * vertexCount;
	std::vector<float> vertices;
	vertices.resize(count * 3);
	std::vector<int> indices;
	indices.resize(6 * (vertexCount - 1) * (vertexCount - 1));
	std::vector<float> texCoords;
	texCoords.resize(count * 2);
	std::vector<float> normals;
	normals.resize(count * 3);
	std::vector<float> slopes;
	slopes.resize(count);

	//Time::startTimer();
	
	int vertexPointer = 0;
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			//Time::startTimer();
			float height = getHeight(i, j);
			//Time::endTimer();
			//height /= 256.0;
			height *= maxHeight;
			height += 100;

			// Vertices
			vertices[vertexPointer*3+0] = (float)j/((float)vertexCount - 1) * size;
			vertices[vertexPointer*3+1] = height - height_offset;
			vertices[vertexPointer*3+2] = (float)i/((float)vertexCount - 1) * size;

			// Texcoords
			texCoords[vertexPointer*2+0] = (float)j/((float)vertexCount - 1) * 16.0;
			texCoords[vertexPointer*2+1] = (float)i/((float)vertexCount - 1) * 16.0;

			// Normals
			Vec3 normal = getNormal(i, j);
			normals[vertexPointer*3+0] = normal.x;
			normals[vertexPointer*3+1] = normal.y;
			normals[vertexPointer*3+2] = normal.z;
			
			// Slope
			float slope = getSlope(i*mul, j*mul);
			//std::cout << slope << std::endl;
			slopes[vertexPointer] = slope;

			storeHeight(i, j, getHeight(i, j));
			storeNormal(i, j, normal);
			storeSlope(i, j, slope);
			
			++vertexPointer;
		}
	}

	//Time::endTimer();
	//std::cout << "time vertices = " << Time::getElapsedTime() << " ms" << std::endl;
	std::cout << "time getHeight = " << Time::getElapsedTime() << " ms" << std::endl;
	
	// Indices
	int pointer = 0;
  for(int gz = 0; gz < vertexCount - 1; gz++){
		for(int gx = 0; gx < vertexCount - 1; gx++){
			int topLeft = (gz * vertexCount) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vertexCount) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
		}
	}
	
	// Create terrain model
	//Time::startTimer();
	//Model3D* model = new Model3D(vertices, indices, texCoords, normals, slopes, true);
	//Time::endTimer();
	//std::cout << "time model = " << Time::getElapsedTime() << " ms" << std::endl;
	//return model;

	// Prepare data for main thread to build terrain model
	HeightMapTerrain::staticVertices = vertices;
	HeightMapTerrain::staticIndices = indices;
	HeightMapTerrain::staticTexCoords = texCoords;
	HeightMapTerrain::staticNormals = normals;
	HeightMapTerrain::staticSlopes = slopes;
	HeightMapTerrain::building = true;
}

Model3D* createTerrain(std::function<float(float, float)> getHeight, std::function<Vec3(float, float)> getNormal, std::function<float(float, float)> getSlope){
	const int vertexCount = 513;
	const float size = 200;
	const float maxHeight = 30;

	const float height_offset = 20;
	
	int count = vertexCount * vertexCount;
	std::vector<float> vertices;
	vertices.resize(count * 3);
	std::vector<int> indices;
	indices.resize(6 * (vertexCount - 1) * (vertexCount - 1));
	std::vector<float> texCoords;
	texCoords.resize(count * 2);
	std::vector<float> normals;
	normals.resize(count * 3);
	std::vector<float> slopes;
	slopes.resize(count);

	//Time::startTimer();
	
	int vertexPointer = 0;
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			//Time::startTimer();
			float height = getHeight(i, j);
			//Time::endTimer();
			//height /= 256.0;
			height *= maxHeight;
			height += 60;

			// Vertices
			vertices[vertexPointer*3+0] = (float)j/((float)vertexCount - 1) * size;
			vertices[vertexPointer*3+1] = height - height_offset;
			vertices[vertexPointer*3+2] = (float)i/((float)vertexCount - 1) * size;

			// Texcoords
			texCoords[vertexPointer*2+0] = (float)j/((float)vertexCount - 1) * 16.0;
			texCoords[vertexPointer*2+1] = (float)i/((float)vertexCount - 1) * 16.0;

			// Normals
			Vec3 normal = getNormal(i, j);
			normals[vertexPointer*3+0] = normal.x;
			normals[vertexPointer*3+1] = normal.y;
			normals[vertexPointer*3+2] = normal.z;
			
			// Slope
			float slope = getSlope(i, j);
			//std::cout << slope << std::endl;
			slopes[vertexPointer] = slope;
			
			++vertexPointer;
		}
	}

	//Time::endTimer();
	//std::cout << "time vertices = " << Time::getElapsedTime() << " ms" << std::endl;
	std::cout << "time getHeight = " << Time::getElapsedTime() << " ms" << std::endl;
	
	// Indices
	int pointer = 0;
  for(int gz = 0; gz < vertexCount - 1; gz++){
		for(int gx = 0; gx < vertexCount - 1; gx++){
			int topLeft = (gz * vertexCount) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vertexCount) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
		}
	}
	
	// Create terrain model
	Time::startTimer();
	Model3D* model = new Model3D(vertices, indices, texCoords, normals, slopes, true);
	Time::endTimer();
	std::cout << "time model = " << Time::getElapsedTime() << " ms" << std::endl;
	return model;
}

Model3D* HeightMapTerrain::createHeightMapTerrain(const std::string& filename){
	float maxHeight = 100;
	float vertexCount = 1024;
	// Bitmap loading
	Bitmap* bitmap = LevelWolf::loadBitmap(filename);
	assert(bitmap->width == bitmap->height);
	
	auto lambda = [maxHeight, bitmap](float i, float j)->float{
		uint32 color = (bitmap->pixels[(int)j * bitmap->width + (int)i] & 0xFFFFFF);
		float height = (color & 0xFF);
		height /= (256.0 / maxHeight);
		return height;
	};
	
	auto normalLambda = [bitmap](float i, float j)->Vec3{
		return Vec3(0,0,0);
	};
	
	auto slopeLambda = [bitmap, vertexCount, maxHeight](float x, float z)->float{
		float length_triangle = (float) 600.0 / (float) (vertexCount - 1);
		
		auto getHeight = [bitmap, vertexCount, maxHeight](float x, float z)->float{
			if(x < 0 || x >= vertexCount || z < 0 || z >= vertexCount)
				return 0;
		  uint32 color = (bitmap->pixels[(int)z * bitmap->width + (int)x] & 0xFFFFFF);
			float height = (color & 0xFF);
			height /= (256.0 / maxHeight);
			return height;
		};
	  float height = getHeight(x, z);
		float heightL = getHeight(x - 1, z);
		float heightR = getHeight(x + 1, z);
		float heightD = getHeight(x, z - 1);
		float heightU = getHeight(x, z + 1);
		float heightLF = getHeight(x - 2, z);
		float heightRF = getHeight(x + 2, z);
		float heightDF = getHeight(x, z - 2);
		float heightUF = getHeight(x, z + 2);
		if(x == 0)
			heightL = heightR;
		if(x == vertexCount - 1)
			heightR = heightL;
		if(z == 0)
			heightD = heightU;
		if(z == vertexCount - 1)
			heightU = heightD;
		//		if(x == 1)
		//			heightLF = heightL;
		if(x == vertexCount - 2)
			heightRF = height;
		//		if(z == 1)
		//			heightDF = heightD;
		if(z == vertexCount - 2)
			heightUF = height;
		
		float slopeHoriz = (std::abs(height - heightL) + std::abs(height - heightR) + 
												std::abs(height - heightLF) + std::abs(height - heightRF)) / (length_triangle * 4.0f);
		float slopeVert  = (std::abs(height - heightU) + std::abs(height - heightD) + 
												std::abs(height - heightUF) + std::abs(height - heightDF)) / (length_triangle * 4.0f);
		float slope = slopeHoriz > slopeVert ? slopeHoriz : slopeVert;
		return slope;
	};
	
	return createTerrain(lambda, normalLambda, slopeLambda);
}

int hash32shift(int key) {
	key = ~key + (key << 15); // key = (key << 15) - key - 1;
	key = key ^ ((unsigned)key >> 12);
	key = key + (key << 2);
	key = key ^ ((unsigned)key >> 4);
	key = key * 2057; // key = (key + (key << 3)) + (key << 11);
	key = key ^ ((unsigned)key >> 16);
	return key;
}

Vec2 getNoise(const int x, const int z) {
	static int seed = 666;
	static int seedGradient = seed + 1000;
	int height = hash32shift(seed + hash32shift(x + hash32shift(z)));
	height = height >> 31;
	int gradient = hash32shift(seedGradient + hash32shift(x + hash32shift(z)));
	gradient = gradient >> 31;
  
	return Vec2(height, gradient);
}

//int fastFloor(float x){
//	static int BIG_ENOUGH_INT = 16 * 1024;
//	return (int) (x + BIG_ENOUGH_INT) - BIG_ENOUGH_INT;
//}

float getHeight(float x, float z){
	int intX = fastFloor(x);
	int intZ = fastFloor(z);
	float fracX = x - intX;
	float fracZ = z - intZ;
	
	Vec2 v1(x - intX, z - intZ);
	Vec2 v2(x - (intX + 1), z - intZ);
	Vec2 v3(x - intX, z - (intZ + 1));
	Vec2 v4(x - (intX + 1), z - (intZ + 1));
  
	float d1 = dot(v1, getNoise(intX, intZ));
	float d2 = dot(v2, getNoise(intX + 1, intZ));
	float d3 = dot(v3, getNoise(intX, intZ + 1));
	float d4 = dot(v4, getNoise(intX + 1, intZ + 1));
  
	float Sx = (3 * fracX * fracX) - (2 * fracX * fracX * fracX);
	float a = d1 + Sx * (d2 - d1);
	float b = d3 + Sx * (d4 - d3);
	
	float Sy = (3 * fracZ * fracZ) - (2 * fracZ * fracZ * fracZ);
	float gradient = a + Sy * (b - a);
  
	return gradient;
}

//FastNoise noise;

float getFastNoise(float x, float z){
	
}

float getHeight(float x, float z, std::function<Vec2(unsigned x, unsigned z)> getNoise){
	int intX = fastFloor(x);
	int intZ = fastFloor(z);
	float fracX = x - intX;
	float fracZ = z - intZ;
	
	Vec2 v1(x - intX, z - intZ);
	Vec2 v2(x - (intX + 1), z - intZ);
	Vec2 v3(x - intX, z - (intZ + 1));
	Vec2 v4(x - (intX + 1), z - (intZ + 1));
  
	float d1 = dot(v1, getNoise(intX, intZ));
	float d2 = dot(v2, getNoise(intX + 1, intZ));
	float d3 = dot(v3, getNoise(intX, intZ + 1));
	float d4 = dot(v4, getNoise(intX + 1, intZ + 1));
  
	float Sx = (3 * fracX * fracX) - (2 * fracX * fracX * fracX);
	float a = d1 + Sx * (d2 - d1);
	float b = d3 + Sx * (d4 - d3);
	
	float Sy = (3 * fracZ * fracZ) - (2 * fracZ * fracZ * fracZ);
	float gradient = a + Sy * (b - a);
  
	return gradient;
}

float getNoise2(const int x, const int y){
	int n = x + y * 57;
	n = (n<<13) ^ n;
	float rand = (float) ( 1.0 - ( (n * (n * n * 15731 + 3263443) + 1376312589) & 0x7fffffff) / 1073741824.0); 
	return (rand / 2.0f) + 0.5f;
}

float getSmoothNoise(int x, int z){
	float corners = (getNoise2(x - 1, z - 1) + getNoise2(x + 1, z - 1) + getNoise2(x - 1, z + 1) + getNoise2(x + 1, z + 1)) / 16.0f;
	float sides = (getNoise2(x + 1, z) + getNoise2(x - 1, z) + getNoise2(x, z + 1) + getNoise2(x, z - 1)) / 8.0f;
	float center = getNoise2(x, z) / 4.0f;
	return corners + sides + center;
}

float getHeight2(float x, float z){
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

Vec2** noise_heightmap = nullptr;
//float** perlin_heightmap = nullptr;
float** fBm_heightmap = nullptr;

float fastAbs(float x){
		return (x <= 0.0f) ? 0.0f - x : x;
}

void diamondStep(int x, int y, int size, float disp, float** heights){
	float c1 = heights[x][y];
	float c2 = heights[x + size][y];
	float c3 = heights[x + size][y + size];
	float c4 = heights[x][y + size];
	float random = ((float)rand() / (float)RAND_MAX * 2.0f * disp) - disp;
	
	heights[size / 2 + x][size / 2 + y] = (c1 + c2 + c3 + c4) / 4.0f + random;
}

void squareStep(int x, int y, int size, float disp, float** heights){
	float c1 = heights[x][y];
	float c2 = heights[x + size][y];
	float c3 = heights[x + size][y + size];
	float c4 = heights[x][y + size];
	float center = heights[size / 2 + x][size / 2 + y];
	
	const float vertexCount = 257;
	
	//down
	if((y - (size / 2)) > 0){
		float c5 = heights[size / 2 + x][y - (size / 2)];
		heights[size / 2 + x][y] = (c1 + c2 + center + c5) / 4.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	}else
		heights[size / 2 + x][y] = (c1 + c2 + center) / 3.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	
	//left
	if((x - (size / 2)) > 0){
		float c6 = heights[x - (size / 2)][size / 2 + y];
		heights[x][size / 2 + y] = (c1 + c4 + center + c6) / 4.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	}else
		heights[x][size / 2 + y] = (c1 + c4 + center) / 3.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	
	//up
	if((y + size + (size / 2)) < vertexCount){
		float c7 = heights[size / 2 + x][y + size + (size / 2)];
		heights[size / 2 + x][y + size] = (c4 + c3 + center + c7) / 4.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	}else
		heights[size / 2 + x][y + size] = (c4 + c3 + center) / 3.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	
	//right
	if((x + size + (size / 2)) < vertexCount){
		float c8 = heights[x + size + (size / 2)][size / 2 + y];
		heights[x + size][size / 2 + y] = (c2 + c3 + center + c8) / 4.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
	}else
		heights[x + size][size / 2 + y] = (c2 + c3 + center) / 3.0f + ((float)rand()/(float)RAND_MAX * 2.0f * disp - disp);
}

Model3D* HeightMapTerrain::createMPDTerrain(){
	const float vertexCount = 513;
	float disp = 50;
	float roughness = 1; 
	
	float** heights = new float*[vertexCount];
	for(int i = 0; i < vertexCount; ++i)
		heights[i] = new float[vertexCount];

	unsigned iterations = (int)(std::log(vertexCount-1) / std::log(2.0));

	//heights[(int)vertexCount / 2][(int)vertexCount / 2] = 70;
	for(int i = 0; i < vertexCount; i++){
		for(int j = 0; j < vertexCount; j++){
			int i_ = i - vertexCount / 2;
			int j_ = j - vertexCount / 2; 
			heights[i][j] = 0;
		}
	}
	
	// Generate height map
	float size = vertexCount;
	for(int i = 0; i < iterations; ++i){
		float sub = (float)std::pow(2, i);
		for(int x = 0; x < sub; ++x){
			for(int y = 0; y < sub; ++y){
				diamondStep(x * (size - 1), y * (size - 1), size - 1, disp, heights);
				squareStep(x * (size - 1), y * (size - 1), size - 1, disp, heights);
			}
		}
		size = std::ceil((float)size / 2.0f);
		disp *= std::pow(2, -roughness);
	}
	
	//Implementing MAX_HEIGHT
	//1. Find highest summit
	float highest = heights[0][0];
	for(int i = 0; i < vertexCount; i++){
		for(int j = 0; j < vertexCount; j++)
			highest = heights[i][j] > highest ? heights[i][j] : highest;
	}
	
	float scalingFactor = 4.0f / highest;
	
	for(int i = 0; i < vertexCount; i++){
		for(int j = 0; j < vertexCount; j++){
			heights[i][j] *= scalingFactor;
		}
	}

	float length_triangle = (float) 200 / (float) (vertexCount - 1);
	
	auto heightLambda = [vertexCount, heights](float i, float j)->float{
		if(i < 0 || i >= vertexCount || j < 0 || j >= vertexCount)
			return 0;
		return heights[(int)i][(int)j];
	};
	auto normalLambda = [vertexCount, heightLambda, length_triangle, heights](float x, float z)->Vec3{
	  float heightL = heightLambda(x - 1, z);
		float heightR = heightLambda(x + 1, z);
		float heightD = heightLambda(x, z - 1);
		float heightU = heightLambda(x, z + 1);
		if(x == 0)
			heightL = heightR;
		if(x == vertexCount - 1)
			heightR = heightL;
		if(z == 0)
			heightD = heightU;
		if(z == vertexCount - 1)
			heightU = heightD;
		Vec3 normal(heightL - heightR, length_triangle/2.0, heightD - heightU);
		normal.normalize();
		return normal;
		//return Vec3(0,0,0);
	};
	auto slopeLambda = [heights](float i, float j)->float{
		return 0;
	};

	return createTerrain(heightLambda, normalLambda, slopeLambda);
}

float getHeightFault(int x, int z, int vertexCount, float** heights){
	if(x < 0 || x >= vertexCount || z < 0 || z >= vertexCount)
		return 0;
	return heights[x][z];
	
}

Model3D* HeightMapTerrain::createFaultTerrain(){
	const int vertexCount = 64;

	float** heights = new float*[vertexCount];
	for(int i = 0; i < vertexCount; ++i){
		heights[i] = new float[vertexCount];
		for(int j = 0; j < vertexCount; ++j)
			heights[i][j] = 0;
	}
	
	float displacement0 = 3.0f;
	float displacementN = 1.0f;
	float displacement = displacement0;

	const float iterations = 100;
	for(int i = 0; i < iterations; ++i){
		if(i < iterations/2.0f)
			displacement = displacement0 + ((float)i / (float)iterations) * (displacementN - displacement0);
		else
			displacement = displacementN;
		
		float random = (float)rand() / (float)RAND_MAX;
		float a = std::sin(2.0f * M_PI * random);
		float b = std::cos(2.0f * M_PI * random);
		float d = std::sqrt(vertexCount*vertexCount + vertexCount*vertexCount);
		float c = ((float)rand() / (float)RAND_MAX) * d - (d / 2.0f);

		for(int j = 0; j < vertexCount; ++j){
			for(int k = 0; k < vertexCount; ++k){
				if((a * j + b * k - c) > 0)
					heights[k][j] += displacement;
				else
					heights[k][j] -= displacement;
			}
		}
	}

	// Smoothing
	for(int i = 0; i < vertexCount; i++){
		for(int j = 0; j < vertexCount; j++){
			float heightL = getHeightFault(i - 1, j, vertexCount, heights);
			float heightR = getHeightFault(i + 1, j, vertexCount, heights);
			float heightD = getHeightFault(i, j - 1, vertexCount, heights);
			float heightU = getHeightFault(i, j + 1, vertexCount, heights);
			float heightLD = getHeightFault(i - 1, j - 1, vertexCount, heights);
			float heightDR = getHeightFault(i + 1, j - 1, vertexCount, heights);
			float heightRU = getHeightFault(i + 1, j + 1, vertexCount, heights);
			float heightUL = getHeightFault(i - 1, j + 1, vertexCount, heights);
			
			if(i == 0){
				heightL = heightR;
				if(j == 0)
					heightLD = heightRU;
			}
			if(i == vertexCount - 1){
				heightR = heightL;
				if(j == vertexCount - 1)
					heightRU = heightLD;
			}
			if(j == 0){
				heightD = heightU;
				if(i == vertexCount - 1)
					heightDR = heightUL;
			}
			if(j == vertexCount - 1){
				heightU = heightD;
				if(i == 0)
					heightUL = heightDR;
			}
			
			heights[i][j] = (heights[i][j]) / 4.0f + 
				(heightR + heightL + heightU + heightD) / 8.0f + 
				(heightLD + heightDR + heightRU + heightUL) / 16.0f;
		}
	}
	
	//Implementing MAX_HEIGHT
	//1. Find highest summit
	float highest = heights[0][0];
	for(int i = 0; i < vertexCount; i++){
		for(int j = 0; j < vertexCount; j++){
			highest = heights[i][j] > highest ? heights[i][j] : highest;
		}
	}
	
	float scalingFactor = 10.0f / highest;
	
	for(int i = 0; i < vertexCount; i++){
		for(int j = 0; j < vertexCount; j++){
			heights[i][j] *= scalingFactor;
		}
	}

	float length_triangle = (float) 200 / (float) (vertexCount - 1);
	
	auto heightLambda = [vertexCount, heights](float i, float j)->float{
		if(i < 0 || i >= vertexCount || j < 0 || j >= vertexCount)
			return 0;
		return heights[(int)i][(int)j];
	};
	auto normalLambda = [length_triangle, heightLambda, heights](float x, float z)->Vec3{
	  float heightL = heightLambda(x - 1, z);
		float heightR = heightLambda(x + 1, z);
		float heightD = heightLambda(x, z - 1);
		float heightU = heightLambda(x, z + 1);
		if(x == 0)
			heightL = heightR;
		if(x == vertexCount - 1)
			heightR = heightL;
		if(z == 0)
			heightD = heightU;
		if(z == vertexCount - 1)
			heightU = heightD;
		Vec3 normal(heightL - heightR, length_triangle/2.0, heightD - heightU);
		normal.normalize();
		return normal;
	};
	auto slopeLambda = [heights](float i, float j)->float{
		return 0;
	};
	
	return createTerrain(heightLambda, normalLambda, slopeLambda);
	//return nullptr;
}

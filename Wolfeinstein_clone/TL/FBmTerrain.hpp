
#ifndef FBM_TERRAIN_HPP
#define FBM_TERRAIN_HPP

#include "../math.hpp"

#include <noise/noise.h>
#include "noiseutils.h"

#include "../RL/Model3D.hpp"

struct TileHolder;

struct MEMFUNC_GET_FLOAT;

class FBmTerrain {
public:
	typedef std::function<void(int i, int j, float h)> FUNC_SET_FLOAT;
	typedef std::function<void(int i, int j, Vec3 v)> FUNC_SET_VEC3;

	typedef std::function<float(int i, int j)> FUNC_GET_FLOAT;
	typedef std::function<Vec3(int i, int j)> FUNC_GET_VEC3;
	
	int vertexCount = 128;
	float size = 100;

	int octaves = 6;
	float persistence = 0.5;
	float lacunarity = 0.5;
	float frequency = 0;

	float x, z;
	
	noise::module::Perlin perlin;

	float** heights;
	Vec3** normals;
	float** slopes;
	
	FBmTerrain(unsigned vertexCount = 128, unsigned size = 100, unsigned x = 0, unsigned z = 0, unsigned octaves = 4, float persistence = 0.5, float lacunarity = 0.5)
		: vertexCount(vertexCount), size(size), x(x), z(z), octaves(octaves), persistence(persistence), lacunarity(lacunarity), frequency(1.0 / std::pow(2, octaves - 1))
	{
		// Libnoise
		perlin.SetOctaveCount(octaves);
		perlin.SetFrequency(frequency);
		perlin.SetPersistence(persistence);
		perlin.SetLacunarity(lacunarity);

		// maps
		heights = new float*[vertexCount];
		normals = new Vec3*[vertexCount];
		slopes = new float*[vertexCount];
		for(int i = 0; i < vertexCount; ++i){
			heights[i] = new float[vertexCount];
			normals[i] = new Vec3[vertexCount];
			slopes[i] = new float[vertexCount];
		}
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		/*
		switch(LOD){
		case 0:
			generate_async_LOD0(nullptr);
			break;
		case 1:
			generate_async_LOD1(nullptr);
			break;
		case 2:
			generate_async_LOD2(nullptr);
			break;
		case 3:
			generate_async_LOD3(nullptr);
			break;
		}
		*/
	}

	// Move to a more generic noise class
	// --- //
	static int hash32shift(int key);
	static Vec2 getGradientNoise(const int x, const int z);
	static float getPerlinNoise(float x, float z);
	
	static float getHashNoise(int x, int z);
	static float getSmoothNoise(int x, int z);
	static float getValueNoise(float x, float z);
	// --- //

	float getFBmHeight(float x, float z);
	float getFBmHeightCustom(float x, float z);
	
	void calculateNormals(unsigned vertexCount, FUNC_GET_FLOAT getHeight, FUNC_SET_VEC3 setNormal, float length_triangle);
	void calculateSlopes(unsigned vertexCount, FUNC_GET_FLOAT getHeight, FUNC_SET_FLOAT setSlope, float length_triangle);
	void calculateTangents(unsigned vertexCount, FUNC_GET_FLOAT getHeight, FUNC_SET_VEC3 setTangent, float length_triangle);

	void generateHeights(unsigned vertexCount, float (FBmTerrain::*getHeight)(float, float));
	void interpolateHeights(unsigned vertexCount, float (FBmTerrain::*getHeight)(float, float));
	
	auto getHeightLambda(int scale);
	auto getNormalLambda(int scale);
	auto getSlopeLambda(int scale);

	auto getSetNormalLambda(int scale);
	auto getSetSlopeLambda(int scale);

	void upgradeHeightsResolution(unsigned vertexCount, float (FBmTerrain::*getHeight)(float, float));
	void upgradeNormalsResolution(unsigned vertexCount, FUNC_GET_FLOAT getHeight, FUNC_SET_VEC3 setNormal, float length_triangle);
	void upgradeSlopesResolution(unsigned vertexCount, FUNC_GET_FLOAT getHeight, FUNC_SET_FLOAT setSlope, float length_triangle);
	
	void generate_async_LOD0(TileHolder* tile);
	void generate_async_LOD1(TileHolder* tile);
	void generate_async_LOD2(TileHolder* tile);
	void generate_async_LOD3(TileHolder* tile);
	
	void upgrade_to_LOD0(TileHolder* tile);
	void upgrade_to_LOD1(TileHolder* tile);
	void upgrade_to_LOD2(TileHolder* tile);
	
	void downgrade_to_LOD1(TileHolder* tile);
	void downgrade_to_LOD2(TileHolder* tile);
	void downgrade_to_LOD3(TileHolder* tile);

	void update();
	void render() const;
	
private:
	Model3D* model;
};

//typedef float (FBmTerrain::*)(float, float) MEMFUNC_GET_FLOAT;

#endif

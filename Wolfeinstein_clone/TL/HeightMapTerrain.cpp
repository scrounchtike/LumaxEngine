
#include "HeightMapTerrain.hpp"

#include "../math.hpp"

Model3D* HeightMapTerrain::createHeightMapTerrain(const std::string& filename){
	const int vertexCount = 1024;
	const float size = 600;
	const float maxHeight = 100;
	
	// Load terrain model from heightmap
	
	int count = vertexCount * vertexCount;
	//float* vertices = new float[count * 3];
	//int* indices = new int[6 * (vertexCount - 1) * (vertexCount - 1)];
	//float* texCoords = new float[count * 2];
	std::vector<float> vertices;
	vertices.resize(count * 3);
	std::vector<int> indices;
	indices.resize(6 * (vertexCount - 1) * (vertexCount - 1));
	std::vector<float> texCoords;
	texCoords.resize(count * 2);
	
	//float* normals = new float[count * 3];
	//float* slopes = new float[count];

	// Bitmap loading
	Bitmap* bitmap = LevelWolf::loadBitmap(filename);
	assert(bitmap->width == bitmap->height);
	assert(bitmap->width == vertexCount);
	
	int vertexPointer = 0;
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			assert(bitmap);
			//float height = getHeight() - height_offset;
			uint32 color = (bitmap->pixels[j * bitmap->width + i] & 0xFFFFFF);
			float height = (color & 0xFF);
			//if(height != 0)
			//	std::cout << "YAY" << std::endl;
			//height = rand() % 20;
			//height = -1000;
			//float height = 0;
			height /= 256.0;
			height *= maxHeight;
			
			vertices[vertexPointer*3+0] = (float)j/((float)vertexCount - 1) * size;
			vertices[vertexPointer*3+1] = height;
			vertices[vertexPointer*3+2] = (float)i/((float)vertexCount - 1) * size;
			
			texCoords[vertexPointer*2+0] = (float)j/((float)vertexCount - 1) * 16.0;
			texCoords[vertexPointer*2+1] = (float)i/((float)vertexCount - 1) * 16.0;
			
			//Vec3 normal(0,0,0);
			//normals[vertexPointer*3+0] = normal.x;
			//normals[vertexPointer*3+1] = normal.y;
			//normals[vertexPointer*3+2] = normal.z;
			
			//float slope = 0;
			//slopes[vertexPointer] = slope;
			
			++vertexPointer;
		}
	}
	
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
	//std::cout << "test" << std::endl;
	
	// Create terrain model
	Model3D* model = new Model3D(vertices, indices, texCoords);
	return model;
	//return lmx::getStaticResourceManager()->getModel3D("cube.obj");
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
	int height = hash32shift(seed + hash32shift(x + hash32shift(z)));
	height = height >> 31;
	int gradient = hash32shift(seedGradient + hash32shift(x + hash32shift(z)));
	gradient = gradient >> 31;
  
	return Vec2(height, gradient);
}

float getHeight(float x, float z){
	int intX = std::floor(x);
	int intZ = std::floor(z);
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

Model3D* HeightMapTerrain::createFBmTerrain(){
	const int vertexCount = 1024;
	const float size = 600;
	const float maxHeight = 100;

	float d = 1.0;
	float roughness = 0.5f;
	int numOctaves = 6;
	
	// Load terrain model from heightmap
	
	int count = vertexCount * vertexCount;
	std::vector<float> vertices;
	vertices.resize(count * 3);
	std::vector<int> indices;
	indices.resize(6 * (vertexCount - 1) * (vertexCount - 1));
	std::vector<float> texCoords;
	texCoords.resize(count * 2);
	
	int vertexPointer = 0;
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			//float height = getHeight() - height_offset;
			float height = 0;
			{
				float total = 0;
				float freq = 1.0f / d;
				float amp = 1;

				for(int k = 0; k < numOctaves; ++k){
					float add = 

					total += add * amp * maxHeight;
					amp *= roughness;
				}
				height = total;
			}
			height /= 256.0;
			height *= maxHeight;
			
			vertices[vertexPointer*3+0] = (float)j/((float)vertexCount - 1) * size;
			vertices[vertexPointer*3+1] = height;
			vertices[vertexPointer*3+2] = (float)i/((float)vertexCount - 1) * size;
			
			texCoords[vertexPointer*2+0] = (float)j/((float)vertexCount - 1) * 16.0;
			texCoords[vertexPointer*2+1] = (float)i/((float)vertexCount - 1) * 16.0;
			
			++vertexPointer;
		}
	}
	
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
	Model3D* model = new Model3D(vertices, indices, texCoords);
	return model;
}

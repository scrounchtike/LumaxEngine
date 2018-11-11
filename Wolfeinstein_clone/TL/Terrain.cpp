
#include "Terrain.hpp"

#include "../RL/Model3D.hpp"

#include "Map.hpp"
#include "TerrainQueue.hpp"

#include <vector>

std::vector<float> Terrain::staticVertices;
std::vector<int> Terrain::staticIndices;
std::vector<float> Terrain::staticTexCoords;
std::vector<float> Terrain::staticNormals;
std::vector<float> Terrain::staticSlopes;
bool Terrain::building = false;

Model3D* Terrain::generateTerrainAsync(unsigned vertexCount, float size,
																			 std::function<float(int i, int j)> getHeight,
																			 std::function<Vec3(int i, int j)> getNormal,
																			 std::function<float(int i, int j)> getSlope)
{
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
	
	int vertexPointer = 0;
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			float height = getHeight(i, j);

			// Vertices
			vertices[vertexPointer*3+0] = (float)j/((float)vertexCount - 1) * size;
			vertices[vertexPointer*3+1] = height;
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
	
	Terrain::staticVertices = vertices;
	Terrain::staticIndices = indices;
	Terrain::staticTexCoords = texCoords;
	Terrain::staticNormals = normals;
	Terrain::staticSlopes = slopes;
	Terrain::building = true;
	
}

Model3D* Terrain::generateTerrain(unsigned vertexCount, float size,
																	std::function<float(int i, int j)> getHeight,
																	std::function<Vec3(int i, int j)> getNormal,
																	std::function<float(int i, int j)> getSlope)
{
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
	
	int vertexPointer = 0;
	for(int i = 0; i < vertexCount; ++i){
		for(int j = 0; j < vertexCount; ++j){
			float height = getHeight(i, j);

			// Vertices
			vertices[vertexPointer*3+0] = (float)j/((float)vertexCount - 1) * size;
			vertices[vertexPointer*3+1] = height;
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
	Model3D* model = new Model3D(vertices, indices, texCoords, normals, slopes, true);
	return model;
	//return nullptr;
}

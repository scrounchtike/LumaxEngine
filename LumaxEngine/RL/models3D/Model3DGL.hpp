
#ifndef MODEL_3D_GL_HPP
#define MODEL_3D_GL_HPP

#include "../../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include <vector>
#include "../../RAL/UsingOpenGL.hpp"

class Model3DGL {
public:
	unsigned int numVertices, numIndices;
	bool isIndexed, isTextured, hasNormals, hasTangents, isAnimated;
	
	Model3DGL(const float* vertices, int numVertices);
	Model3DGL(const float* vertices, int numVertices, const float* texCoords);
	Model3DGL(const float* vertices, int numVertices, const float* texCoords, const float* normals);
	Model3DGL(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents);

	// Animation support
	void addAnimationWeights(const float* boneIDs, const float* weights, unsigned numWeights);

	~Model3DGL();

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	void cleanUp();
	GLuint vboID;
	GLuint vbotID;
	GLuint vbonID;
	GLuint vbotgID;
	GLuint iboID;
	GLuint vaoID;
	// Animation data
	GLuint vbobID;
	GLuint vbowID;

	static void createVertexArray(GLuint* vaoID);
	static void createVBO(GLuint* vobID, const float* vertices, int numVertices, int index, int size);
	static void createVBOT(GLuint* vbotID, const float* texCoords, int numTexcoords, int index, int size);
	static void createVBON(GLuint* vbonID, const float* normals, int numNormals, int index, int size);
	static void createVBOTG(GLuint* vbotgID, const float* tangents, int numTangents, int index, int size);
	static void createIBO(GLuint* iboID, const int* indices, int numIndices);
	// Animation support
	static void createVBOB(GLuint* vbobID, const float* bones, int numWeights, int index, int size);
	static void createVBOW(GLuint* vbowID, const float* weights, int numWeights, int index, int size);

	static void unbindBuffers();
};

#endif
#endif

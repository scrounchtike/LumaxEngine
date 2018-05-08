
#ifndef MODEL_2D_GL_HPP
#define MODEL_2D_GL_HPP

#include "Model2D.hpp"
#include "../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include "../RAL/UsingOpenGL.hpp"

#include <vector>

class Model2DGL : public Model2D {
public:
	Model2DGL(float* vertices, int numVertices);
	Model2DGL(float* vertices, int numVertices, float* texCoords);
	Model2DGL(float* vertices, int numVertices, int* indices, int numIndices);
	Model2DGL(float* vertices, int numVertices, int* indices, int numIndices, float* texCoord);

	Model2DGL(std::vector<float>& vertices);
	Model2DGL(std::vector<float>& vertices, std::vector<float>& texCoords);
	Model2DGL(std::vector<float>& vertices, std::vector<int>& indices);
	Model2DGL(std::vector<float>& vertices, std::vector<int>& indices, std::vector<float>& texCoords);

	~Model2DGL();

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	void initialize(float* vertices, int numVertices);
	void initialize(float* vertices, int numVertices, float* texCoords);
	void initialize(float* vertices, int numVertices, int* indices, int numIndices);
	void initialize(float* vertices, int numVertices, int* indices, int numIndices, float* texCoords);

	void cleanUp();

	// State variables
	bool isTextured, isIndexed;
	unsigned int numVertices, numIndices;
	GLuint vboID;
	GLuint vbotID;
	GLuint iboID;
	GLuint vaoID;

	static void createVertexArray(GLuint*);
	static void createVBO(GLuint*, float* vertices, int numVertices, int index, int size);
	static void createVBOT(GLuint*, float* texCoords, int numTexcoords, int index, int size);
	static void createIBO(GLuint*, int* indices, int numIndices);

	static void unbindBuffers();
};

#endif
#endif
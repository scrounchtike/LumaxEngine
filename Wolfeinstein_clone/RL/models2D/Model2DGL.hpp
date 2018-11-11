
#ifndef MODEL_2D_GL_HPP
#define MODEL_2D_GL_HPP

#include "../../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include "../../RAL/UsingOpenGL.hpp"

#include <vector>
#include "../../math/types.hpp"

class Model2DGL {
	friend class Model2D;
public:
	const uint16 ID;
	
	bool isTextured, isIndexed;
	unsigned int numVertices, numIndices;

	Model2DGL(const float* vertices, int numVertices);
	Model2DGL(const float* vertices, int numVertices, const float* texCoords);
	Model2DGL(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model2DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoord);

	~Model2DGL();

	void update();
	void render() const;
	void renderInstanced(unsigned int count) const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	// State variables
	GLuint vboID;
	GLuint vbotID;
	GLuint iboID;
	GLuint vaoID;

	static void createVertexArray(GLuint*);
	static void createVBO(GLuint*, const float* vertices, int numVertices, int index, int size);
	static void createVBOT(GLuint*, const float* texCoords, int numTexcoords, int index, int size);
	static void createIBO(GLuint*, const int* indices, int numIndices);

	static void unbindBuffers();
};

#endif
#endif

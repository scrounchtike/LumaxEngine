
#ifndef MODEL_3D_GL_HPP
#define MODEL_3D_GL_HPP

#include "../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include <vector>
#include "../RAL/UsingOpenGL.hpp"

class Model3DGL {
public:
	Model3DGL(const float* vertices, int numVertices);
	Model3DGL(const float* vertices, int numVertices, const float* texCoords);
	Model3DGL(const float* vertices, int numVertices, const float* texCoords, const float* normals);
	Model3DGL(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals);
	Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents);

	Model3DGL(const std::vector<float>& vertices);
	Model3DGL(const std::vector<float>& vertices, const std::vector<float>& texCoords);
	Model3DGL(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals);
	Model3DGL(const std::vector<float>& vertices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents);
	Model3DGL(const std::vector<float>& vertices, const std::vector<int>& indices);
	Model3DGL(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords);
	Model3DGL(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals);
	Model3DGL(const std::vector<float>& vertices, const std::vector<int>& indices, const std::vector<float>& texCoords, const std::vector<float>& normals, const std::vector<float>& tangents);

	~Model3DGL();

	void update();
	void render() const;

	void bindForRender() const;
	void renderBuffersOnly() const;
	void unbindForRender() const;
private:
	void initialize(const float* vertices, int numVertices);
	void initialize(const float* vertices, int numVertices, const float* texCoords);
	void initialize(const float* vertices, int numVertices, const float* texCoords, const float* normals);
	void initialize(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals);
	void initialize(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents);

	void cleanUp();

	bool isTextured, isIndexed, hasNormals, hasTangents;
	unsigned int numVertices, numIndices;
	GLuint vboID;
	GLuint vbotID;
	GLuint vbonID;
	GLuint vbotgID;
	GLuint iboID;
	GLuint vaoID;

	static void createVertexArray(GLuint* vaoID);
	static void createVBO(GLuint* vobID, const float* vertices, int numVertices, int index, int size);
	static void createVBOT(GLuint* vbotID, const float* texCoords, int numTexcoords, int index, int size);
	static void createVBON(GLuint* vbonID, const float* normals, int numNormals, int index, int size);
	static void createVBOTG(GLuint* vbotgID, const float* tangents, int numTangents, int index, int size);
	static void createIBO(GLuint* iboId, const int* indices, int numIndices);

	static void unbindBuffers();
};

#endif
#endif
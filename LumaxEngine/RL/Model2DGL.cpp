
#include "Model2DGL.hpp"

#ifdef _USE_OPENGL

Model2DGL::Model2DGL(float* vertices, int numVertices) : isTextured(false), isIndexed(false) {
	initialize(vertices, numVertices);
}

Model2DGL::Model2DGL(float* vertices, int numVertices, float* texCoords) : isTextured(true), isIndexed(false) {
	initialize(vertices, numVertices, texCoords);
}

Model2DGL::Model2DGL(float* vertices, int numVertices, int* indices, int numIndices) : isTextured(false), isIndexed(true) {
	initialize(vertices, numVertices, indices, numIndices);
}

Model2DGL::Model2DGL(float* vertices, int numVertices, int* indices, int numIndices, float* texCoord) : isTextured(true), isIndexed(true) {
	initialize(vertices, numVertices, indices, numIndices, texCoord);
}

Model2DGL::Model2DGL(std::vector<float>& vertices) : isTextured(false), isIndexed(false) {
	initialize(&vertices[0], vertices.size() / 2);
}

Model2DGL::Model2DGL(std::vector<float>& vertices, std::vector<float>& texCoords) : isTextured(true), isIndexed(false) {
	initialize(&vertices[0], vertices.size() / 2, &texCoords[0]);
}

Model2DGL::Model2DGL(std::vector<float>& vertices, std::vector<int>& indices) : isTextured(false), isIndexed(true) {
	initialize(&vertices[0], vertices.size() / 2, &indices[0], indices.size());
}

Model2DGL::Model2DGL(std::vector<float>& vertices, std::vector<int>& indices, std::vector<float>& texCoords) : isTextured(true), isIndexed(false) {
	initialize(&vertices[0], vertices.size() / 2, &indices[0], indices.size(), &texCoords[0]);
}

Model2DGL::~Model2DGL() {
	cleanUp();
}

void Model2DGL::update() {

}

void Model2DGL::render() const {
	// Binding for render
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	if (isTextured)
		glEnableVertexAttribArray(1);

	// Render call
	if (isIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
		glDrawArrays(GL_TRIANGLES, 0, numVertices);

	// Unbinding
	glDisableVertexAttribArray(0);
	if (isTextured)
		glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Model2DGL::bindForRender() const {
	// Binding for render
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	if (isTextured)
		glEnableVertexAttribArray(1);
}

void Model2DGL::renderBuffersOnly() const {
	// Render call
	if (isIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Model2DGL::unbindForRender() const {
	// Unbinding
	glDisableVertexAttribArray(0);
	if (isTextured)
		glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Model2DGL::initialize(float* vertices, int numVertices) {
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 2, 0, 2);
	unbindBuffers();
	this->numVertices = numVertices;
}

void Model2DGL::initialize(float* vertices, int numVertices, float* texCoords) {
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 2, 0, 2);
	createVBOT(&vbotID, texCoords, numVertices * 2, 1, 2);
	unbindBuffers();
	this->numVertices = numVertices;
}

void Model2DGL::initialize(float* vertices, int numVertices, int* indices, int numIndices) {
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 2, 0, 2);
	createIBO(&iboID, indices, numIndices);
	unbindBuffers();
	this->numIndices = numIndices;
}

void Model2DGL::initialize(float* vertices, int numVertices, int* indices, int numIndices, float* texCoords) {
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 2, 0, 2);
	createVBOT(&vbotID, texCoords, numVertices * 2, 1, 2);
	createIBO(&iboID, indices, numIndices);
	unbindBuffers();
	this->numIndices = numIndices;
}

void Model2DGL::cleanUp() {
	// TODO: Delete all GL buffers
}

void Model2DGL::createVertexArray(GLuint* vaoID) {
	glGenVertexArrays(1, vaoID);
	glBindVertexArray(*vaoID);
}

void Model2DGL::createVBO(GLuint* vboID, float *vertices, int numVertices, int index, int size) {
	glGenBuffers(1, vboID);
	glBindBuffer(GL_ARRAY_BUFFER, *vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numVertices, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model2DGL::createVBOT(GLuint* vbotID, float *texCoords, int numTexCoords, int index, int size) {
	glGenBuffers(1, vbotID);
	glBindBuffer(GL_ARRAY_BUFFER, *vbotID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numTexCoords, texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model2DGL::createIBO(GLuint* iboID, int *indices, int numIndices) {
	glGenBuffers(1, iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*numIndices, indices, GL_STATIC_DRAW);
}

void Model2DGL::unbindBuffers() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#endif
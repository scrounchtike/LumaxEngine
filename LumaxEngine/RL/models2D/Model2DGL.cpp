
#include "Model2DGL.hpp"
#include "../Model2D.hpp"

#ifdef _USE_OPENGL

Model2DGL::Model2DGL(const float* vertices, int numVertices)
	: isTextured(false), isIndexed(false), numVertices(numVertices), numIndices(0)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices, 0, 2);
	unbindBuffers();
}

Model2DGL::Model2DGL(const float* vertices, int numVertices, const float* texCoords)
	: isTextured(true), isIndexed(false), numVertices(numVertices), numIndices(0)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices, 0, 2);
	createVBOT(&vbotID, texCoords, numVertices, 1, 2);
	unbindBuffers();
}

Model2DGL::Model2DGL(const float* vertices, int numVertices, const int* indices, int numIndices)
	: isTextured(false), isIndexed(true), numVertices(numVertices), numIndices(numIndices)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices, 0, 2);
	createIBO(&iboID, indices, numIndices);
	unbindBuffers();
}

Model2DGL::Model2DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords)
	: isTextured(true), isIndexed(true), numVertices(numVertices), numIndices(numIndices)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices, 0, 2);
	createVBOT(&vbotID, texCoords, numVertices, 1, 2);
	createIBO(&iboID, indices, numIndices);
	unbindBuffers();
}

Model2DGL::~Model2DGL() {
	// TODO: Delete all GL buffers
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

void Model2DGL::renderInstanced(unsigned int count) const {
	// Binding for render
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	if (isTextured)
		glEnableVertexAttribArray(1);
	
	// Render call
	if (isIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0, count);
	}
	else
		glDrawArraysInstanced(GL_TRIANGLES, 0, numVertices, count);

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

void Model2DGL::createVertexArray(GLuint* vaoID) {
	glGenVertexArrays(1, vaoID);
	glBindVertexArray(*vaoID);
}

void Model2DGL::createVBO(GLuint* vboID, const float *vertices, int numVertices, int index, int size) {
	glGenBuffers(1, vboID);
	glBindBuffer(GL_ARRAY_BUFFER, *vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numVertices, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model2DGL::createVBOT(GLuint* vbotID, const float *texCoords, int numTexCoords, int index, int size) {
	glGenBuffers(1, vbotID);
	glBindBuffer(GL_ARRAY_BUFFER, *vbotID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numTexCoords, texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model2DGL::createIBO(GLuint* iboID, const int *indices, int numIndices) {
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

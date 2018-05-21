
#include "Model3DGL.hpp"

#ifdef _USE_OPENGL

Model3DGL::Model3DGL(const float* vertices, int numVertices) 
	: numVertices(numVertices), numIndices(0), isIndexed(false), isTextured(false), hasNormals(false), hasTangents(false)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const float* texCoords)
	: numVertices(numVertices), numIndices(0), isIndexed(false), isTextured(true), hasNormals(false), hasTangents(false)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createVBOT(&vbotID, texCoords, numVertices * 2, 1, 2);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const float* texCoords, const float* normals) 
	: numVertices(numVertices), numIndices(0), isIndexed(false), isTextured(true), hasNormals(true), hasTangents(false)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createVBOT(&vbotID, texCoords, numVertices * 2, 1, 2);
	createVBON(&vbonID, normals, numVertices * 3, 2, 3);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const float* texCoords, const float* normals, const float* tangents) 
	: numVertices(numVertices), numIndices(0), isIndexed(false), isTextured(true), hasNormals(true), hasTangents(true) 
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createVBOT(&vbotID, texCoords, numVertices * 2, 1, 2);
	createVBON(&vbonID, normals, numVertices * 3, 2, 3);
	createVBOTG(&vbotgID, tangents, numVertices * 3, 3, 3);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices) 
	: numVertices(numVertices), numIndices(numIndices), isIndexed(true), isTextured(false), hasNormals(false), hasTangents(false)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createIBO(&iboID, indices, numIndices);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords) 
	: numVertices(numVertices), numIndices(numIndices), isIndexed(true), isTextured(true), hasNormals(false), hasTangents(false) 
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createIBO(&iboID, indices, numIndices);
	createVBOT(&vbotgID, texCoords, numVertices * 2, 1, 2);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals) 
	: numVertices(numVertices), numIndices(numIndices), isIndexed(true), isTextured(true), hasNormals(true), hasTangents(false)
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createIBO(&iboID, indices, numIndices);
	createVBOT(&vbotgID, texCoords, numVertices * 2, 1, 2);
	createVBON(&vbonID, normals, numVertices * 3, 2, 3);
	unbindBuffers();
}

Model3DGL::Model3DGL(const float* vertices, int numVertices, const int* indices, int numIndices, const float* texCoords, const float* normals, const float* tangents) 
	: numVertices(numVertices), numIndices(numIndices), isIndexed(true), isTextured(true), hasNormals(true), hasTangents(true) 
{
	createVertexArray(&vaoID);
	createVBO(&vboID, vertices, numVertices * 3, 0, 3);
	createIBO(&iboID, indices, numIndices);
	createVBOT(&vbotID, texCoords, numVertices * 2, 1, 2);
	createVBON(&vbonID, normals, numVertices * 3, 2, 3);
	createVBOTG(&vbotgID, tangents, numVertices * 3, 3, 3);
	unbindBuffers();
}

Model3DGL::~Model3DGL() {
	cleanUp();
}

void Model3DGL::update() {

}

void Model3DGL::render() const {
	glBindVertexArray(vaoID);

	glEnableVertexAttribArray(0);
	if (isTextured)
		glEnableVertexAttribArray(1);
	if (hasNormals)
		glEnableVertexAttribArray(2);
	if(hasTangents)
		glEnableVertexAttribArray(3);

	// Render call
	if (isIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else
		glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glDisableVertexAttribArray(0);
	if (isTextured)
		glDisableVertexAttribArray(1);
	if (hasNormals)
		glDisableVertexAttribArray(2);
	if(hasTangents)
		glDisableVertexAttribArray(3);

	glBindVertexArray(0);
}

void Model3DGL::bindForRender() const {
	glBindVertexArray(vaoID);

	glEnableVertexAttribArray(0);
	if (isTextured)
		glEnableVertexAttribArray(1);
	if (hasNormals)
		glEnableVertexAttribArray(2);
	if (hasTangents)
		glEnableVertexAttribArray(3);

	if(isIndexed)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
}

void Model3DGL::renderBuffersOnly() const {
	if (isIndexed)
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Model3DGL::unbindForRender() const {
	if(isIndexed)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	if (isTextured)
		glDisableVertexAttribArray(1);
	if (hasNormals)
		glDisableVertexAttribArray(2);
	if (hasTangents)
		glDisableVertexAttribArray(3);

	glBindVertexArray(0);
}

void Model3DGL::cleanUp() {
	// TODO: Delete all GL buffers
}

void Model3DGL::createVertexArray(GLuint* vaoID) {
	glGenVertexArrays(1, vaoID);
	glBindVertexArray(*vaoID);
}

void Model3DGL::createVBO(GLuint* vboID, const float* vertices, int numVertices, int index, int size) {
	glGenBuffers(1, vboID);
	glBindBuffer(GL_ARRAY_BUFFER, *vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numVertices, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model3DGL::createVBOT(GLuint* vbotID, const float* texCoords, int numTexcoords, int index, int size) {
	glGenBuffers(1, vbotID);
	glBindBuffer(GL_ARRAY_BUFFER, *vbotID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numTexcoords, texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model3DGL::createVBON(GLuint* vbonID, const float* normals, int numNormals, int index, int size) {
	glGenBuffers(1, vbonID);
	glBindBuffer(GL_ARRAY_BUFFER, *vbonID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numNormals, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model3DGL::createVBOTG(GLuint* vbotgID, const float* tangents, int numTangents, int index, int size) {
	glGenBuffers(1, vbotgID);
	glBindBuffer(GL_ARRAY_BUFFER, *vbotgID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numTangents, tangents, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
}

void Model3DGL::createIBO(GLuint* iboID, const int* indices, int numIndices) {
	glGenBuffers(1, iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*numIndices, indices, GL_STATIC_DRAW);
}

void Model3DGL::unbindBuffers() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#endif
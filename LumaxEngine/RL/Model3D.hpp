
#ifndef MODEL_3D_HPP
#define MODEL_3D_HPP

class Model3D {
public:
	virtual void update() = 0;
	virtual void render() const = 0;

	virtual void bindForRender() const = 0;
	virtual void renderBuffersOnly() const = 0;
	virtual void unbindForRender() const = 0;
private:
	virtual void initialize(float* vertices, int numVertices) = 0;
	virtual void initialize(float* vertices, int numVertices, float* texCoords) = 0;
	virtual void initialize(float* vertices, int numVertices, float* texCoords, float* normals) = 0;
	virtual void initialize(float* vertices, int numVertices, float* texCoords, float* normals, float* tangents) = 0;
	virtual void initialize(float* vertices, int numVertices, int* indices, int numIndices) = 0;
	virtual void initialize(float* vertices, int numVertices, int indices, int numIndices, float* texCoords) = 0;
	virtual void initialize(float* vertices, int numVertices, int indices, int numIndices, float* texCoords, float* normals) = 0;
	virtual void initialize(float* vertices, int numVertices, int indices, int numIndices, float* texCoords, float* normals, float* tangents) = 0;

	virtual void cleanUp();
};

#endif
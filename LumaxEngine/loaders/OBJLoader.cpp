
#include "OBJLoader.hpp"

#ifdef _USE_OPENGL

Model3DGL* OBJLoader::loadFileOBJ(const std::string& filename) {
	float vertices[12] = {
		-0.5f,-0.5f, 10,
		-0.5f, 0.5f, 10,
		 0.5f, 0.5f, 10,
		 0.5f,-0.5f, 10
	};
	int indices[6] = {
		0,1,2,2,3,0
	};

	Model3DGL* model = new Model3DGL(vertices, 4, indices, 6);

	return model;
}

#elif defined _USE_DIRECTX11

Model3DDX11* OBJLoader::loadFileOBJ(const std::string& filename) {
	float vertices[12] = {
		-0.5f,-0.5f, 10,
		-0.5f, 0.5f, 10,
		0.5f, 0.5f, 10,
		0.5f,-0.5f, 10
	};
	int indices[6] = {
		0,1,2,2,3,0
	};

	Model3DDX11* model = new Model3DDX11(vertices, 4, indices, 6);

	return model;
}

#endif
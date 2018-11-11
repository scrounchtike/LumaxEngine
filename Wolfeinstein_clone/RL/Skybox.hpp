
#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <string>

#include "../RAL/UsingOpenGL.hpp"

class Skybox{
public:
	Skybox(const std::string* filenames);

	void update();
	void render() const;
	
private:
	GLuint vaoID, vboID;
	GLuint texID;
	void loadCubeTexture(const std::string* filenames);
	void initGL();

	static const float SIZE;
	static const int NUM_VERTICES;
	static const float VERTICES[];
};

#endif

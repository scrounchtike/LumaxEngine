
#include "Skybox.hpp"

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"
#include "../RML/loaders/TextureLoader.hpp"

Skybox::Skybox(const std::string* filenames){
	loadCubeTexture(filenames);
	initGL();
}

void Skybox::update(){
	
}

void Skybox::render() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	
	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
	glBindVertexArray(0);
}

void Skybox::loadCubeTexture(const std::string* filenames){
	texID = TextureLoader::loadCubeTexture(filenames);
}

void Skybox::initGL(){
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUM_VERTICES, &VERTICES[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
  
	glBindVertexArray(0);
}

const float Skybox::SIZE = 1.0;
const int Skybox::NUM_VERTICES = 108;

const float Skybox::VERTICES[] = {
    -SIZE,  SIZE, -SIZE,
    -SIZE, -SIZE, -SIZE,
    SIZE, -SIZE, -SIZE,
    SIZE, -SIZE, -SIZE,
    SIZE,  SIZE, -SIZE,
    -SIZE,  SIZE, -SIZE,
    
    -SIZE, -SIZE,  SIZE,
    -SIZE, -SIZE, -SIZE,
    -SIZE,  SIZE, -SIZE,
    -SIZE,  SIZE, -SIZE,
    -SIZE,  SIZE,  SIZE,
    -SIZE, -SIZE,  SIZE,
    
    SIZE, -SIZE, -SIZE,
    SIZE, -SIZE,  SIZE,
    SIZE,  SIZE,  SIZE,
    SIZE,  SIZE,  SIZE,
    SIZE,  SIZE, -SIZE,
    SIZE, -SIZE, -SIZE,
    
    -SIZE, -SIZE,  SIZE,
    -SIZE,  SIZE,  SIZE,
    SIZE,  SIZE,  SIZE,
    SIZE,  SIZE,  SIZE,
    SIZE, -SIZE,  SIZE,
    -SIZE, -SIZE,  SIZE,
    
    -SIZE,  SIZE, -SIZE,
    SIZE,  SIZE, -SIZE,
    SIZE,  SIZE,  SIZE,
    SIZE,  SIZE,  SIZE,
    -SIZE,  SIZE,  SIZE,
    -SIZE,  SIZE, -SIZE,
    
    -SIZE, -SIZE, -SIZE,
    -SIZE, -SIZE,  SIZE,
    SIZE, -SIZE, -SIZE,
    SIZE, -SIZE, -SIZE,
    -SIZE, -SIZE,  SIZE,
    SIZE, -SIZE,  SIZE
};

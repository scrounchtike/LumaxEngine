
#ifndef WATER_FRAME_BUFFER_HPP
#define WATER_FRAME_BUFFER_HPP

#include "../RAL/UsingOpenGL.hpp"

class WaterFrameBuffer{
public:
	WaterFrameBuffer();

	void bind();
	void unbind();

	void bindTexture();
	
private:
	GLuint fbo;
	GLuint color_tex;
	GLuint depth_rbo;
};

#endif

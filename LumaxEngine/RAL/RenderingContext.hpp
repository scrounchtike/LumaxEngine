
#ifndef RENDERING_CONTEXT_HPP
#define RENDERING_CONTEXT_HPP

#include "ContextDescription.hpp"

class RenderingContext {
public:
	virtual void clearColorBuffer(float color[4]) = 0;
	virtual void clearDepthBuffer(float depthValue = 1.0) = 0;
	virtual void clearBuffers(float color[4], float dephtValue = 1.0) = 0;
	virtual void clearBuffers() = 0;
	virtual void setClearColor(float color[4]) = 0;

	virtual void enableZBuffer() = 0;
	virtual void disableZBuffer() = 0;

	virtual void enableTransparency() = 0;
	virtual void disableTransparency() = 0;

	virtual void setViewport(unsigned int width, unsigned int height) = 0;

	virtual void swapBuffers() = 0;
private:
};

#endif


#ifndef RENDERING_CONTEXT_GL_HPP
#define RENDERING_CONTEXT_GL_HPP

#include "RenderingContext.hpp"

#include <cassert>

#include "UsingOpenGL.hpp"

class RenderingContextGL : public RenderingContext {
public:
	RenderingContextGL(ContextDescription description);
	virtual ~RenderingContextGL();

	virtual void clearColorBuffer(float color[4]);
	virtual void clearDepthBuffer(float depthValue = 1.0);
	virtual void clearBuffers(float color[4], float dephtValue = 1.0);
	virtual void clearBuffers();
	virtual void setClearColor(float color[4]);

	virtual void enableZBuffer();
	virtual void disableZBuffer();

	virtual void enableTransparency();
	virtual void disableTransparency();

	virtual void setViewport(unsigned int width, unsigned int height);

	virtual void swapBuffers();
private:
	ContextDescription description;

	bool initialize();
	bool cleanUp();
};

#endif

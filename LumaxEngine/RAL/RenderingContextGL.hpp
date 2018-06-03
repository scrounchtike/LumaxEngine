
#ifndef RENDERING_CONTEXT_GL_HPP
#define RENDERING_CONTEXT_GL_HPP

#include "RenderingState.hpp"

class RenderingContextGL {
public:
	void setClearColor(float color[4]);
	void clearBuffers();
	void clearColor();
	void clearDepth();
	void clearStencil();
	
	void setFaceCulling(bool);
	void setCullFace(int);
	void setFrontFace(int);

	void setDepth(bool);
	void setDepthTest(bool);
	void setDepthFunc(int);

	void setStencil(bool);
	void setStencilFailOp(int);
	void setStencilDepthFailOp(int);
	void setStencilPassOp(int);
	void setStencilFunc(int);
	void setStencilBackFailOp(int);
	void setStencilBackDepthFailOp(int);
	void setStencilBackPassOp(int);
	void setStencilBackFunc(int);

	void setScissor(bool);

	void setBlending(bool);
	void setBlendSrcOp(int);
	void setBlendDstOp(int);

	void setViewport(unsigned width, unsigned height);

	void setFillMode(int);

	void update();
	void initialize(RenderingState&);

	// Flags and caching
	int stencilFrontUpdate = 0, stencilBackUpdate = 0;
	int stencilFailOp, stencilDepthFailOp, stencilPassOp;
	int stencilBackFailOp, stencilBackDepthFailOp, stencilBackPassOp;

	int blendUpdate = 0;
	int blendSrcOp, blendDstOp;
};

#endif

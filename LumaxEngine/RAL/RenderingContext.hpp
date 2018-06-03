


#ifndef RENDERING_CONTEXT_HPP
#define RENDERING_CONTEXT_HPP

#include "RenderingState.hpp"

#define _USE_OPENGL

#ifdef _USE_OPENGL
#include "RenderingContextGL.hpp"
#elif defined _USE_DIRECTX11
#include "RenderingContextDX11.hpp"
#endif

namespace lmx {
	// Clearing buffers
	void setClearColor(float color[4]){
		if(state.color[0] != color[0] || state.color[1] != color[1] || state.color[2] != color[2] || state.color[3] != color[3])
			state.color = color, context->setClearColor(color);
	}
	void clearBuffers(){
		context->clearBuffers();
	}
	void clearColor(){
		context->clearColor();
	}
	void clearDepth(){
		context->clearDepth();
	}
	void clearStencil(){
		context->clearStencil();
	}

	// Culling
	void setFaceCulling(bool culling){
		if(state.cullingEnabled != culling)
			state.cullingEnabled = culling, context->setFaceCulling(culling);
	}
	void setCullFace(int cullFace){
		if(state.cullFace != cullFace)
			state.cullFace = cullFace, context->setCullFace(cullFace);
	}
	void setFrontFace(int frontFace){
		if(state.frontFace != frontFace)
			state.frontFace = frontFace, context->setFrontFace;
	}

	// Depth Testing
	void setDepth(bool depth){
		if(state.depthEnabled != depth)
			state.depthEnabled = depth, context->setDepth(depth);
	}
	void setDepthClip(bool depthClip){
		if(state.depthClip != depthClip)
			state.depthClip = depthClip, context->setDepthClip();
	}
	void setDepthFunc(int depthFunc){
		if(state.depthFunc != depthFunc)
			state.depthFunc = depthFunc, context->setDepthFunc(depthFunc);
	}

	// Stencil Testing
	void setStencilTest(bool stencil){
		if(state.stencilEnabled != stencil)
			state.stencilEnabled = stencil, context->setStencilTest(stencil);
	}
	void setStencilFailOp(int op){
		if(state.stencilFailOp != op)
			state.stencilFailOp = op, context->setStencilFailOp(op);
	}
	void setStencilDepthFailOp(int op){
		if(state.stencilDepthFailOp != op)
			state.stencilDepthFailOp = op, context->setStencilFailOp(op);
	}
	void setStencilPassOp(int op){
		if(state.stencilPassOp != op)
			state.stencilPassOp = op, context->setStencilPassOp(op);
	}
	void setStencilFunc(int func){
		if(state.stencilFunc != func)
			state.stencilFunc = func, context->setStencilFunc(func);
	}
	void setStencilBackFailOp(int op){
		if(state.stencilBackFailOp != op)
			state.stencilBackFailOp = op, context->setStencilBackFailOp(op);
	}
	void setStencilBackDepthFailOp(int op){
		if(state.stencilBackDepthFailOp != op)
			state.stencilBackDepthFailOp = op, context->setStencilBackDepthFailOp(op);
	}
	void setStencilBackPassOp(int op){
		if(state.stencilBackPassOp != op)
			state.stencilBackPassOp = op, context->setStencilBackPassOp(op);
	}
	void setStencilBackFunc(int func){
		if(state.stencilBackFunc != func)
			state.stencilBackFunc = func, context->setStencilBackFunc(func);
	}

	// Scissoring
	void setScissorTest(bool scissor){
		if(state.scissorEnabled != scissor)
			state.scissorEnabled = scissor, context->setScissorTest(scissor);
	}
	// TODO: Scissor state

	// Blending
	void setBlending(bool blending){
		if(state.blendingEnabled != blending)
			state.blendingEnabled = blending, context->setBlending(blending);
	}
	void setBlendSrcOp(int op){
		if(state.blendSrcOp != op)
			state.blendSrcOp = op, context->setBlendSrcOp(op);
	}n
	void setBlendDstOp(int op){
		if(state.blendDstOp != op)
			state.blendDstOp = op, context->setBlendDstOp)op);
	}

	// Viewport
	void setViewport(unsigned width, unsigned height){
		if(state.width != width || state.height != height){
			state.width = width;
			state.height = height;
			context->setViewport(width, height);
		}
	}

	// Fill Mode
	void setFillMode(int fillMode){
		if(state.fillMode != fillMode)
			state.fillMode = fillMode, context->setFillMode(fillMode);
	}

	// Update after all state changes and before rendering
	void update(){
		context->update();
	}

void initialize(){
	state = RenderingState();
	context->initialize(state);
}

void initialize(const RenderingState& state){
	this->state = state;
	context->initialize(state);
}

void cleanUp(){
	// TODO: Cleanup graphics
}

	static RenderingState state;

#ifdef _USE_OPENGL
	static RenderingContextGL* context;
#elif defined _USE_DIRECTX11
	static RenderingContextDX11* context;
#endif
};

#endif

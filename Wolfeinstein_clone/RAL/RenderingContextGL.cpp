
#include "RenderingContextGL.hpp"

void RenderingContextGL::setClearColor(const float color[4]){
	glClearColor(color[0], color[1], color[2], color[3]);
}

void RenderingContextGL::clearBuffers(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderingContextGL::clearColor(){
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderingContextGL::clearDepth(){
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderingContextGL::clearStencil(){
	glClear(GL_STENCIL_BUFFER_BIT);
}

void RenderingContextGL::setFaceCulling(bool culling){
	if(culling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void RenderingContextGL::setCullFace(int cullFace){
	glCullFace(cullFace);
}

void RenderingContextGL::setFrontFace(int frontFace){
	glFrontFace(frontFace);
}

void RenderingContextGL::setDepth(bool depth){
	glDepthMask(depth);
}

void RenderingContextGL::setDepthTest(bool depthTest){
	if(depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void RenderingContextGL::setDepthFunc(int depthFunc){
	glDepthFunc(depthFunc);
}

void RenderingContextGL::setStencil(bool stencil){
	if(stencil)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
}

void RenderingContextGL::setStencilFailOp(int op){
	stencilFailOp = op;
	stencilFrontUpdate = 1;
}

void RenderingContextGL::setStencilDepthFailOp(int op){
	stencilDepthFailOp = op;
	stencilFrontUpdate = 1;
}

void RenderingContextGL::setStencilPassOp(int op){
	stencilPassOp = op;
	stencilFrontUpdate = 1;
}

void RenderingContextGL::setStencilFunc(int func){
	glStencilFuncSeparate(GL_FRONT, func, 0, 0xFFFFFFFF);
}

void RenderingContextGL::setStencilBackFailOp(int op){
	stencilBackFailOp = op;
	stencilBackUpdate = 1;
}

void RenderingContextGL::setStencilBackDepthFailOp(int op){
	stencilBackDepthFailOp = op;
	stencilBackUpdate = 1;
}

void RenderingContextGL::setStencilBackPassOp(int op){
	stencilBackPassOp = op;
	stencilBackUpdate = 1;
}

void RenderingContextGL::setStencilBackFunc(int func){
	glStencilFuncSeparate(GL_BACK, func, 0, 0xFFFFFFFF);
}

void RenderingContextGL::setScissor(bool scissor){
	if(scissor)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

void RenderingContextGL::setBlending(bool blending){
	if(blending)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void RenderingContextGL::setBlendSrcOp(int op){
	blendSrcOp = op;
	blendUpdate = 1;
}

void RenderingContextGL::setBlendDstOp(int op){
	blendDstOp = op;
	blendUpdate = 1;
}

void RenderingContextGL::setViewport(unsigned width, unsigned height){
	glViewport(0, 0, width, height);
}

void RenderingContextGL::setFillMode(int fillmode){
	glPolygonMode(GL_FRONT_AND_BACK, fillmode);
}

void RenderingContextGL::setRenderingTopology(int topology){
	// Do nothing in GL state
}

void RenderingContextGL::update(){
	if(stencilFrontUpdate)
		glStencilOpSeparate(GL_FRONT, stencilFailOp, stencilDepthFailOp, stencilPassOp);
	if(stencilBackUpdate)
		glStencilOpSeparate(GL_BACK, stencilBackFailOp, stencilBackDepthFailOp, stencilBackPassOp);
	if(blendUpdate)
		glBlendFunc(blendSrcOp, blendDstOp);
	
	// Clear flags
	stencilFrontUpdate = 0;
	stencilBackUpdate = 0;
	blendUpdate = 0;
}

void RenderingContextGL::initialize(const RenderingState& state){
	// Clear color
	setClearColor(&state.color[0]);

	// Culling
	setFaceCulling(state.cullingEnabled);
	setCullFace(state.cullFace);
	setFrontFace(state.frontFace);
	
	// Depth
	setDepth(state.depthEnabled);
	setDepthTest(state.depthTest);
	setDepthFunc(state.depthFunc);
	
	// Stencil
	setStencil(state.stencilEnabled);
	// Front face
	setStencilFailOp(state.stencilFailOp);
	setStencilDepthFailOp(state.stencilDepthFailOp);
	setStencilPassOp(state.stencilPassOp);
	setStencilFunc(state.stencilFunc);
	// Back face
	setStencilBackFailOp(state.stencilBackFailOp);
	setStencilBackDepthFailOp(state.stencilBackDepthFailOp);
	setStencilBackPassOp(state.stencilBackPassOp);
	setStencilBackFunc(state.stencilBackFunc);
  
	// Scissoring
	setScissor(state.scissorEnabled);

	// Blending
	setBlending(state.blendingEnabled);
	setBlendSrcOp(state.blendSrcOp);
	blendSrcOp = state.blendSrcOp;
	setBlendDstOp(state.blendDstOp);
	blendDstOp = state.blendDstOp;

	// Viewport
	setViewport(state.width, state.height);

	// Fill Mode
	setFillMode(state.fillMode);

	// Last GL calls
	update();
}

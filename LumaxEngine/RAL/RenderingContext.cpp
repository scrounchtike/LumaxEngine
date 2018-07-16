
#include "RenderingContext.hpp"

// Clearing buffers
void lmx::setClearColor(float color[4]){
	if(state.color[0] != color[0] || state.color[1] != color[1] || state.color[2] != color[2] || state.color[3] != color[3])
		memcpy(state.color, color, 4*sizeof(float)), context->setClearColor(color);
}
void lmx::clearBuffers(){
	context->clearBuffers();
}
void lmx::clearColor(){
	context->clearColor();
}
void lmx::clearDepth(){
	context->clearDepth();
}
void lmx::clearStencil(){
	context->clearStencil();
}

// Culling
void lmx::setFaceCulling(bool culling){
	if(state.cullingEnabled != culling)
		state.cullingEnabled = culling, context->setFaceCulling(culling);
}
void lmx::setCullFace(int cullFace){
	if(state.cullFace != cullFace)
		state.cullFace = cullFace, context->setCullFace(cullFace);
}
void lmx::setFrontFace(int frontFace){
	if(state.frontFace != frontFace)
		state.frontFace = frontFace, context->setFrontFace(frontFace);
}

// Depth Testing
void lmx::setDepth(bool depth){
	if(state.depthEnabled != depth)
		state.depthEnabled = depth, context->setDepth(depth);
}
void lmx::setDepthClip(bool depthTest){
	if(state.depthTest != depthTest)
		state.depthTest = depthTest, context->setDepthTest(depthTest);
}
void lmx::setDepthFunc(int depthFunc){
	if(state.depthFunc != depthFunc)
		state.depthFunc = depthFunc, context->setDepthFunc(depthFunc);
}

// Stencil Testing
void lmx::setStencilTest(bool stencil){
	if(state.stencilEnabled != stencil)
		state.stencilEnabled = stencil, context->setStencil(stencil);
}
void lmx::setStencilFailOp(int op){
	if(state.stencilFailOp != op)
		state.stencilFailOp = op, context->setStencilFailOp(op);
}
void lmx::setStencilDepthFailOp(int op){
	if(state.stencilDepthFailOp != op)
		state.stencilDepthFailOp = op, context->setStencilFailOp(op);
}
void lmx::setStencilPassOp(int op){
	if(state.stencilPassOp != op)
		state.stencilPassOp = op, context->setStencilPassOp(op);
}
void lmx::setStencilFunc(int func){
	if(state.stencilFunc != func)
		state.stencilFunc = func, context->setStencilFunc(func);
}
void lmx::setStencilBackFailOp(int op){
	if(state.stencilBackFailOp != op)
		state.stencilBackFailOp = op, context->setStencilBackFailOp(op);
}
void lmx::setStencilBackDepthFailOp(int op){
	if(state.stencilBackDepthFailOp != op)
		state.stencilBackDepthFailOp = op, context->setStencilBackDepthFailOp(op);
}
void lmx::setStencilBackPassOp(int op){
	if(state.stencilBackPassOp != op)
		state.stencilBackPassOp = op, context->setStencilBackPassOp(op);
}
void lmx::setStencilBackFunc(int func){
	if(state.stencilBackFunc != func)
		state.stencilBackFunc = func, context->setStencilBackFunc(func);
}

// Scissoring
void lmx::setScissorTest(bool scissor){
	if(state.scissorEnabled != scissor)
		state.scissorEnabled = scissor, context->setScissor(scissor);
}
// TODO: Scissor state

// Blending
void lmx::setBlending(bool blending){
	if(state.blendingEnabled != blending)
		state.blendingEnabled = blending, context->setBlending(blending);
}
void lmx::setBlendSrcOp(int op){
	if(state.blendSrcOp != op)
		state.blendSrcOp = op, context->setBlendSrcOp(op);
}
void lmx::setBlendDstOp(int op){
	if(state.blendDstOp != op)
		state.blendDstOp = op, context->setBlendDstOp(op);
}

// Viewport
void lmx::setViewport(unsigned width, unsigned height){
	if(state.width != width || state.height != height){
		state.width = width;
		state.height = height;
		context->setViewport(width, height);
	}
}

// Rendering Topology
void lmx::setRenderingTopology(int topology){
	if(state.renderingTopology != topology)
		state.renderingTopology = topology, context->setRenderingTopology(topology);
}
int lmx::getRenderingTopology(){
	return state.renderingTopology;
}

// Fill Mode
void lmx::setFillMode(int fillMode){
	if(state.fillMode != fillMode)
		state.fillMode = fillMode, context->setFillMode(fillMode);
}

// Update after all state changes and before rendering
void lmx::update(){
	context->update();
}

void lmx::initialize(){
	state = RenderingState();
#ifdef _USE_OPENGL
	context = new RenderingContextGL();
#elif defined _USE_DIRECTX11
	context = new RenderingContextDX11();
#endif
	context->initialize(state);
}

void lmx::initialize(const RenderingState& state){
	lmx::state = state;
#ifdef _USE_OPENGL
	context = new RenderingContextGL();
#elif defined _USE_DIRECTX11
	context = new RenderingContextDX11();
#endif
	context->initialize(state);
}

void lmx::cleanUp(){
	// TODO: Cleanup graphics
}

#ifdef _USE_DIRECTX11
ID3D11Device* lmx::getDeviceDX11(){
	return context->device;
}
ID3D11DeviceContext* lmx::getDeviceContextDX11(){
	return context->deviceContext;
}
#endif

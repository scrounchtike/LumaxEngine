
#include "RenderingContext.hpp"

void lmx::setClearColor(float color[4]){
	context->setClearColor(color);
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

void lmx::setFaceCulling(bool culling){
	context->setFaceCulling(culling);
}

void lmx::setCullFace(unsigned cull_face){
	context->setCullFace(cull_face);
}

void lmx::setFrontFace(unsigned face){
	context->setFrontFace(face);
}

void lmx::setDepthTest(bool depth){
	context->setDepthTest(depth);
}

void lmx::setDepthFunc(int func){
	context->setDepthFunc(func);
}

void lmx::setStencilTest(bool stencil){
	context->setStencilTest(stencil);
}

void lmx::setStencilState(StencilState & state){
	context->setStencilState(state);
}

void lmx::setScissorTest(bool scissor){
	context->setScissorTest(scissor);
}

void lmx::setBlending(bool blending){
	context->setBlending(blending);
}

void lmx::setBlendState(unsigned src, unsigned dst){
	context->setBlendState(src, dst);
}

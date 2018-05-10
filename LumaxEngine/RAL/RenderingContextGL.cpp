
#include "RenderingContextGL.hpp"

#include "Log.hpp"

RenderingContextGL::RenderingContextGL(ContextDescription desc) : description(desc) {
	bool success = initialize();
	assert(success);
}

RenderingContextGL::~RenderingContextGL() {
	bool success = cleanUp();
	assert(success);
}

void RenderingContextGL::clearColorBuffer(float color[4]) {
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderingContextGL::clearDepthBuffer(float depthValue) {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderingContextGL::clearBuffers(float color[4], float depthValue) {
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderingContextGL::clearBuffers() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderingContextGL::setClearColor(float color[4]) {
	glClearColor(color[0], color[1], color[2], color[3]);
}

void RenderingContextGL::enableZBuffer() {
	glEnable(GL_DEPTH_TEST);
}

void RenderingContextGL::disableZBuffer() {
	glDisable(GL_DEPTH_TEST);
}

void RenderingContextGL::enableTransparency() {
	glEnable(GL_BLEND);
}

void RenderingContextGL::disableTransparency() {
	glDisable(GL_BLEND);
}

void RenderingContextGL::swapBuffers() {
	// DO NOT CALL
	// Call the one in GLFW window rather
	Log::println("ERROR: Tried to call swapbuffers in OpenGL context. The Window is responsible for that (GLFW)");
	assert(false);
}

void RenderingContextGL::setViewport(unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
}

bool RenderingContextGL::initialize() {
	glClearColor(0, 0, 1, 0);

	// Initialization of GLEW
#ifdef _WINDOWS
	if (glewInit()) {
		Log::println("Error: Abort. Could not initialize GLEW");
		assert(false);
		return false;
	}
#endif

	// Depth testing
	if (description.depthBuffer.depthEnable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	// Depth function
	if (description.depthBuffer.depthFunc)
		glDepthFunc(description.depthBuffer.depthFunc);

	// Stencil testing
	if (description.stencilBuffer.stencilEnable)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	// Stencil Function
	if (description.stencilBuffer.stencilFunc)
		glStencilFunc(description.stencilBuffer.stencilFunc, 0, 0xFFFFFFFF);
	// Stencil operations
	glStencilOp(description.stencilBuffer.stencilFailOp == 0 ? GL_KEEP : description.stencilBuffer.stencilFailOp,
				GL_KEEP,
				description.stencilBuffer.stencilDepthFailOp == 0 ? GL_KEEP : description.stencilBuffer.stencilDepthFailOp);

	// Culling test
	if (description.renderBuffer.faceCullEnable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	// Cull face
	glCullFace(description.renderBuffer.faceCullMode == 0 ? GL_BACK : description.renderBuffer.faceCullMode);
	// Cull mode
	glFrontFace(GL_CW);
	if (description.renderBuffer.frontFaceCCW)
		glFrontFace(GL_CCW);

	// Fill Mode
	glPolygonMode(GL_FRONT_AND_BACK, description.renderBuffer.fillMode == 0 ? GL_FILL : description.renderBuffer.fillMode);

	// Scissor test
	if (description.renderBuffer.scissorEnable)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);

	// Blending
	if (description.blendMode.blendEnable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	// Blend function
	glBlendFunc(description.blendMode.srcBlend == 0 ? GL_SRC_ALPHA : description.blendMode.srcBlend,
				description.blendMode.blendFunc == 0 ? GL_ONE_MINUS_SRC_ALPHA : description.blendMode.blendFunc);

	return true;
}

bool RenderingContextGL::cleanUp() {
	return true;
}

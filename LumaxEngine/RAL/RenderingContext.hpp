
#ifndef RENDERING_CONTEXT_HPP
#define RENDERING_CONTEXT_HPP

#include "RenderingState.hpp"

#include "Log.hpp"

#define _USE_OPENGL

#ifdef _USE_OPENGL
#include "RenderingContextGL.hpp"
#elif defined _USE_DIRECTX11
#include "RenderingContextDX11.hpp"
#endif

namespace lmx {
	static RenderingState state;

#ifdef _USE_OPENGL
	static RenderingContextGL* context;
#elif defined _USE_DIRECTX11
	static RenderingContextDX11* context;
#endif
	
	// Clearing buffers
	void setClearColor(float color[4]);
	void clearBuffers();
	void clearColor();
	void clearDepth();
	void clearStencil();

	// Culling
	void setFaceCulling(bool culling);
	void setCullFace(int cullFace);
	void setFrontFace(int frontFace);

	// Depth Testing
	void setDepth(bool depth);
	void setDepthClip(bool depthTest);
	void setDepthFunc(int depthFunc);

	// Stencil Testing
	void setStencilTest(bool stencil);
	void setStencilFailOp(int op);
	void setStencilDepthFailOp(int op);
	void setStencilPassOp(int op);
	void setStencilFunc(int func);
	void setStencilBackFailOp(int op);
	void setStencilBackDepthFailOp(int op);
	void setStencilBackPassOp(int op);
	void setStencilBackFunc(int func);

	// Scissoring
	void setScissorTest(bool scissor);
	// TODO: Scissor state

	// Blending
	void setBlending(bool blending);
	void setBlendSrcOp(int op);
	void setBlendDstOp(int op);

	// Viewport
	void setViewport(unsigned width, unsigned height);

	// Rendering Topology
	void setRenderingTopology(int topology);
	int getRenderingTopology();

	// Fill Mode
	void setFillMode(int fillMode);

	// Update after all state changes and before rendering
	void update();

	void initialize();

	void initialize(const RenderingState& state);

	void cleanUp();

#ifdef _USE_DIRECTX11
	ID3D11Device* getDeviceDX11();
	ID3D11DeviceContext* getDeviceContextDX11();
#endif
};

#endif

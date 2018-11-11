
#ifndef RENDERING_STATE_HPP
#define RENDERING_STATE_HPP

#include "buildDesc.hpp"

#ifdef _USE_OPENGL
#include "UsingOpenGL.hpp"
#elif defined _USE_DIRECTX11
#include "UsingDX11.hpp"
#endif

#ifdef _USE_OPENGL

#define LMX_CULL_FRONT GL_FRONT
#define LMX_CULL_BACK GL_BACK
#define LMX_CULL_BACK_AND_FRONT GL_BACK_AND_FRONT
#define LMX_CCW GL_CCW
#define LMX_CW GL_CW

#define LMX_LEQUAL GL_LEQUAL
#define LMX_LESS GL_LESS
#define LMX_GEQUAL GL_GEQUAL
#define LMX_GREATER GL_GREATER
#define LMX_EQUAL GL_EQUAL
#define LMX_NOTEQUAL GL_NOTEQUAL
#define LMX_ALWAYS GL_ALWAYS
#define LMX_NEVER GL_NEVER

#define LMX_KEEP GL_KEEP
#define LMX_INCR GL_INCR
#define LMX_DECR GL_DECR
#define LMX_ZERO GL_ZERO
#define LMX_INVERT GL_INVERT
#define LMX_REPLACE GL_REPLACE

#define LMX_SRC_COLOR GL_SRC_COLOR
#define LMX_ONE_MINUS_SRC_COLOR GL_ONE_MINUS_SRC_COLOR
#define LMX_SRC_ALPHA GL_SRC_ALPHA
#define LMX_ONE_MINUS_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
#define LMX_DST_ALPHA GL_DST_ALPHA
#define LMX_ONE_MINUS_DST_ALPHA GL_ONE_MINUS_DST_ALPHA

#define LMX_FILL_LINES GL_LINES
#define LMX_FILL_TRIANGLES GL_TRIANGLES

#define LMX_LINES GL_LINES
#define LMX_TRIANGLES GL_TRIANGLES

#elif defined _USE_DIRECTX11

#define LMX_CULL_FRONT D3D11_CULL_FRONT
#define LMX_CULL_BACK D3D11_CULL_BACK
#define LMX_CULL_BACK_AND_FRONT -1
#define LMX_CCW true
#define LMX_CW false

#define LMX_LEQUAL D3D11_COMPARISON_LESS_EQUAL
#define LMX_LESS D3D11_COMPARISON_LESS
#define LMX_GEQUAL D3D11_COMPARISON_GREATER_EQUAL
#define LMX_GREATER D3D11_COMPARISON_GREATER
#define LMX_EQUAL D3D11_COMPARISON_EQUAL
#define LMX_NOTEQUAL D3D11_COMPARISON_NOT_EQUAL
#define LMX_ALWAYS D3D11_COMPARISON_ALWAYS
#define LMX_NEVER D3D11_COMPARISON_NEVER

#define LMX_KEEP D3D11_STENCIL_OP_KEEP
#define LMX_INCR D3D11_STENCIL_OP_INCR
#define LMX_DECR D3D11_STENCIL_OP_DECR
#define LMX_ZERO D3D11_STENCIL_OP_ZERO
#define LMX_INVERT D3D11_STENCIL_OP_INVERT
#define LMX_REPLACE D3D11_STENCIL_OP_REPLACE

#define LMX_SRC_COLOR D3D11_BLEND_SRC_COLOR
#define LMX_ONE_MINUS_SRC_COLOR D3D11_BLEND_INV_SRC_COLOR
#define LMX_SRC_ALPHA D3D11_BLEND_SRC_ALPHA
#define LMX_ONE_MINUS_SRC_ALPHA D3D11_BLEND_INV_SRC_ALPHA
#define LMX_DST_ALPHA D3D11_BLEND_DEST_ALPHA
#define LMX_ONE_MINUS_DST_ALPHA D3D11_BLEND_INV_DEST_ALPHA

#define LMX_FILL_LINES D3D11_FILL_WIREFRAME
#define LMX_FILL_TRIANGLES D3D11_FILL_SOLID

#define LMX_LINES D3D11_PRIMITIVE_TOPOLOGY_LINES
#define LMX_TRIANGLES D3D11_PRIMITIVE_TOPOLOGY_TRIANGLES

#endif

struct RenderingState {
	RenderingState() : width(800), height(600) { }
	RenderingState(unsigned width, unsigned height) : width(width), height(height) { }
	float color[4];

	bool cullingEnabled = true;
	int cullFace = LMX_CULL_BACK;
	int frontFace = LMX_CW;

	bool depthEnabled = true;
	bool depthTest = true;
	int depthFunc = LMX_LEQUAL;
	
	bool stencilEnabled = false;
	int stencilFailOp = LMX_KEEP;
	int stencilDepthFailOp = LMX_KEEP;
	int stencilPassOp = LMX_KEEP;
	int stencilFunc = LMX_ALWAYS;
	int stencilBackFailOp = LMX_KEEP;
	int stencilBackDepthFailOp = LMX_KEEP;
	int stencilBackPassOp = LMX_KEEP;
	int stencilBackFunc = LMX_ALWAYS;
	
	bool scissorEnabled = false;
	// TODO: Add scissoring support
	
	bool blendingEnabled = false;
	int blendSrcOp = LMX_SRC_ALPHA;
	int blendDstOp = LMX_ONE_MINUS_SRC_ALPHA;
	
	unsigned width = 800, height = 600;

	int fillMode = LMX_FILL_TRIANGLES;
	int renderingTopology = LMX_TRIANGLES;
};

#endif

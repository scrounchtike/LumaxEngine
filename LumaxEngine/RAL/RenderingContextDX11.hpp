
#ifndef RENDERING_CONTEXT_DX11_HPP
#define RENDERING_CONTEXT_DX11_HPP

#include "RenderingState.hpp"

#ifdef _USE_DIRECTX11

class RenderingContextDX11 {
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

	void setRenderingTopology(int topology);

	void update();
	void initialize(const RenderingState&);

	/*
		DX11 Data and caching
	*/

	// flags
#define DEPTH_STENCIL 1
#define RASTER 2
#define BLENDING 4
#define VIEWPORT 8
	unsigned int updateFlag = 0;
	
	float color[4];

	// Depth Stencil
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	// Raster
	D3D11_RASTERIZER_DESC rasterDesc;

	// Blending
	D3D11_BLEND_DESC blendStateDesc;

	// Viewport
	D3D11_VIEWPORT viewportDesc;
	
	/*
		DX11 State buffers
	*/
	
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;

	//Depth Stencil
	ID3D11Texture2D* depthStencilTexture;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;

	// Raster
	ID3D11RasterizerState* rasterState;

	// Target View
	ID3D11RenderTargetView* renderTargetView;

	// Blending
	ID3D11BlendState* blendState;

	D3D_FEATURE_LEVEL featureLevel;
};

#endif
#endif

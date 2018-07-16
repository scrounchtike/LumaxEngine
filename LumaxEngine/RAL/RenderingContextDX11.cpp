
#include "RenderingContextDX11.hpp"

#ifdef _USE_DIRECTX11

void RenderingContextDX11::setClearColor(float color[4]){
	memcpy(&this->color[0], &color[0], 4*sizeof(float));
}

void RenderingContextDX11::clearBuffers(){
	clearColor();
	clearDepth();
	clearStencil();
}

void RenderingContextDX11::clearColor(){
	deviceContext->ClearRenderTargetView(renderTargetView, color);
}

void RenderingContextDX11::clearDepth(){
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
}

void RenderingContextDX11::clearStencil(){
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
}
	
void RenderingContextDX11::setFaceCulling(bool culling){
	if(culling)
		rasterDesc.CullMode = D3D11_CULL_BACK;
	else
		rasterDesc.CullMode = D3D11_CULL_NONE;
	updateFlag |= RASTER;
}

void RenderingContextDX11::setCullFace(int cullFace){
	rasterDesc.CullMode = cullFace;
	updateFlag |= RASTER;
}

void RenderingContextDX11::setFrontFace(int frontFace){
	rasterDesc.FrontCounterClockwise = frontFace;
	updateFlag |= RASTER;
}

void RenderingContextDX11::setDepth(bool depth){
	depthStencilStateDesc.DepthEnable = depth;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setDepthTest(bool depthTest){
	rasterDesc.DepthClipEnable = depthTest;
	updateFlag |= RASTER;
}

void RenderingContextDX11::setDepthFunc(int func){
	depthStencilStateDesc.DepthFunc = func;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencil(bool stencil){
	depthStencilStateDesc.StencilEnable = stencil;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilFailOp(int op){
	depthStencilStateDesc.FrontFace.StencilFailOp = op;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilDepthFailOp(int op){
	depthStencilStateDesc.FrontFace.StencilDepthFailOp = op;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilPassOp(int op){
	depthStencilStateDesc.FrontFace.StencilPassOp = op;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilFunc(int func){
	depthStencilStateDesc.FrontFace.StencilFunc = func;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilBackFailOp(int op){
	depthStencilStateDesc.BackFace.StencilFailOp = op;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilBackDepthFailOp(int op){
	depthStencilStateDesc.BackFace.StencilDepthFailOp = op;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilBackPassOp(int op){
	depthStencilStateDesc.BackFace.StencilPassOp = op;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setStencilBackFunc(int func){
	depthStencilStateDesc.BackFace.StencilFunc = func;
	updateFlag |= DEPTH_STENCIL;
}

void RenderingContextDX11::setScissor(bool scissor){
	rasterDesc.ScissorEnable = scissor;
	updateFlag |= RASTER;
}

void RenderingContextDX11::setBlending(bool blending){
	blendStateDesc.RenderTarget[0].BlendEnable = blending;
	updateFlag |= BLENDING;
}

void RenderingContextDX11::setBlendSrcOp(int op){
	blendStateDesc.RenderTarget[0].SrcBlend = op;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = op;
	updateFlag |= BLENDING;
}

void RenderingContextDX11::setBlendDstOp(int op){
	blendStateDesc.RenderTarget[0].DstBlend = op;
	blendStateDesc.RenderTarget[0].DstBlendAlpha = op;
	updateFlag |= BLENDING;
}

void RenderingContextDX11::setViewport(unsigned width, unsigned height){
	viewportDesc.Width = (float)width;
	viewportDesc.Height = (float)height;
	updateFlag |= VIEWPORT;
}

void RenderingContextDX11::setFillMode(int fillmode){
	rasterDesc.FillMode = fillmode;
	updateFlag |= RASTER;
}

void RenderingContextDX11::setRenderingTopology(int topology){
	deviceContext->IASetPrimitiveTopology(topology);
}

void RenderingContextDX11::update(){
	HRESULT result;
	// Depth Stencil
	if(updateFlag & DEPTH_STENCIL){
		result = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
		if(FAILED(result)){
			Log::println("Error: Could not update depth stencil state in DX11 rendering context");
			assert(false);
		}
		deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	}

	// Raster
	if(updateFlag & RASTER){
		result = device->CreateRasterizerState(&rasterDesc, &rasterState);
		if(FAILED(result)){
			Log::println("Error: Could not update raster state in DX11 rendering context");
			assert(false);
		}
		deviceContext->RSSetState(rasterState);
	}

	// Blending
	if(updateFlag |= BLENDING){
		result = device->CreateBlendState(&blendStateDesc, &blendState);
		if(FAILED(result)){
			Log::println("Error: Could not update blending state in DX11 rendering context");
			assert(false);
		}
		deviceContext->OMSetBlendState(blendState, 0, 0xFFFFFFFF);
	}

	// Viewport
	if(updateFlag & VIEWPORT){
		deviceContext->RSSetViewports(1, &viewportDesc);
	}
	
	// Clear flags
	updateFlag &= 0;
}

void RenderingContextDX11::initialize(const RenderingState& state){
	HRESULT result;

	// Rendering Context initialization
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	
	// Creating window framebuffer
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = state.width;
	swapChainDesc.BufferDesc.Height = state.height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60.0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.OutputWindow = getStaticHWND();
	swapChainDesc.Windowed = true;
	
	// Scanline mode
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create device and swap chain
	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;

	int flags = 0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevels, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &deviceContext);
	if (FAILED(result)) {
		Log::println("Error: Could not create D3D11 device and DXGI swapChain during DX11 rendering context initialization");
		assert(false);
	}

	// Creating the backbuffer
	ID3D11Texture2D* backBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	result = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	if(FAILED(result)){
		Log::println("Error: Could not create back buffer texture during DX11 rendering context initialization");
		assert(false);
	}
	backBuffer->Release();
	backBuffer = 0;
	
	// Depth Stencil
	// Creating the depth/stencil texture
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = state.width;
	depthBufferDesc.Height = state.height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SamplerDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilTexture);
	if(FAILED(result)){
		Log::println("Error: Could not create depth / stencil texture during DX11 rendering context initialization");
		assert(false);
	}

	// Depth State description
	ZeroMemory(&depthStencilStateDesc, sizeof(depthStencilStateDesc));
	depthStencilStateDesc.DepthEnable = state.depthEnabled;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = state.depthFunc;

	// Stencil State description
	depthStencilStateDesc.StencilEnable = state.stencilEnabled;
	depthStencilStateDesc.StencilReadMask = 0xFF;
	depthStencilStateDesc.StencilWriteMask = 0xFF;
	
	// Stencil operations front-face
	depthStencilStateDesc.FrontFace.StencilFailOp = state.stencilFailOp;
	depthStencilStateDesc.FrontFace.StencilDepthFailOp = state.stencilDepthFailOp;
	depthStencilStateDesc.FrontFace.StencilPassOp = state.stencilPassOp;
	depthStencilStateDesc.FrontFace.StencilFunc = state.stencilFunc;

	// Stencil operations back-face
	depthStencilStateDesc.BackFace.StencilFailOp = state.stencilBackFailOp;
	depthStencilStateDesc.BackFace.StencilDepthFailOp = state.stencilBackDepthFailOp;
	depthStencilStateDesc.BackFace.StencilPassOp = state.stencilBackPassOp;
	depthStencilStateDesc.BackFace.StencilFunc = state.stencilBackFunc;

	// Depth Stencil state
	result = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
	if(FAILED(result)){
		Log::println("Error: Could not create depth stencil state during DX11 rendering context initialization");
		assert(false);
	}

	// Set the depth stencil state
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// Creating the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	
	result = device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &depthStencilView);
	if(FAILED(result)){
		Log::println("Error: Could not create depth stencil view during DX11 rendering context initialization");
		assert(false);
	}

	// Set the depth stencil view to the context
	deviceContext->OMsetRenderTargets(1, &renderTargetView, depthStencilView);
	
	// Raster
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	setFaceCulling(state.cullingEnabled);
	rasterDesc.DepthClipEnable = state.depthTest;
	rasterDesc.FillMode = state.fillMode;
	rasterDesc.FrontCounterClockwise = state.frontFace;
	rasterDesc.ScissorEnable = state.scissorEnabled;

	// Create Raster state
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if(FAILED(result)){
		Log::println("Error: Could not create raster state during DX11 rendering context initialization");
		assert(false);
	}

	// Set the raster state
	deviceContext->RSSetState(rasterState);

	// Blending
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
	blendStateDesc.RenderTarget[0].BlendEnable = state.blendingEnabled;
	blendStateDesc.RenderTarget[0].SrcBlend = state.blendSrcOp;
	blendStateDesc.RenderTarget[0].DstBlend = state.blendDstOp;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = state.blendSrcOp;
	blendStateDesc.RenderTarget[0].DstBlendAlpha = state.blendDstOp;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	// Create blend state
	result = device->CreateBlendState(&blendStateDesc, &blendState);
	if(FAILED(result)){
		Log::println("Error: Could not create blend state during DX11 context initialization");
		assert(false);
	}

	// Set blend state 
	deviceContext->OMSetBlendState(blendState, 0, 0xFFFFFFFF);

	// Viewport
	viewportDesc.Width = (float)state.width;
	viewportDesc.Height = (float)state.height;
	viewportDesc.MinDepth = 0.0f;
	viewportDesc.MaxDepth = 1.0f;
	viewportDesc.TopLeftX = 0.0f;
	viewportDesc.TopLeftY = 0.0f;

	// Set the viewport
	deviceContext->RSSetViewports(1, &viewportDesc);
}

#endif

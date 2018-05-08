
#include "RenderingContextDX11.hpp"

#ifdef _USE_DIRECTX11

#include "Log.hpp"

RenderingContextDX11::RenderingContextDX11(ContextDescription description, HWND hwnd)
	: state(description), hwnd(hwnd)
{
	bool success = initialize();
	assert(success);
}

RenderingContextDX11::~RenderingContextDX11() {
	bool success = cleanUp();
	assert(success);
}

void RenderingContextDX11::clearColorBuffer(float color[4]) {
	deviceContext->ClearRenderTargetView(renderTargetView, color);
}

void RenderingContextDX11::clearDepthBuffer(float depthValue) {
	deviceContext->ClearDepthStencilView(depthStencilDescriptions[depthStencilIndex].depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthValue, 0.0f);
}

void RenderingContextDX11::clearBuffers(float color[4], float depthValue){
	clearColorBuffer(color);
	clearDepthBuffer(depthValue);
}

void RenderingContextDX11::clearBuffers() {
	clearColorBuffer(defaultColor);
	clearDepthBuffer(defaultDepthValue);
}

void RenderingContextDX11::setClearColor(float color[4]) {
	defaultColor[0] = color[0];
	defaultColor[1] = color[1];
	defaultColor[2] = color[2];
	defaultColor[3] = color[3];
}

void RenderingContextDX11::enableZBuffer() {
	// TODO
}

void RenderingContextDX11::disableZBuffer() {
	// TODO
}

void RenderingContextDX11::enableTransparency() {
	deviceContext->OMSetBlendState(transparencyEnabledBlendState, 0, 0xFFFFFFFF);
}

void RenderingContextDX11::disableTransparency() {
	deviceContext->OMSetBlendState(transparencyDisabledBlendState, 0, 0xFFFFFFFF);
}

void RenderingContextDX11::setViewport(unsigned int width, unsigned int height) {
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Set the viewport
	deviceContext->RSSetViewports(1, &viewport);
}

void RenderingContextDX11::swapBuffers() {
	swapChain->Present(1, 0);
}

void RenderingContextDX11::registerDepthStencilView(DepthStencilDescription description, const std::string& stateID) {
	int newDepthStencilIndex = depthStencilDescriptions.size();
	DepthStencilState state;

	// Use same buffer as main depth stencil state
	// Somehow cannot create a new depth buffer?
	state.depthStencilBuffer = depthStencilDescriptions[0].depthStencilBuffer;

	HRESULT result;

	// Fill the depth / stencil state description
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(depthStencilStateDesc));
	// Depth testing
	depthStencilStateDesc.DepthEnable = description.depthBuffer.depthEnable;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	D3D11_COMPARISON_FUNC depthFunc = description.depthBuffer.depthFunc != 0 ? (D3D11_COMPARISON_FUNC)description.depthBuffer.depthFunc : D3D11_COMPARISON_LESS_EQUAL;
	depthStencilStateDesc.DepthFunc = depthFunc;

	// Stencil testing
	depthStencilStateDesc.StencilEnable = description.stencilBuffer.stencilEnable;
	depthStencilStateDesc.StencilReadMask = 0xFF;
	depthStencilStateDesc.StencilWriteMask = 0xFF;

	// Stencil operations front-face
	D3D11_STENCIL_OP stencilFailOp = description.stencilBuffer.stencilFailOp != 0 ? (D3D11_STENCIL_OP)description.stencilBuffer.stencilFailOp : D3D11_STENCIL_OP_KEEP;
	D3D11_STENCIL_OP stencilDepthFailOp = description.stencilBuffer.stencilDepthFailOp != 0 ? (D3D11_STENCIL_OP)description.stencilBuffer.stencilDepthFailOp : D3D11_STENCIL_OP_KEEP;
	D3D11_STENCIL_OP stencilPassOp = description.stencilBuffer.stencilPassOp != 0 ? (D3D11_STENCIL_OP)description.stencilBuffer.stencilPassOp : D3D11_STENCIL_OP_KEEP;
	D3D11_COMPARISON_FUNC stencilFunc = description.stencilBuffer.stencilFunc != 0 ? (D3D11_COMPARISON_FUNC)description.stencilBuffer.stencilFunc : D3D11_COMPARISON_ALWAYS;
	depthStencilStateDesc.FrontFace.StencilFailOp = stencilFailOp;
	depthStencilStateDesc.FrontFace.StencilDepthFailOp = stencilDepthFailOp;
	depthStencilStateDesc.FrontFace.StencilPassOp = stencilPassOp;
	depthStencilStateDesc.FrontFace.StencilFunc = stencilFunc;

	// Stencil operations back-face
	depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth / stencil state
	result = device->CreateDepthStencilState(&depthStencilStateDesc, &state.depthStencilState);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create depth / stencil state\n");
		assert(false);
		return;
	}

	// Create depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	DXGI_FORMAT depthStencilFormat = description.depthBuffer.format != 0 ? (DXGI_FORMAT)description.depthBuffer.format : DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.Format = depthStencilFormat;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth / stencil view
	result = device->CreateDepthStencilView(state.depthStencilBuffer, &depthStencilViewDesc, &state.depthStencilView);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create depth / stencil view in DX11 initialization\n");
		assert(false);
		return;
	}

	// Register the new state
	depthStencilDescriptions.push_back(state);
	map.insert(std::pair<const std::string, int>(stateID, newDepthStencilIndex));
}

void RenderingContextDX11::changeState(const std::string& stateID) {
	if (stateID == "main")
		depthStencilIndex = 0;
	else {
		std::map<std::string, int>::iterator it = map.find(stateID);
		if (it != map.end())
			depthStencilIndex = it->second;
		else {
			Log::println("Error: Tried to acquire a depth stencil description not present in DX11 rendering context: " + stateID);
			assert(false);
		}
	}

	// Bind selected state
	deviceContext->OMSetDepthStencilState(depthStencilDescriptions[depthStencilIndex].depthStencilState, 1);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilDescriptions[depthStencilIndex].depthStencilView);
}

bool RenderingContextDX11::initialize() {
	bool success;
	success = createRenderBuffer();
	assert(success);
	success = createDepthStencilBuffer();
	assert(success);
	success = createRasterMode();
	assert(success);

	success = createBlendMode(state.blendMode, blendState);
	assert(success);
	// Using the blend state
	deviceContext->OMSetBlendState(blendState, 0, 0xFFFFFFFF);

	// Creating both transparency states
	BlendModeDescription blendMode;
	blendMode.blendEnable = true;
	blendMode.blendFunc = state.blendMode.blendFunc;
	blendMode.dstBlend = state.blendMode.dstBlend;
	blendMode.srcBlend = state.blendMode.srcBlend;
	success = createBlendMode(blendMode, transparencyEnabledBlendState);
	assert(success);
	blendMode.blendEnable = false;
	success = createBlendMode(blendMode, transparencyDisabledBlendState);
	assert(success);

	success = createViewport();
	assert(success);

	return success;
}

bool RenderingContextDX11::createRenderBuffer() {
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// Creating the window framebuffer for rendering
	swapChainDesc.BufferCount = state.renderBuffer.bufferCount;
	swapChainDesc.BufferDesc.Width = state.renderBuffer.width;
	swapChainDesc.BufferDesc.Height = state.renderBuffer.height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = state.renderBuffer.refreshRate;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGI_FORMAT format = state.renderBuffer.format != 0 ? (DXGI_FORMAT)state.renderBuffer.format : DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Format = format;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;

	// Specifying the scanline mode
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Use DirectX 11 as feature level
	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;

	int flags = 0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &featureLevels, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &deviceContext);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create D3D11 device and DXGI swapChain.\n");
		assert(false);
		return false;
	}

	// Creating the back buffer (Render Target View)
	ID3D11Texture2D* backBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	result = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create render target view during DX11 initialization\n");
		assert(false);
		return false;
	}
	backBuffer->Release();
	backBuffer = 0;

	return true;
}

bool RenderingContextDX11::createDepthStencilBuffer() {
	HRESULT result;

	DepthStencilState desc;
	depthStencilDescriptions.push_back(desc);

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = state.renderBuffer.width;
	depthBufferDesc.Height = state.renderBuffer.height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	DXGI_FORMAT depthFormat = state.depthBuffer.format != 0 ? (DXGI_FORMAT)state.depthBuffer.format : DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.Format = depthFormat;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Create the texture for the depth / stencil buffer
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilDescriptions[0].depthStencilBuffer);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create depth texture required for depth testing\n");
		assert(false);
		return false;
	}

	// Fill the depth / stencil state description
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(depthStencilStateDesc));
	// Depth testing
	depthStencilStateDesc.DepthEnable = state.depthBuffer.depthEnable;
	//depthStencilStateDesc.DepthEnable = false;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	D3D11_COMPARISON_FUNC depthFunc = state.depthBuffer.depthFunc != 0 ? (D3D11_COMPARISON_FUNC)state.depthBuffer.depthFunc : D3D11_COMPARISON_LESS_EQUAL;
	depthStencilStateDesc.DepthFunc = depthFunc;

	// Stencil testing
	depthStencilStateDesc.StencilEnable = state.stencilBuffer.stencilEnable;
	depthStencilStateDesc.StencilReadMask = 0xFF;
	depthStencilStateDesc.StencilWriteMask = 0xFF;

	// Stencil operations front-face
	D3D11_STENCIL_OP stencilFailOp = state.stencilBuffer.stencilFailOp != 0 ? (D3D11_STENCIL_OP)state.stencilBuffer.stencilFailOp : D3D11_STENCIL_OP_KEEP;
	D3D11_STENCIL_OP stencilDepthFailOp = state.stencilBuffer.stencilDepthFailOp != 0 ? (D3D11_STENCIL_OP)state.stencilBuffer.stencilDepthFailOp : D3D11_STENCIL_OP_KEEP;
	D3D11_STENCIL_OP stencilPassOp = state.stencilBuffer.stencilPassOp != 0 ? (D3D11_STENCIL_OP)state.stencilBuffer.stencilPassOp : D3D11_STENCIL_OP_KEEP;
	D3D11_COMPARISON_FUNC stencilFunc = state.stencilBuffer.stencilFunc != 0 ? (D3D11_COMPARISON_FUNC)state.stencilBuffer.stencilFunc : D3D11_COMPARISON_ALWAYS;
	depthStencilStateDesc.FrontFace.StencilFailOp = stencilFailOp;
	depthStencilStateDesc.FrontFace.StencilDepthFailOp = stencilDepthFailOp;
	depthStencilStateDesc.FrontFace.StencilPassOp = stencilPassOp;
	depthStencilStateDesc.FrontFace.StencilFunc = stencilFunc;

	// Stencil operations back-face
	depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth / stencil state
	result = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilDescriptions[0].depthStencilState);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create depth / stencil state\n");
		assert(false);
		return false;
	}

	// Setting the depth/stencil state
	deviceContext->OMSetDepthStencilState(depthStencilDescriptions[0].depthStencilState, 1);

	// Create depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	DXGI_FORMAT depthStencilFormat = state.depthBuffer.format != 0 ? (DXGI_FORMAT)state.depthBuffer.format : DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.Format = depthStencilFormat;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth / stencil view
	result = device->CreateDepthStencilView(depthStencilDescriptions[0].depthStencilBuffer, &depthStencilViewDesc, &depthStencilDescriptions[0].depthStencilView);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create depth / stencil view in DX11 initialization\n");
		assert(false);
		return false;
	}

	// Set the depth / stencil view to the context
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilDescriptions[0].depthStencilView);

	return true;
}

bool RenderingContextDX11::createRasterMode() {
	HRESULT result;
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	rasterDesc.AntialiasedLineEnable = false;
	D3D11_CULL_MODE cullMode = D3D11_CULL_BACK;
	if (state.renderBuffer.faceCullEnable && (state.renderBuffer.faceCullMode != 0))
		cullMode = (D3D11_CULL_MODE)state.renderBuffer.faceCullMode;
	//rasterDesc.CullMode = cullMode;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.DepthClipEnable = true;
	D3D11_FILL_MODE fillMode = state.renderBuffer.fillMode != 0 ? (D3D11_FILL_MODE)state.renderBuffer.fillMode : D3D11_FILL_SOLID;
	rasterDesc.FillMode = fillMode;
	rasterDesc.FrontCounterClockwise = state.renderBuffer.frontFaceCCW;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = state.renderBuffer.scissorEnable;
	rasterDesc.SlopeScaledDepthBias = 0;

	// Creation of Rasterization state
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create rasterizer state during DirectX 11 initialization.\n");
		assert(false);
		return false;
	}

	// Setting of raster state
	deviceContext->RSSetState(rasterState);

	return true;
}

bool RenderingContextDX11::createBlendMode(BlendModeDescription blendMode, ID3D11BlendState* blendState) {
	HRESULT result;
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));

	blendStateDesc.RenderTarget[0].BlendEnable = state.blendMode.blendEnable;
	D3D11_BLEND srcBlend = state.blendMode.srcBlend != 0 ? (D3D11_BLEND)state.blendMode.srcBlend : D3D11_BLEND_SRC_ALPHA;
	D3D11_BLEND dstBlend = state.blendMode.dstBlend != 0 ? (D3D11_BLEND)state.blendMode.dstBlend : D3D11_BLEND_INV_SRC_ALPHA;
	D3D11_BLEND_OP blendOp = state.blendMode.blendFunc != 0 ? (D3D11_BLEND_OP)state.blendMode.blendFunc : D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlend = srcBlend;
	blendStateDesc.RenderTarget[0].DestBlend = dstBlend;
	blendStateDesc.RenderTarget[0].BlendOp = blendOp;

	// For now, let them have default values
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	result = device->CreateBlendState(&blendStateDesc, &blendState);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create transparancy blend state during DX11 context initialization\n");
		assert(false);
		return false;
	}

	return true;
}

bool RenderingContextDX11::createViewport() {
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)state.renderBuffer.width;
	viewport.Height = (float)state.renderBuffer.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Setting the viewport
	deviceContext->RSSetViewports(1, &viewport);

	return true;
}

bool RenderingContextDX11::cleanUp() {
	if (deviceContext) {
		deviceContext->Release();
		delete deviceContext;
		deviceContext = 0;
	}
	if (device) {
		device->Release();
		device = 0;
	}
	if (swapChain) {
		swapChain->Release();
		swapChain = 0;
	}
	if (rasterState) {
		rasterState->Release();
		rasterState = 0;
	}
	if (renderTargetView) {
		renderTargetView->Release();
		renderTargetView = 0;
	}
	if (blendState) {
		blendState->Release();
		blendState = 0;
		transparencyEnabledBlendState->Release();
		transparencyEnabledBlendState = 0;
		transparencyDisabledBlendState->Release();
		transparencyDisabledBlendState = 0;
	}
	for (int i = 0; i < depthStencilDescriptions.size(); ++i) {
		if (depthStencilDescriptions[i].depthStencilBuffer) {
			depthStencilDescriptions[i].depthStencilBuffer->Release();
			depthStencilDescriptions[i].depthStencilBuffer = 0;
		}
		if (depthStencilDescriptions[i].depthStencilState) {
			depthStencilDescriptions[i].depthStencilState->Release();
			depthStencilDescriptions[i].depthStencilState = 0;
		}
		if (depthStencilDescriptions[i].depthStencilView) {
			depthStencilDescriptions[i].depthStencilView->Release();
			depthStencilDescriptions[i].depthStencilView = 0;
		}
	}

	return true;
}

#endif
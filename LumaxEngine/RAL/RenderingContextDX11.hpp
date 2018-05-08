
#include "buildDesc.hpp"

#if !defined(RENDERING_CONTEXT_DX11_HPP) && defined(_USE_DIRECTX11)
#define RENDERING_CONTEXT_DX11_HPP

#include "RenderingContext.hpp"
#include "UsingDX11.hpp"

#include <vector>
#include <string>
#include <map>

struct DepthStencilState {
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
};

class RenderingContextDX11 : public RenderingContext {
public:
	RenderingContextDX11(ContextDescription description, HWND hwnd);
	~RenderingContextDX11();

	virtual void clearColorBuffer(float color[4]);
	virtual void clearDepthBuffer(float depthValue = 1.0);
	virtual void clearBuffers(float color[4], float depthValue = 1.0);
	virtual void clearBuffers();
	virtual void setClearColor(float color[4]);

	virtual void enableZBuffer();
	virtual void disableZBuffer();

	virtual void enableTransparency();
	virtual void disableTransparency();

	virtual void setViewport(unsigned int width, unsigned int height);

	virtual void swapBuffers();

	// Allow state modification
	void registerDepthStencilView(DepthStencilDescription depthStencilDescription, const std::string& stateIdentifier);
	void changeState(const std::string& stateIdentifier);

	ID3D11Device* getDevice() { return device; }
	ID3D11DeviceContext* getDeviceContext() { return deviceContext; }

	ID3D11DepthStencilView* getDepthStencilView() { return depthStencilDescriptions[depthStencilIndex].depthStencilView; }
private:
	ContextDescription state;

	// Clear color
	float defaultColor[4];
	float defaultDepthValue = 1.0;

	// Window reference
	HWND hwnd;

	// DX11 State declarations
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;

	ID3D11RasterizerState* rasterState;
	ID3D11RenderTargetView* renderTargetView;

	int depthStencilIndex = 0;
	std::vector<DepthStencilState> depthStencilDescriptions;
	std::map<const std::string, int> map;

	// Blend states
	ID3D11BlendState* blendState;
	ID3D11BlendState* transparencyEnabledBlendState;
	ID3D11BlendState* transparencyDisabledBlendState;

	ID3D11BlendState* transparencyState;

	D3D_FEATURE_LEVEL featureLevel;

	bool initialize();
	bool cleanUp();

	bool createRenderBuffer();
	bool createDepthStencilBuffer();
	bool createRasterMode();
	bool createBlendMode(BlendModeDescription, ID3D11BlendState* blendState);
	bool createViewport();
};

#endif
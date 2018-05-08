
struct DepthBufferDescription {
	bool depthEnable = true;
	int depthFunc = 0;
	int format = 0;
};

struct StencilBufferDescription {
	bool stencilEnable = false;
	// Only support for front face stencil buffer for now
	int stencilFunc = 0;
	int stencilFailOp = 0;
	int stencilDepthFailOp = 0;
	int stencilPassOp = 0;
};

struct RenderBufferDescription {
	int bufferCount = 1;
	int width = 800;
	int height = 600;
	int refreshRate = 60;
	bool faceCullEnable = true;
	int faceCullMode = 0;
	// By default, use DX11 CW front faces
	bool frontFaceCCW = false;
	int fillMode = 0;
	int format = 0;
	bool scissorEnable = false;
};

struct BlendModeDescription {
	bool blendEnable = true;
	int blendFunc = 0;
	int srcBlend = 0;
	int dstBlend = 0;
};

class ContextDescription {
public:
	DepthBufferDescription depthBuffer;
	StencilBufferDescription stencilBuffer;
	RenderBufferDescription renderBuffer;
	BlendModeDescription blendMode;
};

class DepthStencilDescription {
public:
	DepthBufferDescription depthBuffer;
	StencilBufferDescription stencilBuffer;
	int width = 800, height = 600;
};
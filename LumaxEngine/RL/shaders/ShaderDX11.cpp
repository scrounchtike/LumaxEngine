
#include "ShaderDX11.hpp"
#include "../Shader.hpp"

#ifdef _USE_DIRECTX11

#include <fstream>

#include "../../RAL/UsingDX11.hpp"
#include "../../RAL/Log.hpp"
#include "../../main.hpp"

ShaderDX11::ShaderDX11(const ShaderInformation& info) {
	this->dx11 = getStaticRenderingContextDX11();

	initialize(info);
}

ShaderDX11::~ShaderDX11() {
	cleanUp();
}

void ShaderDX11::bind() const {
	
}

void ShaderDX11::prepareUniforms() const {
	dx11->getDeviceContext()->IASetInputLayout(inputLayout);
	dx11->getDeviceContext()->VSSetShader(vertexShader, NULL, 0);
	dx11->getDeviceContext()->PSSetShader(pixelShader, NULL, 0);

	// Bind Uniforms
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = dx11->getDeviceContext()->Map(uniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		Log::println("Error: Failed to create mapped resource for constant buffers in shader binding");
		assert(false);
	}

	struct UniformBuffer {
		Mat4 projection;
		Mat4 view;
		Mat4 transform;
		Vec3 color;
		float padding;
	};
	UniformBuffer buffer;
	buffer.projection = Mat4().initProjectionDX11(35.0f, 800.0f/600.0f, 0.1f, 1000.0f);
	buffer.view = Mat4().initIdentity();
	buffer.transform = Mat4().initIdentity();
	buffer.color = Vec3(1, 0, 1);
	buffer.padding = 1.0;

	float* dataPtr = (float*)mappedResource.pData;
	//memcpy(dataPtr, uniformTempBuffer, sizeUniforms * sizeof(float));
	memcpy(dataPtr, &buffer, sizeof(UniformBuffer));

	dx11->getDeviceContext()->Unmap(uniformBuffer, 0);

	// Setting the constant buffer in the shader
	dx11->getDeviceContext()->VSSetConstantBuffers(0, 1, &uniformBuffer);
	dx11->getDeviceContext()->PSSetConstantBuffers(0, 1, &uniformBuffer);
}

void ShaderDX11::addUniform(const std::string& uniformName) {
	// DO NOT CALL
	Log::println("WARNING: Calling addUniform in shaderDX11 does absolutely nothing!");
}

void ShaderDX11::setUniform1i(const std::string& uniformName, int value) {
	*findUniform(uniformName) = value;
}

void ShaderDX11::setUniform1f(const std::string& uniformName, float x) {
	*findUniform(uniformName) = x;
}

void ShaderDX11::setUniform2f(const std::string& uniformName, float x, float y) {
	float* ptr = findUniform(uniformName);
	*ptr++ = x;
	*ptr++ = y;
}

void ShaderDX11::setUniform3f(const std::string& uniformName, float x, float y, float z) {
	float* ptr = findUniform(uniformName);
	*ptr++ = x;
	*ptr++ = y;
	*ptr++ = z;
}

void ShaderDX11::setUniform4f(const std::string& uniformName, float x, float y, float z, float w) {
	float* ptr = findUniform(uniformName);
	*ptr++ = x;
	*ptr++ = y;
	*ptr++ = z;
	*ptr++ = w;
}

void ShaderDX11::setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size) {
	float* ptr = findUniform(uniformName);
	memcpy(ptr, matrix, size * sizeof(float));
}

void ShaderDX11::setUniformMatrix4f(const std::string& uniformName, const Mat4& matrix) {
	float* ptr = findUniform(uniformName);
	memcpy(ptr, matrix.getHeadPointer(), 16 * sizeof(float));
}

void ShaderDX11::setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix) {
	float* ptr = findUniform(uniformName);
	memcpy(ptr, matrix.getHeadPointer(), 9 * sizeof(float));
}

bool ShaderDX11::initialize(const ShaderInformation& info) {
	std::string vertexPath = info.shaderPath + ".vs";
	std::string pixelPath = info.shaderPath + ".ps";

	HRESULT result;

	int flags = 0;

	ID3D10Blob* compiledVertex;
	ID3D10Blob* compiledPixel;
	ID3D10Blob* compileErrors;

	// Compile vertex shader
	result = D3DX11CompileFromFile((LPCSTR)vertexPath.c_str(), NULL, NULL, "main", "vs_5_0", flags | D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &compiledVertex, &compileErrors, NULL);
	if (FAILED(result)) {
		if (compileErrors) {
			// Log an error statement to inform user to go check at error log
			Log::println("Error: Failed to compile vertex shader. See shader-error.txt for more detail");
			outputShaderError((LPCSTR)vertexPath.c_str(), compileErrors);
		}
		else
			Log::println("Error: Could not find vertex shader");
		assert(false);
		return false;
	}

	// Compile pixel shader
	result = D3DX11CompileFromFile((LPCSTR)pixelPath.c_str(), NULL, NULL, "main", "ps_5_0", flags | D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &compiledPixel, &compileErrors, NULL);
	if (FAILED(result)) {
		if (compileErrors) {
			// Log an error statement to inform user to go check at error log
			Log::println("Error: Failed to compile pixel shader. See shader-error.txt for more detail");
			outputShaderError((LPCSTR)pixelPath.c_str(), compileErrors);
		}
		else
			Log::println("Error: Could not find pixel shader");
		assert(false);
		return false;
	}

	// Create vertex shader
	result = dx11->getDevice()->CreateVertexShader(compiledVertex->GetBufferPointer(), compiledVertex->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result)) {
		// Log error
		Log::println("Error: Vertex Shader could not be created");
		assert(false);
		return false;
	}
	// Create pixel shader
	result = dx11->getDevice()->CreatePixelShader(compiledPixel->GetBufferPointer(), compiledPixel->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result)) {
		// Log error
		Log::println("Error: Pixel Shader could not be created");
		assert(false);
		return false;
	}

	// Create input layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> attributes;
	for (int i = 0; i < info.attributeNames.size(); ++i) {
		D3D11_INPUT_ELEMENT_DESC inputDesc;
		ZeroMemory(&inputDesc, sizeof(inputDesc));

		inputDesc.SemanticName = info.attributeNames[i].c_str();
		inputDesc.SemanticIndex = 0;
		inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDesc.InstanceDataStepRate = 0;
		// Size description
		int size = info.attributeSizes[i];
		if (size == 1)
			inputDesc.Format = DXGI_FORMAT_R32_FLOAT;
		else if (size == 2)
			inputDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		else if (size == 3)
			inputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		else
			inputDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		// Input slot
		inputDesc.InputSlot = i;
		// Appended
		if (i != 0)
			inputDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		else
			inputDesc.AlignedByteOffset = 0;

		attributes.push_back(inputDesc);
	}

	result = dx11->getDevice()->CreateInputLayout(&attributes[0], info.attributeNames.size(), compiledVertex->GetBufferPointer(),
		compiledVertex->GetBufferSize(), &inputLayout);
	if (FAILED(result)) {
		// Log error
		Log::println("Error: Linking of shader failed. Could not create input layout of shader " + info.shaderPath);
		assert(false);
		return false;
	}

	// Texture uniform creation
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;

	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dx11->getDevice()->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(result)) {
		Log::println("Error: Could not create sampler state in shader initialization");
		assert(false);
		return false;
	}

	// Uniform Buffer initialization
	for (int size : info.uniformSizes)
		sizeUniforms += size;

	// Check that uniform size is a multiple of 4
	if (sizeUniforms % 4) {
		Log::println("Fatal Error: Constant uniform buffer in ShaderDX11 " + info.shaderPath + " must be a multiple of 4 in size. Currently : " + std::to_string(sizeUniforms));
		assert(false);
	}

	D3D11_BUFFER_DESC uniformDesc;
	ZeroMemory(&uniformDesc, sizeof(uniformDesc));
	uniformDesc.Usage = D3D11_USAGE_DYNAMIC;
	uniformDesc.ByteWidth = sizeof(float) * sizeUniforms;
	uniformDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	uniformDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = dx11->getDevice()->CreateBuffer(&uniformDesc, nullptr, &uniformBuffer);
	if (FAILED(result)) {
		OutputDebugString("Error: Could not create constant buffer for uniforms\n");
		assert(false);
		return false;
	}

	// Initialize temp uniform buffer
	uniformTempBuffer = new float[sizeUniforms];

	// Add Uniforms individually to a map string -> address
	unsigned int offset = 0;
	for (int i = 0; i < info.uniformNames.size(); ++i) {
		uniforms.insert(std::pair<std::string, int>(info.uniformNames[i], offset));
		offset += info.uniformSizes[i];
	}

	// Release temp data
	compiledVertex->Release();
	compiledVertex = 0;

	compiledPixel->Release();
	compiledPixel = 0;

	if (compileErrors) {
		compileErrors->Release();
		compileErrors = 0;
	}

	return true;
}

bool ShaderDX11::cleanUp() {
	if (inputLayout) {
		inputLayout->Release();
		inputLayout = 0;
	}
	if (pixelShader) {
		pixelShader->Release();
		pixelShader = 0;
	}
	if (vertexShader) {
		vertexShader->Release();
		vertexShader = 0;
	}
	return true;
}

void ShaderDX11::outputShaderError(LPCSTR shaderName, ID3D10Blob* compileErrors) {
	char* errorOutput = (char*)(compileErrors->GetBufferPointer());
	unsigned long bufferSize = compileErrors->GetBufferSize();

	std::ofstream out;
	out.open("shader-error.txt");
	for (int i = 0; i < bufferSize; ++i) {
		out << errorOutput[i];
	}
	out.close();

	compileErrors->Release();
	compileErrors = 0;
}

float* ShaderDX11::findUniform(const std::string& uniformName) const {
	std::map<std::string, int>::const_iterator it = uniforms.find(uniformName);
	if (it == uniforms.end()) {
		Log::println("Warning: Uniform " + uniformName + " not found in setUniform shaderDX11 function");
		return nullptr;
	}
	float* ptr = uniformTempBuffer;
	ptr += it->second;

	return ptr;
}

#endif
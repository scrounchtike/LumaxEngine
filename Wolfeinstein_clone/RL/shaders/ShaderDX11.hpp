
#ifndef SHADER_DX11_HPP
#define SHADER_DX11_HPP

#include "../../RAL/buildDesc.hpp"
#include "../../math.hpp"

#ifdef _USE_DIRECTX11

#include "../../RAL/RenderingContextDX11.hpp"

struct ShaderInformation;

class ShaderDX11 {
public:
	ShaderDX11(const ShaderInformation& info);
	~ShaderDX11();

	void bind() const;
	void prepareUniforms() const;

	void addUniform(const std::string& uniformName);

	void setUniform1i(const std::string& uniformName, int value);
	void setUniform1f(const std::string& uniformName, float x);
	void setUniform2f(const std::string& uniformName, float x, float y);
	void setUniform3f(const std::string& uniformName, float x, float y, float z);
	void setUniform4f(const std::string& uniformName, float x, float y, float z, float w);
	void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size);
	void setUniformMatrix4f(const std::string& uniformName, const Mat4& matrix);
	void setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix);
private:
	bool initialize(const ShaderInformation& info);
	bool cleanUp();

	float* findUniform(const std::string& uniformName) const;

	// Uniforms
	std::map<std::string, int> uniforms;

	unsigned int sizeUniforms;
	float* uniformTempBuffer;

	// Rendering context
	RenderingContextDX11* dx11;

	// Shader DX11 data
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	// Texture
	ID3D11SamplerState* samplerState;

	// Uniform buffer
	ID3D11Buffer* uniformBuffer;

	static void outputShaderError(LPCSTR, ID3D10Blob*);
};

#endif
#endif

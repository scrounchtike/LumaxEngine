
#ifndef SHADER_DX11_HPP
#define SHADER_DX11_HPP

#include "Shader.hpp"
#include "../RAL/buildDesc.hpp"

#ifdef _USE_DIRECTX11

#include "../RAL/RenderingContextDX11.hpp"

class ShaderDX11 : public Shader {
public:
	ShaderDX11(ShaderInformation info);
	~ShaderDX11();

	virtual void bind() const;

	virtual void addUniform(const std::string& uniformName);

	virtual void setUniform1i(const std::string& uniformName, int value);
	virtual void setUniform1f(const std::string& uniformName, float x);
	virtual void setUniform2f(const std::string& uniformName, float x, float y);
	virtual void setUniform2f(const std::string& uniformName, const Vec2& value);
	virtual void setUniform3f(const std::string& uniformName, float x, float y, float z);
	virtual void setUniform3f(const std::string& uniformName, const Vec3& value);
	virtual void setUniform4f(const std::string& uniformName, float x, float y, float z, float w);
	virtual void setUniform4f(const std::string& uniformName, const Vec4& value);
	virtual void setUniform4f(const std::string& uniformName, const Quaternion& value);
	virtual void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size);
	virtual void setUniformMatrix(const std::string& uniformName, const Mat4& matrix);
	virtual void setUniformMatrix(const std::string& uniformName, const Mat3& matrix);
private:
	virtual bool initialize(ShaderInformation info);
	virtual bool cleanUp();

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
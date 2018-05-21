
#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

#include "../RAL/Window.hpp"
#include "../math.hpp"
#include "../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL
class ShaderGL;
#elif defined _USE_DIRECTX11
class ShaderDX11;
#endif

struct ShaderInformation {
	std::string shaderPath;

	std::vector<std::string> uniformNames;
	std::vector<int> uniformSizes;
	std::vector<std::string> attributeNames;
	std::vector<std::string> attributeKeywords;
	std::vector<int> attributeSizes;

	// Instancing
	std::vector<std::string> instanceAttributeNames;
	std::vector<std::string> instanceAttributeKeywords;
	std::vector<int> instanceAttributeSizes;
};

class Shader {
public:
	Shader() {}
	Shader(const ShaderInformation& info);
	~Shader();

	Shader(const Shader& lhs) { shader = lhs.shader; }
	
	void bind() const;
	void prepareUniforms() const;

	void addUniform(const std::string& uniformName);

	void setUniform1i(const std::string& uniformName, int value);
	void setUniform1f(const std::string& uniformName, float x);
	void setUniform2f(const std::string& uniformName, float x, float y);
	void setUniform2f(const std::string& uniformName, const Vec2& value);
	void setUniform3f(const std::string& uniformName, float x, float y, float z);
	void setUniform3f(const std::string& uniformName, const Vec3& value);
	void setUniform4f(const std::string& uniformName, float x, float y, float z, float w);
	void setUniform4f(const std::string& uniformName, const Vec4& value);
 	void setUniform4f(const std::string& uniformName, const Quaternion& value);
	void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size);
	void setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix);
	void setUniformMatrix4f(const std::string& uniformName, const Mat4& matrix);
private:
	// Graphics API dependent shader implementation
#ifdef _USE_OPENGL
	ShaderGL* shader;
#elif defined _USE_DIRECTX11
	ShaderDX11* shader;
#endif
};

#endif

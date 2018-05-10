
#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

#include "../RAL/Window.hpp"
#include "../math.hpp"

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
	virtual ~Shader() { }
	
	virtual void bind() const = 0;

	virtual void addUniform(const std::string& uniformName) = 0;

	virtual void setUniform1i(const std::string& uniformName, int value) = 0;
	virtual void setUniform1f(const std::string& uniformName, float x) = 0;
	virtual void setUniform2f(const std::string& uniformName, float x, float y) = 0;
	virtual void setUniform2f(const std::string& uniformName, const Vec2& value) = 0;
	virtual void setUniform3f(const std::string& uniformName, float x, float y, float z) = 0;
	virtual void setUniform3f(const std::string& uniformName, const Vec3& value) = 0;
	virtual void setUniform4f(const std::string& uniformName, float x, float y, float z, float w) = 0;
	virtual void setUniform4f(const std::string& uniformName, const Vec4& value) = 0;
 	virtual void setUniform4f(const std::string& uniformName, const Quaternion& value) = 0;
	virtual void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size) = 0;
	virtual void setUniformMatrix(const std::string& uniformName, const Mat4& matrix) = 0;
	virtual void setUniformMatrix(const std::string& uniformName, const Mat3& matrix) = 0;
private:
	virtual bool initialize(ShaderInformation info) = 0;
	virtual bool cleanUp() = 0;
};

#endif

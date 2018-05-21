
#include "Shader.hpp"

#ifdef _USE_OPENGL
#include "shaders/ShaderGL.hpp"
#elif defined _USE_DIRECTX11
#include "shaders/ShaderDX11.hpp"
#endif

Shader::Shader(const ShaderInformation& info) {
#ifdef _USE_OPENGL
	shader = new ShaderGL(info);
#elif defined _USE_DIRECTX11
	shader = new ShaderDX11(info);
#endif
}

Shader::~Shader() {
	delete shader;
}

void Shader::bind() const {
	shader->bind();
}

void Shader::prepareUniforms() const {
	shader->prepareUniforms();
}

void Shader::addUniform(const std::string& uniformName) {
	shader->addUniform(uniformName);
}

void Shader::setUniform1i(const std::string& uniformName, int value) {
	shader->setUniform1i(uniformName, value);
}

void Shader::setUniform1f(const std::string& uniformName, float x) {
	shader->setUniform1f(uniformName, x);
}

void Shader::setUniform2f(const std::string& uniformName, float x, float y) {
	shader->setUniform2f(uniformName, x, y);
}

void Shader::setUniform2f(const std::string& uniformName, const Vec2& value) {
	shader->setUniform2f(uniformName, value.x, value.y);
}

void Shader::setUniform3f(const std::string& uniformName, float x, float y, float z) {
	shader->setUniform3f(uniformName, x, y, z);
}

void Shader::setUniform3f(const std::string& uniformName, const Vec3& value) {
	shader->setUniform3f(uniformName, value.x, value.y, value.z);
}

void Shader::setUniform4f(const std::string& uniformName, float x, float y, float z, float w) {
	shader->setUniform4f(uniformName, x, y, z, w);
}

void Shader::setUniform4f(const std::string& uniformName, const Vec4& value) {
	shader->setUniform4f(uniformName, value.x, value.y, value.z, value.z);
}

void Shader::setUniform4f(const std::string& uniformName, const Quaternion& value) {
	shader->setUniform4f(uniformName, value.x, value.y, value.z, value.w);
}

void Shader::setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size) {
	shader->setUniformMatrix(uniformName, matrix, size);
}

void Shader::setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix) {
	shader->setUniformMatrix3f(uniformName, matrix);
}

void Shader::setUniformMatrix4f(const std::string& uniformName, const Mat4& matrix) {
	shader->setUniformMatrix4f(uniformName, matrix);
}
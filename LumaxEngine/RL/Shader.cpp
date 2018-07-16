
#include "Shader.hpp"

#ifdef _USE_OPENGL
#include "shaders/ShaderGL.hpp"
#elif defined _USE_DIRECTX11
#include "shaders/ShaderDX11.hpp"
#endif

Shader::Shader(const std::string& shaderPath) {
#ifdef _USE_OPENGL
	shader = new ShaderGL(shaderPath);
#elif defined _USE_DIRECTX11
	shader = new ShaderDX11(shaderPath);
#endif
}

Shader::Shader(const std::string& shaderPath, unsigned shaderType){
#ifdef _USE_OPENGL
	shader = new ShaderGL(shaderPath, shaderType);
#elif defined _USE_DIRECTX11
	shader = new ShaderDX11(shaderPath, shaderType);
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

void Shader::updateSubroutines() const {
	shader->updateSubroutines();
}

unsigned Shader::getProgram() const {
#ifdef _USE_OPENGL
	return shader->program;
#elif defined _USE_DIRECTX11
	return -1; // TODO: Get unique ID for DX11 shader
#endif
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

void Shader::setSubroutine(const std::string &name, unsigned shaderType, const std::string &function){
	shader->setSubroutine(name, shaderType, function);
}

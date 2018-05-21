
#include "ShaderGL.hpp"
#include "../Shader.hpp"

#ifdef _USE_OPENGL

#include <fstream>

#include "../../RAL/Log.hpp"

ShaderGL::ShaderGL(const ShaderInformation& info) {
	bool success = initialize(info);
	assert(success);
}

ShaderGL::~ShaderGL() {
	bool success = cleanUp();
	assert(success);
}

void ShaderGL::bind() const {
	glUseProgram(program);
}

void ShaderGL::prepareUniforms() const {
	// NOP. Used for DX11 interface compatibility
}

void ShaderGL::addUniform(const std::string& uniformName) {
	GLint location = glGetUniformLocation(program, uniformName.c_str());
	uniforms.insert(std::pair<std::string, int>(uniformName, location));
}

void ShaderGL::setUniform1i(const std::string &uniformName, int value) {
	glUniform1i(uniforms[uniformName], value);
}

void ShaderGL::setUniform1f(const std::string &uniformName, float x) {
	glUniform1f(uniforms[uniformName], x);
}

void ShaderGL::setUniform2f(const std::string &uniformName, float x, float y) {
	glUniform2f(uniforms[uniformName], x, y);
}

void ShaderGL::setUniform3f(const std::string &uniformName, float x, float y, float z) {
	glUniform3f(uniforms[uniformName], x, y, z);
}

void ShaderGL::setUniform4f(const std::string &uniformName, float x, float y, float z, float w) {
	glUniform4f(uniforms[uniformName], x, y, z, w);
}

void ShaderGL::setUniformMatrix(const std::string &uniformName, const float *matrix, unsigned int size) {
	if (size == 16)
		glUniformMatrix4fv(uniforms[uniformName], 1, GL_TRUE, matrix);
	else if (size == 9)
		glUniformMatrix3fv(uniforms[uniformName], 1, GL_TRUE, matrix);
}

void ShaderGL::setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix) {
	glUniformMatrix3fv(uniforms[uniformName], 1, GL_TRUE, matrix.getHeadPointer());
}

void ShaderGL::setUniformMatrix4f(const std::string &uniformName, const Mat4 &matrix) {
	glUniformMatrix4fv(uniforms[uniformName], 1, GL_TRUE, matrix.getHeadPointer());
}

bool ShaderGL::initialize(const ShaderInformation& info) {
	vertexShader = createShader(info.shaderPath + ".vs", GL_VERTEX_SHADER);
	fragmentShader = createShader(info.shaderPath + ".fs", GL_FRAGMENT_SHADER);

	glCompileShader(vertexShader);
	checkError(vertexShader, GL_COMPILE_STATUS, SHADER_TYPE);
	glCompileShader(fragmentShader);
	checkError(fragmentShader, GL_COMPILE_STATUS, SHADER_TYPE);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	checkError(program, GL_LINK_STATUS, PROGRAM_TYPE);

	// Add uniforms
	for (int i = 0; i < info.uniformNames.size(); ++i) {
		addUniform(info.uniformNames[i]);
	}

	return true;
}

bool ShaderGL::cleanUp() {
	return true;
}

GLuint ShaderGL::createShader(const std::string& filename, GLenum type) {
	GLuint shader = glCreateShader(type);

	std::string shaderSourceString = loadShader(filename);
	const GLchar* shaderSource[1] = { shaderSourceString.c_str() };
	GLint shaderSourceLength[1] = { (GLint)shaderSourceString.length() };
	glShaderSource(shader, 1, shaderSource, shaderSourceLength);

	return shader;
}

std::string ShaderGL::loadShader(const std::string& filename) {
	std::ifstream file;
	file.open(filename);

	if (!file.good()) {
		Log::println("Error while trying to read shader source file: " + filename);
		return NULL;
	}

	std::string line;
	std::string result = "";
	while (std::getline(file, line)) {
		result += line + '\n';
	}

	return result;
}

void ShaderGL::checkError(GLuint object, GLenum pname, TYPE type) {
	GLint success = 1;
	if (type == SHADER_TYPE)
		glGetShaderiv(object, pname, &success);
	else if (type == PROGRAM_TYPE)
		glGetProgramiv(object, pname, &success);
	if (!success) {
		std::cerr << "An error occured with shader generation" << std::endl;
		GLchar errorBuf[512];
		GLint length = 0;
		if (type == SHADER_TYPE)
			glGetShaderInfoLog(object, 512, &length, errorBuf);
		else if (type == PROGRAM_TYPE)
			glGetProgramInfoLog(object, 512, &length, errorBuf);
		for (int i = 0; i < length; ++i) {
			std::cerr << errorBuf[i];
		}
		std::cout << std::endl;
		
		// Crash for testing purposes
		assert(false);
	}
}

#endif

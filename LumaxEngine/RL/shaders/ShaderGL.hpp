
#ifndef SHADER_GL_HPP
#define SHADER_GL_HPP

#include <map>

#include "../../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include "../../RAL/UsingOpenGL.hpp"
#include "../../math.hpp"

struct ShaderInformation;

class ShaderGL {
public:
	ShaderGL(const ShaderInformation& info);
	~ShaderGL();

	void bind() const;
	void prepareUniforms() const;

	void addUniform(const std::string& uniformName);

	void setUniform1i(const std::string& uniformName, int value);
	void setUniform1f(const std::string& uniformName, float x);
	void setUniform2f(const std::string& uniformName, float x, float y);
	void setUniform3f(const std::string& uniformName, float x, float y, float z);
	void setUniform4f(const std::string& uniformName, float x, float y, float z, float w);
	void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size);
	void setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix);
	void setUniformMatrix4f(const std::string& uniformName, const Mat4& matrix);
private:
	bool initialize(const ShaderInformation& info);
	bool cleanUp();

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	std::map<std::string, int> uniforms;

	static GLuint createShader(const std::string& filename, GLenum type);
	static std::string loadShader(const std::string& filename);

	enum TYPE {
		SHADER_TYPE, PROGRAM_TYPE
	};
	static void checkError(GLuint shader, GLenum pname, TYPE type);
};

#endif
#endif
